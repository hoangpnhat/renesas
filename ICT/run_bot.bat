@echo off
cd /d "%~dp0"
.venv\Scripts\python.exe -u bot_trader.py --duration 5 --interval 30 --confidence 70
pause
