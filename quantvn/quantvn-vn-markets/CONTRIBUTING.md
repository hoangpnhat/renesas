# Đóng góp cho dự án `quantvn`

Chúng tôi hoan nghênh mọi đóng góp nhằm cải thiện dự án `quantvn`. Vui lòng làm theo các bước dưới đây để bắt đầu.

---

## 🚀 Bước 1: Fork & clone dự án

1. Truy cập vào GitHub repository của dự án `quantvn`
2. Fork dự án về GitHub của bạn
3. Clone về máy tính:

```bash
git clone https://github.com/your-username/quantvn.git
cd quantvn
```

---

## 🔧 Bước 2: Thiết lập môi trường phát triển

Tạo môi trường ảo và cài đặt dependencies:

```bash
# Tạo môi trường ảo
python -m venv venv

# Kích hoạt môi trường ảo
# Trên macOS/Linux:
source venv/bin/activate
# Trên Windows:
venv\Scripts\activate

# Cài đặt dependencies
pip install -r requirements.txt

# Cài đặt package ở chế độ development
pip install -e .
```

---

## 🌿 Bước 3: Tạo nhánh mới từ main

Vui lòng đặt tên nhánh theo chuẩn sau:

- `feature/ten-tinh-nang` - cho tính năng mới
- `fix/ten-loi` - cho sửa lỗi
- `docs/ten-chu-de` - cho tài liệu
- `refactor/ten-phan` - cho tái cấu trúc code

```bash
git checkout -b feature/your-feature-name
```

---

## 💻 Bước 4: Phát triển tính năng

### Đối với code mới:

1. **Viết code theo chuẩn Python PEP 8**
2. **Thêm docstrings cho functions/classes**
3. **Viết unit tests** (nếu có)
4. **Kiểm tra code hoạt động đúng**

### Đối với tài liệu:

Viết tài liệu mới vào các file `.rst` trong thư mục `source/`

Cấu trúc thư mục tài liệu:

```
source/
├── index.rst
├── installation.rst
├── quickstart.rst
├── data/
│   ├── stocks.rst
│   ├── derivatives.rst
├── metrics/
│   ├── performance_metrics.rst
└── examples/
    ├── basic_usage.rst
```

---

## 🔍 Bước 5: Kiểm tra code

Trước khi commit, hãy chắc chắn:

```bash
# Kiểm tra syntax và style
python -m py_compile quantvn/**/*.py

# Chạy tests (nếu có)
python -m pytest tests/

# Build tài liệu (nếu thay đổi docs)
make html  # trên macOS/Linux
.\make.bat html  # trên Windows
```

Sau đó mở file `build/html/index.html` trong trình duyệt để xem tài liệu.

---

## 📤 Bước 6: Commit & Push

```bash
# Stage changes
git add .

# Commit với message rõ ràng
git commit -m "feat: add new feature X"
# hoặc
git commit -m "fix: resolve issue with Y"
# hoặc
git commit -m "docs: update documentation for Z"

# Push lên GitHub
git push origin feature/your-feature-name
```

### Quy tắc commit message:

- `feat:` - Thêm tính năng mới
- `fix:` - Sửa lỗi
- `docs:` - Thay đổi tài liệu
- `refactor:` - Tái cấu trúc code
- `test:` - Thêm hoặc sửa tests
- `chore:` - Công việc bảo trì

---

## 🔄 Bước 7: Tạo Pull Request

1. Truy cập repository trên GitHub
2. Click vào nút "New Pull Request"
3. Chọn nhánh của bạn so với nhánh `main`
4. Viết mô tả chi tiết về thay đổi:
   - Tính năng/lỗi được giải quyết
   - Cách test
   - Screenshots (nếu có)
5. Submit Pull Request

---

## 📋 Checklist trước khi gửi Pull Request

- [ ] Code tuân theo chuẩn PEP 8
- [ ] Đã thêm docstrings đầy đủ cho functions/classes mới
- [ ] Đã test code hoạt động đúng
- [ ] Đã cập nhật tài liệu (nếu cần)
- [ ] Commit messages rõ ràng và có ý nghĩa
- [ ] Không có merge conflicts với nhánh main
- [ ] Đã kiểm tra build tài liệu (nếu thay đổi docs)

---

## 💡 Tips khi viết tài liệu (Sphinx + reStructuredText)

Để giúp tài liệu của `quantvn` rõ ràng, dễ đọc và dễ maintain, bạn vui lòng tuân theo các quy tắc sau:

### 1. Tiêu đề rõ ràng, có cấu trúc

| Cấp độ | Ký hiệu | Ví dụ                        |
| ------ | ------- | ---------------------------- |
| 1      | `===`   | `QuantVN Documentation`      |
| 2      | `---`   | `Module: quantvn.vn.data`    |
| 3      | `~~~`   | `Function: get_stock_hist`   |
| 4      | `^^^^`  | `Parameters` hoặc `Examples` |

**Ví dụ:**

```rst
QuantVN Documentation
=====================

Module: quantvn.vn.data
-----------------------

Function: get_stock_hist
~~~~~~~~~~~~~~~~~~~~~~~~
```

### 2. Dùng `autodoc` cho docstrings

```rst
.. autofunction:: quantvn.vn.data.stocks.get_stock_hist

.. autoclass:: quantvn.vn.metrics.Metrics
   :members:
   :undoc-members:
```

### 3. Thêm ví dụ code

```rst
Examples
^^^^^^^^

.. code-block:: python

   from quantvn.vn.data import get_stock_hist

   df = get_stock_hist("VIC", resolution='h')
   print(df.head())
```

---

## 🐛 Báo cáo lỗi

Nếu bạn phát hiện lỗi, hãy tạo GitHub Issue với thông tin:

- **Mô tả lỗi**: Giải thích rõ ràng vấn đề
- **Cách tái hiện**: Các bước để tái hiện lỗi
- **Kết quả mong đợi**: Điều bạn kỳ vọng xảy ra
- **Kết quả thực tế**: Điều thực sự xảy ra
- **Môi trường**: OS, Python version, quantvn version
- **Code mẫu**: Đoạn code tối thiểu để tái hiện lỗi

---

## 💬 Liên hệ & Hỗ trợ

Nếu bạn cần hỗ trợ hoặc có câu hỏi:

- 🐛 **GitHub Issues**: Để báo lỗi và đề xuất tính năng
- 📧 **Email**: Liên hệ maintainers qua email
- 💬 **Discussions**: Tham gia thảo luận trên GitHub Discussions

---

## 📝 Các loại đóng góp được chào đón

- 🐛 Báo cáo và sửa lỗi
- ✨ Đề xuất và phát triển tính năng mới
- 📖 Cải thiện tài liệu
- ✅ Viết tests
- 🎨 Cải thiện code structure
- 🌐 Dịch tài liệu sang ngôn ngữ khác
- 📊 Thêm ví dụ sử dụng

---

Cảm ơn bạn đã quan tâm đóng góp cho `quantvn`! 🙏

Mọi đóng góp, dù lớn hay nhỏ, đều được đánh giá cao và giúp cộng đồng phân tích tài chính định lượng Việt Nam phát triển mạnh mẽ hơn. 💙
