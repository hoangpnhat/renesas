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
import torch
import asyncio
from concurrent.futures import ThreadPoolExecutor
import queue
import threading

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Model configuration
MODEL_NAME = "hoangpnhat/qwen2.5-coder-7b-awq"

class OptimizedQwenCoderWrapper(mlflow.pyfunc.PythonModel):
    """Speed-optimized QwenCoder wrapper for T4"""
    
    def __init__(self):
        self.llm = None
        self.tokenizer = None
        self.request_queue = queue.Queue()
        self.batch_processor = None
        self.is_processing = False
        
    def _get_optimized_vllm_config(self) -> Dict[str, Any]:
        """Get speed-optimized vLLM configuration for T4"""
        config = {
            "model": MODEL_NAME,
            "quantization": "awq",
            "dtype": "half",
            "max_model_len": 2048,
            "tensor_parallel_size": 1,
            "trust_remote_code": True,
            "gpu_memory_utilization": 0.90,
            "max_num_seqs": 8,
            "max_num_batched_tokens": 8192,
            "enable_prefix_caching": True,
            "enable_chunked_prefill": True,
            "block_size": 16,
            "swap_space": 2,
            "enforce_eager": True,
            "disable_log_stats": False,
        }
        return config
    
    def load_context(self, context):
        """Load model with speed optimizations"""
        logger.info("Loading speed-optimized model for T4...")
        vllm_config = self._get_optimized_vllm_config()
        try:
            self.llm = LLM(**vllm_config)
            self.tokenizer = AutoTokenizer.from_pretrained(
                MODEL_NAME, 
                trust_remote_code=True,
                use_fast=True
            )
            self._warmup_model()
            self._start_batch_processor()
            logger.info("Speed-optimized model loaded successfully")
        except Exception as e:
            logger.error(f"Failed to load model: {str(e)}")
            raise
    
    def _warmup_model(self):
        """Pre-warm the model to avoid cold start delays"""
        logger.info("Warming up model...")
        warmup_prompts = [
            "Hello, how are you?",
            "Write a Python function",
            "Explain machine learning",
        ]
        sampling_params = SamplingParams(
            temperature=0.1,
            top_p=0.95,
            max_tokens=10,
        )
        self.llm.generate(warmup_prompts, sampling_params)
        logger.info("Model warmed up successfully")
    
    def _start_batch_processor(self):
        """Start background batch processor for better throughput"""
        self.is_processing = True
        self.batch_processor = threading.Thread(target=self._process_batch_requests)
        self.batch_processor.daemon = True
        self.batch_processor.start()
        logger.info("Batch processor started")
    
    def _process_batch_requests(self):
        batch_size = 1
        timeout = 0.01
        while self.is_processing:
            batch = []
            start_time = time.time()
            while len(batch) < batch_size and (time.time() - start_time) < timeout:
                try:
                    request = self.request_queue.get(timeout=timeout)
                    batch.append(request)
                except queue.Empty:
                    break
            if batch:
                self._process_request_batch(batch)
    
    def _process_request_batch(self, batch):
        try:
            prompts = [req['prompt'] for req in batch]
            sampling_params = [req['sampling_params'] for req in batch]
            outputs = self.llm.generate(prompts, sampling_params[0])
            for i, output in enumerate(outputs):
                batch[i]['result_queue'].put(output.outputs[0].text)
        except Exception as e:
            logger.error(f"Batch processing error: {e}")
            for req in batch:
                req['result_queue'].put(f"Error: {str(e)}")
    
    def format_chat_prompt(self, messages: List[Dict[str, str]]) -> str:
        formatted_prompt = ""
        for message in messages:
            formatted_prompt += message.get("content", "")
        return formatted_prompt
    
    def count_tokens(self, text: str) -> int:
        try:
            tokens = self.tokenizer.encode(text, add_special_tokens=False)
            return len(tokens)
        except:
            return int(len(text.split()) * 1.3)
    
    def predict(self, context, model_input):
        if isinstance(model_input, pd.DataFrame):
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

        if isinstance(temperature, pd.Series):
            temperature = temperature.iloc[0]
        if isinstance(top_p, pd.Series):
            top_p = top_p.iloc[0]
        if isinstance(max_tokens, pd.Series):
            max_tokens = max_tokens.iloc[0]

        # Ensure native Python types
        temperature = float(temperature)
        top_p = float(top_p)
        max_tokens = int(max_tokens)

        formatted_prompt = self.format_chat_prompt(messages)
        
        sampling_params = SamplingParams(
            temperature=temperature,
            top_p=top_p,
            max_tokens=max_tokens,
            repetition_penalty=1.0,
            presence_penalty=0.0,
            frequency_penalty=0.0,
            skip_special_tokens=True,
        )

        start_time = time.time()
        try:
            outputs = self.llm.generate([formatted_prompt], sampling_params)
            generated_text = outputs[0].outputs[0].text
            generation_time = time.time() - start_time
            prompt_tokens = self.count_tokens(formatted_prompt)
            completion_tokens = self.count_tokens(generated_text)
            total_tokens = prompt_tokens + completion_tokens
            tokens_per_second = completion_tokens / generation_time if generation_time > 0 else 0
            finish_reason = "stop"
            if completion_tokens >= max_tokens:
                finish_reason = "length"
            return {
                "model": MODEL_NAME,
                "choices": [
                    {
                        "message": {
                            "role": "assistant",
                            "content": generated_text.strip()
                        },
                        "index": 0,
                        "finish_reason": finish_reason
                    }
                ],
                "usage": {
                    "prompt_tokens": prompt_tokens,
                    "completion_tokens": completion_tokens,
                    "total_tokens": total_tokens
                },
                "object": "chat.completion",
                "id": f"msg_{uuid.uuid4().hex[:20]}",
                "created": int(time.time()),
                "performance": {
                    "generation_time_ms": int(generation_time * 1000),
                    "tokens_per_second": round(tokens_per_second, 2),
                    "gpu_memory_used": "T4-Optimized"
                }
            }
        except Exception as e:
            logger.error(f"Error generating completion: {str(e)}")
            raise
    
    def __del__(self):
        if hasattr(self, 'is_processing'):
            self.is_processing = False

# Set the model for MLflow
set_model(OptimizedQwenCoderWrapper())
