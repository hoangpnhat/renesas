from typing import Any

from injector import Injector
from databricks.session import ChatPromptDatabricks, DownloadFileDatabricks

def init_injector():
    injector = Injector()
    return injector

injector = init_injector()

# INFO: Baseline services, these services needs to be created first
injector.create_object(ChatPromptDatabricks)
injector.create_object(DownloadFileDatabricks)

def AppDepends(i: Any):
    return injector.get(i)
