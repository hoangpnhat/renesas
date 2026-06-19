.PHONY: install format lint check

# Cài đặt các công cụ cần thiết
install:
	pip install flake8 black

# Format code với Black (chuẩn PEP8)
format:
	black quantvn

# Kiểm tra lỗi code với Flake8
lint:
	flake8 quantvn

# Chạy cả format và lint cùng lúc
check: format lint
