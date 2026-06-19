"""
ICT Trading System - Enhanced LLM Analyzer
===========================================
Advanced LLM features: Strategy Creation, RL Help, Price Analysis, Agent Interpretation
"""

import pandas as pd
import numpy as np
from typing import Dict, List, Optional
import os
from openai import OpenAI


class EnhancedLLMAnalyzer:
    """Enhanced LLM analyzer with advanced trading intelligence"""

    def __init__(self, api_key: str = None):
        """Initialize enhanced LLM analyzer"""
        self.api_key = api_key or os.getenv('OPENAI_API_KEY')
        self.client = None
        self.use_real_llm = False

        if self.api_key:
            try:
                self.client = OpenAI(api_key=self.api_key)
                self.use_real_llm = True
                print("✅ Enhanced LLM Analyzer: Using OpenAI GPT-4o")
            except Exception as e:
                print(f"⚠️  Failed to initialize OpenAI: {e}")
                print("   Falling back to rule-based analysis")
                self.use_real_llm = False
        else:
            print("⚠️  No OpenAI API key provided")
            print("   Using rule-based analysis")

    # ==================== STRATEGY CREATION ====================

    def create_strategy_from_description(self, description: str) -> Dict:
        """Convert natural language strategy description to executable logic"""
        if not self.use_real_llm:
            return self._default_strategy_response()

        try:
            response = self.client.chat.completions.create(
                model="gpt-4o",
                messages=[
                    {
                        "role": "system",
                        "content": """You are an expert ICT trading strategy designer.
Convert user's natural language strategy descriptions into structured trading rules.

Respond in JSON format:
{
  "strategy_name": "name",
  "entry_conditions": ["condition1", "condition2"],
  "exit_conditions": ["condition1", "condition2"],
  "risk_management": {"stop_loss": "description", "take_profit": "description"},
  "timeframes": ["1h", "4h"],
  "confidence": 85,
  "implementation_notes": "notes"
}"""
                    },
                    {
                        "role": "user",
                        "content": f"Create a trading strategy: {description}"
                    }
                ],
                temperature=0.7,
                max_tokens=500
            )

            import json
            result_text = response.choices[0].message.content.strip()

            if '```json' in result_text:
                result_text = result_text.split('```json')[1].split('```')[0].strip()
            elif '```' in result_text:
                result_text = result_text.split('```')[1].split('```')[0].strip()

            strategy = json.loads(result_text)
            return strategy

        except Exception as e:
            print(f"⚠️  Strategy creation failed: {e}")
            return self._default_strategy_response()

    # ==================== RL TRAINING HELP ====================

    def analyze_rl_performance(self, training_metrics: Dict) -> Dict:
        """Analyze RL agent performance and provide training recommendations"""
        if not self.use_real_llm:
            return self._default_rl_analysis()

        try:
            metrics_summary = f"""
Training Metrics:
- Total Steps: {training_metrics.get('total_steps', 0)}
- Average Reward: {training_metrics.get('avg_reward', 0):.2f}
- Win Rate: {training_metrics.get('win_rate', 0):.2%}
- Sharpe Ratio: {training_metrics.get('sharpe_ratio', 0):.2f}
- Max Drawdown: {training_metrics.get('max_drawdown', 0):.2%}
- Recent Episode Rewards: {training_metrics.get('recent_rewards', [])}
"""

            response = self.client.chat.completions.create(
                model="gpt-4o-mini",
                messages=[
                    {
                        "role": "system",
                        "content": """You are an RL training expert for trading systems.
Analyze training metrics and provide actionable recommendations.

Respond in JSON format:
{
  "assessment": "good/needs_improvement/poor",
  "key_issues": ["issue1", "issue2"],
  "recommendations": ["rec1", "rec2"],
  "hyperparameter_suggestions": {"param": "value"},
  "confidence": 85
}"""
                    },
                    {
                        "role": "user",
                        "content": metrics_summary
                    }
                ],
                temperature=0.3,
                max_tokens=300
            )

            import json
            result_text = response.choices[0].message.content.strip()

            if '```json' in result_text:
                result_text = result_text.split('```json')[1].split('```')[0].strip()
            elif '```' in result_text:
                result_text = result_text.split('```')[1].split('```')[0].strip()

            analysis = json.loads(result_text)
            return analysis

        except Exception as e:
            print(f"⚠️  RL analysis failed: {e}")
            return self._default_rl_analysis()

    def suggest_reward_shaping(self, current_reward_structure: str) -> Dict:
        """Suggest improvements to reward function"""
        if not self.use_real_llm:
            return {'suggestions': ['Use profit-weighted rewards', 'Add structure alignment bonus']}

        try:
            response = self.client.chat.completions.create(
                model="gpt-4o-mini",
                messages=[
                    {
                        "role": "system",
                        "content": "You are an RL reward engineering expert. Suggest reward function improvements for trading agents."
                    },
                    {
                        "role": "user",
                        "content": f"Current reward structure: {current_reward_structure}\n\nSuggest improvements."
                    }
                ],
                temperature=0.7,
                max_tokens=200
            )

            return {'suggestions': response.choices[0].message.content.strip().split('\n')}

        except:
            return {'suggestions': ['Use profit-weighted rewards', 'Add structure alignment bonus']}

    # ==================== PRICE HISTORY ANALYSIS ====================

    def analyze_price_history(self, df: pd.DataFrame, analysis_type: str = "comprehensive") -> Dict:
        """Deep analysis of price history using LLM"""
        if not self.use_real_llm:
            return self._default_price_analysis(df)

        # Prepare price summary
        recent = df.tail(50)
        price_summary = f"""
Price History Analysis Request ({analysis_type}):

Current Price: ${recent['close'].iloc[-1]:.2f}
24h Change: {((recent['close'].iloc[-1] - recent['close'].iloc[0]) / recent['close'].iloc[0] * 100):.2f}%
Recent High: ${recent['high'].max():.2f}
Recent Low: ${recent['low'].min():.2f}

Price Momentum: {'Bullish' if recent['close'].iloc[-1] > recent['close'].iloc[0] else 'Bearish'}

Last 10 Close Prices: {[f"${p:.2f}" for p in recent['close'].tail(10).tolist()]}

Please analyze:
1. Overall trend and momentum
2. Key support/resistance levels
3. Pattern recognition
4. Likely next moves
"""

        try:
            response = self.client.chat.completions.create(
                model="gpt-4o",
                messages=[
                    {
                        "role": "system",
                        "content": """You are an expert technical analyst specializing in ICT concepts.
Analyze price history and provide insights.

Respond in JSON format:
{
  "overall_trend": "bullish/bearish/neutral",
  "trend_strength": 0-100,
  "key_levels": {"support": [], "resistance": []},
  "patterns_detected": ["pattern1", "pattern2"],
  "next_move_prediction": "description",
  "confidence": 0-100,
  "reasoning": "explanation"
}"""
                    },
                    {
                        "role": "user",
                        "content": price_summary
                    }
                ],
                temperature=0.3,
                max_tokens=400
            )

            import json
            result_text = response.choices[0].message.content.strip()

            if '```json' in result_text:
                result_text = result_text.split('```json')[1].split('```')[0].strip()
            elif '```' in result_text:
                result_text = result_text.split('```')[1].split('```')[0].strip()

            analysis = json.loads(result_text)
            return analysis

        except Exception as e:
            print(f"⚠️  Price analysis failed: {e}")
            return self._default_price_analysis(df)

    # ==================== AGENT INTERPRETATION ====================

    def interpret_agent_decision(self, state: np.ndarray, action: int,
                                 feature_names: List[str], q_values: Optional[np.ndarray] = None) -> Dict:
        """Explain why RL agent made a specific decision"""
        if not self.use_real_llm:
            return self._default_agent_interpretation(action)

        # Prepare state summary
        state_summary = "Agent State Features:\n"
        for i, (feature, value) in enumerate(zip(feature_names[:20], state[:20])):
            state_summary += f"- {feature}: {value:.4f}\n"

        action_map = {0: 'HOLD', 1: 'BUY', 2: 'SELL'}
        action_text = action_map.get(action, 'UNKNOWN')

        query = f"""
{state_summary}

Agent Action: {action_text}
Q-Values: {q_values.tolist() if q_values is not None else 'N/A'}

Explain why the agent chose this action based on the state features.
"""

        try:
            response = self.client.chat.completions.create(
                model="gpt-4o-mini",
                messages=[
                    {
                        "role": "system",
                        "content": """You are an RL agent interpreter. Explain trading decisions in human terms.

Respond in JSON format:
{
  "decision": "BUY/SELL/HOLD",
  "primary_factors": ["factor1", "factor2"],
  "reasoning": "explanation",
  "confidence": 0-100,
  "risk_assessment": "low/medium/high"
}"""
                    },
                    {
                        "role": "user",
                        "content": query
                    }
                ],
                temperature=0.3,
                max_tokens=250
            )

            import json
            result_text = response.choices[0].message.content.strip()

            if '```json' in result_text:
                result_text = result_text.split('```json')[1].split('```')[0].strip()
            elif '```' in result_text:
                result_text = result_text.split('```')[1].split('```')[0].strip()

            interpretation = json.loads(result_text)
            return interpretation

        except Exception as e:
            print(f"⚠️  Agent interpretation failed: {e}")
            return self._default_agent_interpretation(action)

    # ==================== MARKET CONTEXT ANALYSIS (Enhanced) ====================

    def analyze_market_context(self, df: pd.DataFrame, current_idx: int,
                               ict_features: Dict, structure_features: Dict,
                               liquidity_features: Dict, session_features: Dict) -> Dict:
        """Comprehensive market analysis using all ICT features"""
        if not self.use_real_llm:
            return self._default_context_analysis()

        # Build comprehensive context
        context = f"""
Comprehensive ICT Market Analysis:

=== PRICE ACTION ===
Current Price: ${df['close'].iloc[current_idx]:.2f}
24h Change: {((df['close'].iloc[current_idx] - df['close'].iloc[max(0, current_idx-24)]) / df['close'].iloc[max(0, current_idx-24)] * 100):.2f}%

=== MARKET STRUCTURE ===
Trend: {structure_features.get('trend', 'N/A')}
Recent BOS: {structure_features.get('recent_bos_count', 0)}
Recent CHOCH: {structure_features.get('recent_choch_count', 0)}
Structure Alignment: {structure_features.get('structure_alignment', 0):.2f}

=== LIQUIDITY ===
Buy-Side Zones: {liquidity_features.get('buy_side_zones', 0)}
Sell-Side Zones: {liquidity_features.get('sell_side_zones', 0)}
Net Attraction: {liquidity_features.get('net_attraction_direction', 'N/A')}
Imbalance: {liquidity_features.get('liquidity_imbalance', 0):.2f}

=== SESSION ===
Current Session: {'Kill Zone' if session_features.get('is_killzone', 0) else 'Regular'}
Session Priority: {session_features.get('session_priority', 0):.1f}/1.0

=== ICT SIGNALS ===
{ict_features}

Provide comprehensive trading recommendation.
"""

        try:
            response = self.client.chat.completions.create(
                model="gpt-4o",
                messages=[
                    {
                        "role": "system",
                        "content": """You are a master ICT trader analyzing comprehensive market data.

Respond in JSON format:
{
  "market_bias": "BULLISH/BEARISH/NEUTRAL",
  "confidence": 0-100,
  "recommendation": "BUY/SELL/HOLD/WAIT",
  "entry_zone": "price range",
  "stop_loss": "level",
  "take_profit": "levels",
  "key_factors": ["factor1", "factor2", "factor3"],
  "reasoning": "detailed explanation",
  "timeframe_alignment": "aligned/mixed/conflicted"
}"""
                    },
                    {
                        "role": "user",
                        "content": context
                    }
                ],
                temperature=0.3,
                max_tokens=500
            )

            import json
            result_text = response.choices[0].message.content.strip()

            if '```json' in result_text:
                result_text = result_text.split('```json')[1].split('```')[0].strip()
            elif '```' in result_text:
                result_text = result_text.split('```')[1].split('```')[0].strip()

            analysis = json.loads(result_text)
            return analysis

        except Exception as e:
            print(f"⚠️  Market context analysis failed: {e}")
            return self._default_context_analysis()

    # ==================== FALLBACK METHODS ====================

    def _default_strategy_response(self) -> Dict:
        return {
            "strategy_name": "Basic ICT Strategy",
            "entry_conditions": ["Bullish OB in discount", "BOS confirmation"],
            "exit_conditions": ["Take profit at liquidity", "Stop at OB invalidation"],
            "risk_management": {"stop_loss": "Below OB", "take_profit": "1:3 RR"},
            "timeframes": ["1h", "4h"],
            "confidence": 70,
            "implementation_notes": "Rule-based strategy"
        }

    def _default_rl_analysis(self) -> Dict:
        return {
            "assessment": "needs_improvement",
            "key_issues": ["More training needed"],
            "recommendations": ["Increase exploration", "Adjust learning rate"],
            "hyperparameter_suggestions": {"learning_rate": "0.0001"},
            "confidence": 60
        }

    def _default_price_analysis(self, df: pd.DataFrame) -> Dict:
        recent = df.tail(20)
        trend = "bullish" if recent['close'].iloc[-1] > recent['close'].iloc[0] else "bearish"

        return {
            "overall_trend": trend,
            "trend_strength": 60,
            "key_levels": {"support": [recent['low'].min()], "resistance": [recent['high'].max()]},
            "patterns_detected": ["Range"],
            "next_move_prediction": "Continuation likely",
            "confidence": 65,
            "reasoning": "Rule-based analysis"
        }

    def _default_agent_interpretation(self, action: int) -> Dict:
        action_map = {0: 'HOLD', 1: 'BUY', 2: 'SELL'}
        return {
            "decision": action_map.get(action, 'HOLD'),
            "primary_factors": ["Market conditions", "Risk assessment"],
            "reasoning": "Decision based on learned policy",
            "confidence": 70,
            "risk_assessment": "medium"
        }

    def _default_context_analysis(self) -> Dict:
        return {
            "market_bias": "NEUTRAL",
            "confidence": 60,
            "recommendation": "WAIT",
            "entry_zone": "N/A",
            "stop_loss": "N/A",
            "take_profit": "N/A",
            "key_factors": ["Insufficient data"],
            "reasoning": "Rule-based fallback analysis",
            "timeframe_alignment": "mixed"
        }
