#!/bin/bash

# Let the DB start
# python3 ./app/src/backend_pre_start.py

# Create initial data in DB
cd app/src
# Sử dụng PORT từ biến môi trường hoặc default là 80
PORT=${PORT:-80}
uvicorn main:app --host 0.0.0.0 --port $PORT