"""
ICT Trading System - Enhanced Main Script
==========================================
Complete enhanced ICT trading system with all components
"""

import sys
import io
import pandas as pd
import numpy as np
from datetime import datetime

# Fix Windows console encoding
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

import sys
import os
# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from analysis.feature_extraction_pipeline import FeatureExtractionPipeline
from rl.enhanced_rl_environment import EnhancedICTTradingEnv, CurriculumLearningCallback, PerformanceMonitorCallback
from utils.utils import generate_sample_data
from stable_baselines3 import PPO
from stable_baselines3.common.vec_env import DummyVecEnv


def print_banner():
    """Print system banner"""
    print("\n" + "="*80)
    print("╔" + "═"*78 + "╗")
    print("║" + " "*20 + "ENHANCED ICT TRADING SYSTEM" + " "*32 + "║")
    print("║" + " "*78 + "║")
    print("║  Features: PD Arrays | Market Structure | Liquidity | Sessions | LLM  ║")
    print("║            Order Flow | Price Attractors | Protected Arrays | RL      ║")
    print("╚" + "═"*78 + "╝")
    print("="*80 + "\n")


def demonstrate_feature_extraction(use_llm: bool = False):
    """Demonstrate the enhanced feature extraction pipeline"""
    print_banner()

    print("STEP 1: Generating Sample Market Data")
    print("-" * 80)

    # Generate sample data with timestamps
    data = generate_sample_data(
        timeframes=['1h'],
        num_candles=500,
        start_price=50000
    )

    df = data['1h']

    # Add timestamps if not present
    if 'timestamp' not in df.columns:
        df['timestamp'] = pd.date_range(start='2024-01-01', periods=len(df), freq='1H')

    print(f"✅ Generated {len(df)} candles")
    print(f"   Price range: ${df['close'].min():.2f} - ${df['close'].max():.2f}")
    print()

    # Initialize feature extraction pipeline
    print("STEP 2: Initializing Feature Extraction Pipeline")
    print("-" * 80)

    pipeline = FeatureExtractionPipeline(
        df=df,
        timeframe='1h',
        use_llm=use_llm,
        llm_api_key=None  # Will use env var or fallback
    )

    print("\nSTEP 3: Extracting Features for Latest Candle")
    print("-" * 80)

    latest_idx = len(df) - 1
    features = pipeline.extract_features(latest_idx)

    print(f"✅ Extracted {len(features)} features")
    print(f"   Feature vector shape: {features.shape}")
    print(f"   Feature range: [{features.min():.4f}, {features.max():.4f}]")
    print()

    # Get trading signal
    print("STEP 4: Generating Trading Signal")
    print("-" * 80)

    signal = pipeline.get_trading_signal(latest_idx)
    current_price = pipeline.get_current_price(latest_idx)

    print(f"\n📊 TRADING SIGNAL at ${current_price:.2f}")
    print(f"   Recommendation: {signal['recommendation']}")
    print(f"   Confidence: {signal['confidence']:.1f}%")
    print(f"   Bullish Score: {signal['bullish_score']}")
    print(f"   Bearish Score: {signal['bearish_score']}")
    print()

    # Display component summaries
    print("STEP 5: Component Analysis Summary")
    print("-" * 80)

    print(f"\n🔷 PD Arrays: {len(pipeline.pd_arrays)} total")
    print(f"   - Protected: {len(pipeline.protected_arrays)}")
    print(f"   - Order Blocks: {sum(1 for a in pipeline.pd_arrays if a.type == 'OB')}")
    print(f"   - Fair Value Gaps: {sum(1 for a in pipeline.pd_arrays if a.type == 'FVG')}")
    print(f"   - Breakers: {sum(1 for a in pipeline.pd_arrays if a.type == 'Breaker')}")

    print(f"\n🔧 Market Structure:")
    print(f"   - Trend: {pipeline.structure_analysis['current_trend']}")
    print(f"   - Structure Breaks: {len(pipeline.structure_analysis['structure_breaks'])}")
    print(f"   - Swing Highs: {len(pipeline.structure_analysis['swing_highs'])}")
    print(f"   - Swing Lows: {len(pipeline.structure_analysis['swing_lows'])}")

    print(f"\n💧 Liquidity:")
    print(f"   - Total Zones: {len(pipeline.liquidity_analysis['liquidity_zones'])}")
    print(f"   - Swept Zones: {len(pipeline.liquidity_analysis['swept_liquidity'])}")
    print(f"   - Attraction Vectors: {len(pipeline.liquidity_analysis['liquidity_vectors'])}")

    print(f"\n🧲 Price Attractors:")
    print(f"   - Total Attractors: {len(pipeline.attractors)}")
    net_attr = pipeline.attractor_system.calculate_net_attraction(current_price)
    print(f"   - Net Direction: {net_attr['net_direction']}")
    print(f"   - Upward Attraction: {net_attr['upward_attraction']:.1f}")
    print(f"   - Downward Attraction: {net_attr['downward_attraction']:.1f}")

    print(f"\n📈 Order Flow:")
    print(f"   - Signals: {len(pipeline.order_flow_analysis['order_flow_signals'])}")

    print("\n" + "="*80)
    print("✅ FEATURE EXTRACTION DEMONSTRATION COMPLETE")
    print("="*80 + "\n")

    return pipeline


