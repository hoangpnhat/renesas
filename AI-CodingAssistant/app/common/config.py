import logging
import os


class Config:
    _instance = None
    _config = None

    def __new__(cls, config_file_path: str = "config.yml"):
        if cls._instance is None:
            cls._instance = super(Config, cls).__new__(cls)
            if config_file_path is not None:
                cls._load_config(config_file_path)
        return cls._instance._config

    @classmethod
    def _load_config(cls, config_file_path: str):
        from yacs.config import CfgNode as CN
        cfg = CN()
        cfg.set_new_allowed(True)
        cfg.merge_from_file(config_file_path)
        cls._config = cfg

    @classmethod
    def get_config(cls):
        if cls._config is None:
            raise ValueError("Config has not been loaded. Please initialize ConfigSingleton with a config file path.")
        return cls._config


class CustomFormatter(logging.Formatter):
    blue = "\x1b[34;20m"
    green = "\x1b[32;20m"
    grey = "\x1b[38;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"
    format = "%(asctime)s - %(name)s - %(levelname)s - %(message)s (%(filename)s:%(lineno)d)"

    FORMATS = {
        logging.DEBUG: blue + format + reset,
        logging.INFO: green + format + reset,
        logging.WARNING: yellow + format + reset,
        logging.ERROR: red + format + reset,
        logging.CRITICAL: bold_red + format + reset
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)
    

class MyLogger:
    _instances = {}

    def __new__(cls, name: str):
        if name not in cls._instances:
            cls._instances[name] = super(MyLogger, cls).__new__(cls)
            cls._instances[name].__init_once(name)
        return cls._instances[name].logger

    def __init_once(self, name: str):
        self.logger = logging.getLogger(name)

        if name == "dev":
            os.makedirs(os.path.dirname(os.environ.get("DEV_LOG_FILE_PATH")), exist_ok=True)
            self.logger.setLevel(logging.DEBUG)
            # console handling
            ch = logging.StreamHandler()
            ch.setLevel(logging.DEBUG)
            ch.setFormatter(CustomFormatter())
            self.logger.addHandler(ch)

            # file handling
            fh = logging.FileHandler(os.environ.get("DEV_LOG_FILE_PATH"), mode="a+", encoding="utf-8")
            fh.setLevel(logging.DEBUG)
            fh.setFormatter(CustomFormatter())
            self.logger.addHandler(fh)

        elif name == "prd":
            os.makedirs(os.path.dirname(os.environ.get("DEV_LOG_FILE_PATH")), exist_ok=True)
            self.logger.setLevel(logging.INFO)
            # file handling
            fh = logging.FileHandler(os.environ.get("PRD_LOG_FILE_PATH"), mode="a+", encoding="utf-8")
            fh.setLevel(logging.INFO)
            fh.setFormatter(CustomFormatter())
            self.logger.addHandler(fh)


cfg = Config()
logger = MyLogger(os.environ.get("ENV"))
