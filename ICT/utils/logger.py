"""
ICT Trading System - Logging Module
====================================
Comprehensive logging for application logs and TensorBoard visualization
"""

import logging
import sys
from datetime import datetime
from pathlib import Path
from typing import Optional
import os


class ColoredFormatter(logging.Formatter):
    """Colored formatter for console output"""

    # ANSI color codes
    COLORS = {
        'DEBUG': '\033[36m',      # Cyan
        'INFO': '\033[32m',       # Green
        'WARNING': '\033[33m',    # Yellow
        'ERROR': '\033[31m',      # Red
        'CRITICAL': '\033[35m',   # Magenta
        'RESET': '\033[0m'        # Reset
    }

    def format(self, record):
        # Add color to level name
        levelname = record.levelname
        if levelname in self.COLORS:
            record.levelname = f"{self.COLORS[levelname]}{levelname}{self.COLORS['RESET']}"

        return super().format(record)


class ICTLogger:
    """
    Centralized logging system for ICT Trading System

    Features:
    - Console output with colors
    - File logging (rotating)
    - Separate error log
    - Component-specific loggers
    - TensorBoard integration
    """

    _instance = None
    _initialized = False

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(ICTLogger, cls).__new__(cls)
        return cls._instance

    def __init__(self):
        if not ICTLogger._initialized:
            self.setup_logging()
            ICTLogger._initialized = True

    def setup_logging(self, log_dir: str = "logs", level: int = logging.INFO):
        """Setup logging configuration"""
        # Create logs directory
        self.log_dir = Path(log_dir)
        self.log_dir.mkdir(exist_ok=True)

        # Generate timestamp for log files
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")

        # Define log files
        self.main_log_file = self.log_dir / f"ict_trading_{timestamp}.log"
        self.error_log_file = self.log_dir / "errors.log"
        self.trading_log_file = self.log_dir / "trading.log"
        self.rl_log_file = self.log_dir / "rl_training.log"

        # Root logger configuration
        self.root_logger = logging.getLogger('ICT')
        self.root_logger.setLevel(level)
        self.root_logger.handlers.clear()  # Clear existing handlers

        # Console handler with colors
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        console_formatter = ColoredFormatter(
            '%(levelname)s | %(name)s | %(message)s'
        )
        console_handler.setFormatter(console_formatter)

        # Main file handler
        file_handler = logging.FileHandler(self.main_log_file, encoding='utf-8')
        file_handler.setLevel(logging.DEBUG)
        file_formatter = logging.Formatter(
            '%(asctime)s | %(levelname)-8s | %(name)s | %(funcName)s:%(lineno)d | %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        file_handler.setFormatter(file_formatter)

        # Error file handler
        error_handler = logging.FileHandler(self.error_log_file, encoding='utf-8')
        error_handler.setLevel(logging.ERROR)
        error_handler.setFormatter(file_formatter)

        # Add handlers to root logger
        self.root_logger.addHandler(console_handler)
        self.root_logger.addHandler(file_handler)
        self.root_logger.addHandler(error_handler)

        # Component-specific loggers
        self.component_loggers = {}

        # Log initialization
        self.root_logger.info("="*80)
        self.root_logger.info("ICT Trading System - Logging Initialized")
        self.root_logger.info(f"Log Directory: {self.log_dir.absolute()}")
        self.root_logger.info(f"Main Log: {self.main_log_file.name}")
        self.root_logger.info(f"Error Log: {self.error_log_file.name}")
        self.root_logger.info("="*80)

    def get_logger(self, name: str) -> logging.Logger:
        """Get a component-specific logger"""
        if name not in self.component_loggers:
            logger = logging.getLogger(f'ICT.{name}')
            self.component_loggers[name] = logger
        return self.component_loggers[name]

    def get_trading_logger(self) -> logging.Logger:
        """Get trading-specific logger with separate file"""
        logger = logging.getLogger('ICT.Trading')

        # Add trading file handler if not already added
        if not any(isinstance(h, logging.FileHandler) and h.baseFilename == str(self.trading_log_file) for h in logger.handlers):
            trading_handler = logging.FileHandler(self.trading_log_file, encoding='utf-8')
            trading_handler.setLevel(logging.INFO)
            trading_formatter = logging.Formatter(
                '%(asctime)s | %(levelname)-8s | %(message)s',
                datefmt='%Y-%m-%d %H:%M:%S'
            )
            trading_handler.setFormatter(trading_formatter)
            logger.addHandler(trading_handler)

        return logger

    def get_rl_logger(self) -> logging.Logger:
        """Get RL training-specific logger"""
        logger = logging.getLogger('ICT.RL')

        # Add RL file handler if not already added
        if not any(isinstance(h, logging.FileHandler) and h.baseFilename == str(self.rl_log_file) for h in logger.handlers):
            rl_handler = logging.FileHandler(self.rl_log_file, encoding='utf-8')
            rl_handler.setLevel(logging.DEBUG)
            rl_formatter = logging.Formatter(
                '%(asctime)s | %(levelname)-8s | %(message)s',
                datefmt='%Y-%m-%d %H:%M:%S'
            )
            rl_handler.setFormatter(rl_formatter)
            logger.addHandler(rl_handler)

        return logger

    def log_trade(self, action: str, price: float, quantity: float = 1.0,
                  reason: str = "", pnl: Optional[float] = None):
        """Log a trading action"""
        logger = self.get_trading_logger()

        msg = f"TRADE | {action:4s} | Price: ${price:,.2f} | Qty: {quantity:.4f}"
        if reason:
            msg += f" | Reason: {reason}"
        if pnl is not None:
            msg += f" | PnL: ${pnl:+,.2f}"

        logger.info(msg)

    def log_signal(self, signal: str, confidence: float, reasoning: str = ""):
        """Log a trading signal"""
        logger = self.get_trading_logger()

        msg = f"SIGNAL | {signal:4s} | Confidence: {confidence:.1f}%"
        if reasoning:
            msg += f" | {reasoning}"

        logger.info(msg)

    def log_rl_episode(self, episode: int, reward: float, steps: int,
                       win_rate: float = 0.0, balance: float = 0.0):
        """Log RL training episode"""
        logger = self.get_rl_logger()

        msg = f"Episode {episode:5d} | Reward: {reward:+8.2f} | Steps: {steps:4d} | Win Rate: {win_rate:.1%} | Balance: ${balance:,.2f}"
        logger.info(msg)

    def log_error(self, component: str, error: Exception, context: str = ""):
        """Log an error with context"""
        logger = self.get_logger(component)

        msg = f"ERROR in {component}"
        if context:
            msg += f" ({context})"
        msg += f": {type(error).__name__}: {str(error)}"

        logger.error(msg, exc_info=True)

    def log_performance(self, metrics: dict):
        """Log performance metrics"""
        logger = self.get_trading_logger()

        logger.info("="*60)
        logger.info("PERFORMANCE METRICS")
        logger.info("-"*60)

        for key, value in metrics.items():
            if isinstance(value, float):
                if 'rate' in key.lower() or 'ratio' in key.lower():
                    logger.info(f"  {key:20s}: {value:.2%}")
                else:
                    logger.info(f"  {key:20s}: {value:.4f}")
            else:
                logger.info(f"  {key:20s}: {value}")

        logger.info("="*60)


class TensorBoardLogger:
    """
    TensorBoard logging for RL training and system metrics

    Features:
    - Scalars (loss, reward, etc.)
    - Histograms (weights, gradients)
    - Images (charts, visualizations)
    - Text (hyperparameters, notes)
    """

    def __init__(self, log_dir: str = "tensorboard_logs", experiment_name: str = None):
        """
        Initialize TensorBoard logger

        Args:
            log_dir: Base directory for TensorBoard logs
            experiment_name: Name of the experiment (default: timestamp)
        """
        self.log_dir = Path(log_dir)
        self.log_dir.mkdir(exist_ok=True)

        # Create experiment directory
        if experiment_name is None:
            experiment_name = datetime.now().strftime("%Y%m%d_%H%M%S")

        self.experiment_dir = self.log_dir / experiment_name
        self.experiment_dir.mkdir(exist_ok=True)

        # Initialize TensorBoard writer
        self.writer = None
        self._initialize_writer()

        # Log directory info
        logger = ICTLogger().get_logger("TensorBoard")
        logger.info(f"TensorBoard logs: {self.experiment_dir.absolute()}")
        logger.info(f"Start TensorBoard: tensorboard --logdir={self.log_dir.absolute()}")

    def _initialize_writer(self):
        """Initialize TensorBoard SummaryWriter"""
        try:
            from torch.utils.tensorboard import SummaryWriter
            self.writer = SummaryWriter(log_dir=str(self.experiment_dir))
            self.enabled = True
        except ImportError:
            logger = ICTLogger().get_logger("TensorBoard")
            logger.warning("TensorBoard not available. Install with: pip install tensorboard")
            self.enabled = False

    def log_scalar(self, tag: str, value: float, step: int):
        """Log a scalar value"""
        if self.enabled and self.writer:
            self.writer.add_scalar(tag, value, step)

    def log_scalars(self, main_tag: str, tag_scalar_dict: dict, step: int):
        """Log multiple scalars"""
        if self.enabled and self.writer:
            self.writer.add_scalars(main_tag, tag_scalar_dict, step)

    def log_histogram(self, tag: str, values, step: int):
        """Log a histogram"""
        if self.enabled and self.writer:
            self.writer.add_histogram(tag, values, step)

    def log_text(self, tag: str, text: str, step: int = 0):
        """Log text"""
        if self.enabled and self.writer:
            self.writer.add_text(tag, text, step)

    def log_hyperparameters(self, hparams: dict, metrics: dict):
        """Log hyperparameters and metrics"""
        if self.enabled and self.writer:
            self.writer.add_hparams(hparams, metrics)

    def log_training_step(self, step: int, loss: float, reward: float,
                          learning_rate: float = None):
        """Log a training step"""
        self.log_scalar('Training/Loss', loss, step)
        self.log_scalar('Training/Reward', reward, step)

        if learning_rate is not None:
            self.log_scalar('Training/LearningRate', learning_rate, step)

    def log_episode(self, episode: int, total_reward: float, steps: int,
                    win_rate: float = 0.0, balance: float = 0.0):
        """Log RL episode metrics"""
        self.log_scalar('Episode/TotalReward', total_reward, episode)
        self.log_scalar('Episode/Steps', steps, episode)
        self.log_scalar('Episode/WinRate', win_rate, episode)
        self.log_scalar('Episode/Balance', balance, episode)

    def log_trading_metrics(self, step: int, metrics: dict):
        """Log trading performance metrics"""
        for key, value in metrics.items():
            if isinstance(value, (int, float)):
                self.log_scalar(f'Trading/{key}', value, step)

    def log_feature_importance(self, step: int, feature_names: list, importances: list):
        """Log feature importances"""
        if self.enabled and self.writer:
            import matplotlib.pyplot as plt
            import io as io_module
            from PIL import Image
            import numpy as np

            # Create bar plot
            fig, ax = plt.subplots(figsize=(12, 6))
            y_pos = np.arange(len(feature_names))
            ax.barh(y_pos, importances)
            ax.set_yticks(y_pos)
            ax.set_yticklabels(feature_names)
            ax.invert_yaxis()
            ax.set_xlabel('Importance')
            ax.set_title('Feature Importance')

            # Convert to image
            buf = io_module.BytesIO()
            plt.savefig(buf, format='png', bbox_inches='tight')
            buf.seek(0)
            image = Image.open(buf)

            # Log to TensorBoard
            import torchvision.transforms as transforms
            image_tensor = transforms.ToTensor()(image)
            self.writer.add_image('Features/Importance', image_tensor, step)

            plt.close(fig)
            buf.close()

    def close(self):
        """Close the TensorBoard writer"""
        if self.enabled and self.writer:
            self.writer.close()


# Singleton instances
_ict_logger = None
_tensorboard_logger = None


def get_logger(component: str = "Main") -> logging.Logger:
    """
    Get a logger for a specific component

    Args:
        component: Component name (e.g., 'PDDetector', 'Trading', 'RL')

    Returns:
        logging.Logger instance

    Example:
        logger = get_logger('PDDetector')
        logger.info('Detecting order blocks...')
    """
    global _ict_logger
    if _ict_logger is None:
        _ict_logger = ICTLogger()
    return _ict_logger.get_logger(component)


def get_trading_logger() -> logging.Logger:
    """Get the trading logger"""
    global _ict_logger
    if _ict_logger is None:
        _ict_logger = ICTLogger()
    return _ict_logger.get_trading_logger()


def get_rl_logger() -> logging.Logger:
    """Get the RL training logger"""
    global _ict_logger
    if _ict_logger is None:
        _ict_logger = ICTLogger()
    return _ict_logger.get_rl_logger()


def get_tensorboard_logger(experiment_name: str = None) -> TensorBoardLogger:
    """
    Get the TensorBoard logger

    Args:
        experiment_name: Name of the experiment (default: timestamp)

    Returns:
        TensorBoardLogger instance

    Example:
        tb_logger = get_tensorboard_logger('experiment_1')
        tb_logger.log_scalar('loss', 0.5, step=100)
    """
    global _tensorboard_logger
    if _tensorboard_logger is None:
        _tensorboard_logger = TensorBoardLogger(experiment_name=experiment_name)
    return _tensorboard_logger


def log_trade(action: str, price: float, quantity: float = 1.0,
              reason: str = "", pnl: Optional[float] = None):
    """Convenience function to log a trade"""
    global _ict_logger
    if _ict_logger is None:
        _ict_logger = ICTLogger()
    _ict_logger.log_trade(action, price, quantity, reason, pnl)


def log_signal(signal: str, confidence: float, reasoning: str = ""):
    """Convenience function to log a trading signal"""
    global _ict_logger
    if _ict_logger is None:
        _ict_logger = ICTLogger()
    _ict_logger.log_signal(signal, confidence, reasoning)


def log_error(component: str, error: Exception, context: str = ""):
    """Convenience function to log an error"""
    global _ict_logger
    if _ict_logger is None:
        _ict_logger = ICTLogger()
    _ict_logger.log_error(component, error, context)


def log_performance(metrics: dict):
    """Convenience function to log performance metrics"""
    global _ict_logger
    if _ict_logger is None:
        _ict_logger = ICTLogger()
    _ict_logger.log_performance(metrics)


# Example usage and testing
if __name__ == "__main__":
    # Test standard logging
    print("Testing ICT Logger...")

    logger = get_logger("TestComponent")
    logger.debug("This is a debug message")
    logger.info("This is an info message")
    logger.warning("This is a warning message")
    logger.error("This is an error message")

    # Test trading logger
    print("\nTesting Trading Logger...")
    log_trade("BUY", 50000.0, 0.1, "Bullish OB at discount", pnl=None)
    log_signal("BUY", 85.5, "HTF-LTF confluence + BOS confirmation")

    # Test performance logging
    print("\nTesting Performance Logger...")
    metrics = {
        'Total Trades': 50,
        'Win Rate': 0.68,
        'Profit Factor': 2.3,
        'Sharpe Ratio': 1.85,
        'Max Drawdown': 0.15,
        'Total PnL': 5420.50
    }
    log_performance(metrics)

    # Test TensorBoard logger
    print("\nTesting TensorBoard Logger...")
    tb_logger = get_tensorboard_logger('test_experiment')

    # Log some training data
    for step in range(10):
        tb_logger.log_training_step(step, loss=0.5 - step*0.03, reward=step*10)
        tb_logger.log_episode(step, total_reward=step*100, steps=50+step*5,
                             win_rate=0.6 + step*0.02, balance=10000 + step*500)

    tb_logger.close()

    print("\n✅ Logging tests complete!")
    print(f"Check logs in: logs/")
    print(f"Check TensorBoard logs in: tensorboard_logs/")
    print(f"Start TensorBoard: tensorboard --logdir=tensorboard_logs")
