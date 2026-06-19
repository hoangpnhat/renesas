"""
ICT Trading Bot - Setup and Run
===============================
Configure API key and start trading
"""

import sys
import io

# Fix Windows console encoding
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', line_buffering=True)
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', line_buffering=True)

import config


def setup_api_key():
    """Interactive API key setup"""
    print("=" * 80)
    print("ICT TRADING BOT - SETUP")
    print("=" * 80)
    print("\n📝 OpenAI API Key Configuration")
    print("\nTo enable GPT-4o-mini analysis, you need an OpenAI API key.")
    print("Get your key from: https://platform.openai.com/api-keys")
    print("\nPress Enter to skip and use rule-based analysis instead.")
    print("=" * 80)

    api_key = input("\nEnter your OpenAI API key: ").strip()

    if api_key:
        config.set_openai_key(api_key)
        print("\n✅ API key configured! GPT-4o-mini will be used for market analysis.")
    else:
        print("\n⚠️  No API key provided. Using rule-based analysis.")

    return bool(api_key)


def main():
    """Main setup and run"""
    # Setup API key
    has_api_key = setup_api_key()

    print("\n" + "=" * 80)
    print("BOT CONFIGURATION")
    print("=" * 80)

    # Get bot parameters
    print("\nDefault settings:")
    print(f"  Symbol: BTCUSDT")
    print(f"  Initial Balance: ${config.DEFAULT_INITIAL_BALANCE:,.2f}")
    print(f"  Risk per Trade: {config.DEFAULT_RISK_PER_TRADE}%")
    print(f"  Min Confidence: {config.DEFAULT_MIN_CONFIDENCE}%")
    print(f"  Duration: {config.DEFAULT_DURATION} minutes")
    print(f"  Update Interval: {config.DEFAULT_UPDATE_INTERVAL} seconds")

    use_defaults = input("\nUse default settings? (Y/n): ").strip().lower()

    if use_defaults == 'n':
        symbol = input("Symbol (BTCUSDT): ").strip() or "BTCUSDT"
        balance = float(input(f"Initial Balance ({config.DEFAULT_INITIAL_BALANCE}): ").strip() or config.DEFAULT_INITIAL_BALANCE)
        risk = float(input(f"Risk per Trade % ({config.DEFAULT_RISK_PER_TRADE}): ").strip() or config.DEFAULT_RISK_PER_TRADE)
        confidence = float(input(f"Min Confidence % ({config.DEFAULT_MIN_CONFIDENCE}): ").strip() or config.DEFAULT_MIN_CONFIDENCE)
        duration = int(input(f"Duration minutes ({config.DEFAULT_DURATION}): ").strip() or config.DEFAULT_DURATION)
        interval = int(input(f"Update Interval seconds ({config.DEFAULT_UPDATE_INTERVAL}): ").strip() or config.DEFAULT_UPDATE_INTERVAL)
    else:
        symbol = "BTCUSDT"
        balance = config.DEFAULT_INITIAL_BALANCE
        risk = config.DEFAULT_RISK_PER_TRADE
        confidence = config.DEFAULT_MIN_CONFIDENCE
        duration = config.DEFAULT_DURATION
        interval = config.DEFAULT_UPDATE_INTERVAL

    # Ask about RL training
    print("\n" + "=" * 80)
    print("REINFORCEMENT LEARNING")
    print("=" * 80)
    train_rl = input("\nTrain RL model before trading? (y/N): ").strip().lower() == 'y'

    if train_rl:
        rl_steps = int(input(f"Training steps ({config.DEFAULT_RL_TIMESTEPS}): ").strip() or config.DEFAULT_RL_TIMESTEPS)
        use_rl = True
    else:
        rl_steps = config.DEFAULT_RL_TIMESTEPS
        use_rl_input = input("Use RL for trading (if already trained)? (y/N): ").strip().lower()
        use_rl = use_rl_input == 'y'

    # Confirmation
    print("\n" + "=" * 80)
    print("STARTING TRADING BOT")
    print("=" * 80)
    print(f"\n📊 Trading: {symbol}")
    print(f"💰 Balance: ${balance:,.2f}")
    print(f"⚠️  Risk: {risk}% per trade")
    print(f"🎯 Min Confidence: {confidence}%")
    print(f"⏱️  Duration: {duration} minutes")
    print(f"🔄 Interval: {interval} seconds")
    print(f"🤖 GPT-4o-mini: {'ENABLED' if has_api_key else 'DISABLED'}")
    print(f"🧠 RL Training: {'YES' if train_rl else 'NO'}")
    print(f"🧠 RL Trading: {'YES' if use_rl else 'NO'}")

    input("\nPress Enter to start...")

    # Import and run bot
    from bot_trader import ICTTradingBot

    bot = ICTTradingBot(
        symbol=symbol,
        initial_balance=balance,
        risk_per_trade=risk,
        min_confidence=confidence,
        use_rl=use_rl
    )

    # Initialize
    bot.initialize_system()

    # Train RL if requested
    if train_rl:
        bot.train_rl_model(timesteps=rl_steps)

    # Run bot
    bot.run(duration_minutes=duration, update_interval=interval)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n✋ Setup cancelled by user")
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()
