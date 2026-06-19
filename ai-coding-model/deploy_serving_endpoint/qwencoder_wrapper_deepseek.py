import os
import json
import time
import uuid
import pandas as pd
from vllm import LLM, SamplingParams
from transformers import AutoTokenizer
from typing import List, Dict, Any, Optional
import logging
import mlflow.pyfunc
from mlflow.models import set_model

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


MODEL_NAME = "hoangpnhat/deepseek-coder-6.7b-base-awq"

# vLLM configuration
VLLM_CONFIG = {
    "model": MODEL_NAME,
    "quantization": "awq",
    "dtype": "half",
    "max_model_len": 8192,
    "gpu_memory_utilization": 0.8,
    "tensor_parallel_size": 1,
    "trust_remote_code": True,
}


class QwenCoderWrapper(mlflow.pyfunc.PythonModel):
    """OpenAI-compatible chat completion API for Qwen2.5-Coder"""

    def __init__(self):
        self.llm = None
        self.tokenizer = None

    def load_context(self, context):
        """Load model context for MLflow"""
        # Model configuration

        logger.info("Loading model in MLflow context...")
        self.llm = LLM(**VLLM_CONFIG)
        self.tokenizer = AutoTokenizer.from_pretrained(
            MODEL_NAME, trust_remote_code=True
        )
        logger.info("Model loaded successfully in MLflow context")

    def format_chat_prompt(self, messages: List[Dict[str, str]]) -> str:
        """Convert chat messages to Qwen2.5 format"""
        formatted_prompt = ""

        for message in messages:
            formatted_prompt += message.get("content", "")

        return formatted_prompt

    def count_tokens(self, text: str) -> int:
        """Count tokens in text"""
        try:
            tokens = self.tokenizer.encode(text)
            return len(tokens)
        except:
            # Fallback estimation
            return int(len(text.split()) * 1.3)

    def predict(self, context, model_input):
        """MLflow compatible predict method, supporting both dict and DataFrame input formats"""

        # Handle both JSON input (dict) and MLflow default DataFrame input
        if isinstance(model_input, pd.DataFrame):
            # Extract first row
            row = model_input.iloc[0]
            messages = row["messages"]
            max_tokens = row.get("max_tokens", 128)
            temperature = row.get("temperature", 0.1)
            top_p = row.get("top_p", 0.95)
        elif isinstance(model_input, dict):
            messages = model_input["messages"]
            max_tokens = model_input.get("max_tokens", 128)
            temperature = model_input.get("temperature", 0.1)
            top_p = model_input.get("top_p", 0.95)
        else:
            raise ValueError(f"Unsupported model_input type: {type(model_input)}")

        # If temperature accidentally became a Series, extract scalar
        if isinstance(temperature, pd.Series):
            temperature = temperature.iloc[0]
        if isinstance(top_p, pd.Series):
            top_p = top_p.iloc[0]
        if isinstance(max_tokens, pd.Series):
            max_tokens = max_tokens.iloc[0]

        # Format the prompt
        formatted_prompt = self.format_chat_prompt(messages)

        # Count prompt tokens
        prompt_tokens = self.count_tokens(formatted_prompt)

        # Configure sampling parameters
        sampling_params = SamplingParams(
            temperature=temperature,
            top_p=top_p,
            max_tokens=max_tokens,
        )

        # Generate response
        try:
            outputs = self.llm.generate([formatted_prompt], sampling_params)
            generated_text = outputs[0].outputs[0].text

            # Count completion tokens
            completion_tokens = self.count_tokens(generated_text)
            total_tokens = prompt_tokens + completion_tokens

            # Determine finish reason
            finish_reason = "stop"
            if completion_tokens >= max_tokens:
                finish_reason = "length"

            return {
                "model": MODEL_NAME,
                "choices": [
                    {
                        "message": {
                            "role": "assistant",
                            "content": generated_text.strip(),
                        },
                        "index": 0,
                        "finish_reason": finish_reason,
                    }
                ],
                "usage": {
                    "prompt_tokens": prompt_tokens,
                    "completion_tokens": completion_tokens,
                    "total_tokens": total_tokens,
                },
                "object": "chat.completion",
                "id": f"msg_{uuid.uuid4().hex[:20]}",
                "created": int(time.time()),
            }

        except Exception as e:
            logger.error(f"Error generating completion: {str(e)}")
            raise


# Set the model for MLflow
set_model(QwenCoderWrapper())