def train_rl_agent(pipeline: FeatureExtractionPipeline, timesteps: int = 50000):
    """Train RL agent on the enhanced environment"""
    print("\nSTEP 6: Training RL Agent")
    print("-" * 80)

    # Create environment
    print("🏋️  Creating RL environment...")

    def make_env():
        return EnhancedICTTradingEnv(
            feature_extractor=pipeline,
            initial_balance=10000,
            teacher_weight=1.0
        )

    env = DummyVecEnv([make_env])

    print(f"   ✓ Environment created")
    print(f"   ✓ Observation space: {env.observation_space.shape}")
    print(f"   ✓ Action space: {env.action_space.n} actions")

    # Create agent
    print("\n🤖 Initializing PPO agent...")

    model = PPO(
        "MlpPolicy",
        env,
        learning_rate=0.0003,
        n_steps=2048,
        batch_size=64,
        n_epochs=10,
        gamma=0.99,
        gae_lambda=0.95,
        clip_range=0.2,
        verbose=1,
        tensorboard_log="./tensorboard_logs/"
    )

    print("   ✓ PPO agent created")

    # Setup callbacks
    curriculum_callback = CurriculumLearningCallback(
        initial_weight=1.0,
        final_weight=0.1,
        decay_steps=timesteps,
        verbose=1
    )

    monitor_callback = PerformanceMonitorCallback(
        check_freq=1000,
        verbose=1
    )

    print(f"\n📚 Training for {timesteps} timesteps...")
    print("   (This may take several minutes...)\n")

    # Train
    try:
        model.learn(
            total_timesteps=timesteps,
            callback=[curriculum_callback, monitor_callback],
            progress_bar=True
        )

        print("\n✅ Training complete!")

        # Save model
        model_path = "enhanced_ict_agent.zip"
        model.save(model_path)
        print(f"   ✓ Model saved to {model_path}")

    except KeyboardInterrupt:
        print("\n⚠️  Training interrupted by user")
        return None

    except Exception as e:
        print(f"\n❌ Training failed: {e}")
        return None

    return model


def test_trained_agent(pipeline: FeatureExtractionPipeline, model):
    """Test the trained agent"""
    print("\nSTEP 7: Testing Trained Agent")
    print("-" * 80)

    env = EnhancedICTTradingEnv(
        feature_extractor=pipeline,
        initial_balance=10000,
        teacher_weight=0.0  # No teacher guidance during testing
    )

    obs, _ = env.reset()
    total_reward = 0
    steps = 0
    done = False

    print("🧪 Running test episode...\n")

    while not done and steps < 100:
        action, _ = model.predict(obs, deterministic=True)
        obs, reward, done, truncated, info = env.step(action)

        total_reward += reward
        steps += 1

        if steps % 10 == 0:
            print(f"   Step {steps}: Balance = ${info['balance']:.2f}, Profit = {info['total_profit']:.2%}")

    print(f"\n✅ Test episode complete!")
    print(f"   Total Steps: {steps}")
    print(f"   Total Reward: {total_reward:.2f}")
    print(f"   Final Balance: ${info['balance']:.2f}")
    print(f"   Total Profit: {info['total_profit']:.2%}")
    print(f"   Number of Trades: {info['num_trades']}")
    print(f"   Win Rate: {info['win_rate']:.1%}")

    print("\n" + "="*80)
    print("✅ COMPLETE SYSTEM TEST FINISHED")
    print("="*80 + "\n")


def main():
    """Main execution function"""
    print_banner()

    print("Welcome to the Enhanced ICT Trading System!")
    print("This system integrates:")
    print("  • Advanced PD Array detection (OB, FVG, Breaker, EQH/EQL)")
    print("  • Market Structure analysis (BOS, CHOCH)")
    print("  • Liquidity zones and vectors")
    print("  • Session analysis (Kill Zones)")
    print("  • Price attractors and protected arrays")
    print("  • Order flow analysis")
    print("  • LLM integration (optional)")
    print("  • Enhanced RL agent with 100+ features")
    print()

    # Demonstrate feature extraction
    use_llm = input("Enable LLM analysis? (y/n, default=n): ").lower() == 'y'

    pipeline = demonstrate_feature_extraction(use_llm=use_llm)

    # Ask about RL training
    train_rl = input("\nWould you like to train the RL agent? (y/n, default=n): ").lower() == 'y'

    if train_rl:
        timesteps_input = input("Enter training timesteps (default=50000): ")
        timesteps = int(timesteps_input) if timesteps_input.isdigit() else 50000

        model = train_rl_agent(pipeline, timesteps)

        if model:
            # Test the agent
            test_agent = input("\nWould you like to test the trained agent? (y/n, default=y): ").lower() != 'n'

            if test_agent:
                test_trained_agent(pipeline, model)

    print("\n" + "="*80)
    print("Thank you for using the Enhanced ICT Trading System!")
    print("="*80 + "\n")


if __name__ == "__main__":
    main()
