# ChatBot AI - Streamlit Web Application

Ứng dụng chatbot với giao diện tương tự ChatGPT, được xây dựng bằng Streamlit.

## 🌟 Tính năng

- ✨ Giao diện giống ChatGPT với sidebar và cửa sổ chat
- 💬 Chat tương tác real-time
- 📚 Lưu lịch sử các cuộc hội thoại
- 🔄 Quản lý nhiều cuộc hội thoại cùng lúc
- 🗑️ Xóa từng chat hoặc toàn bộ lịch sử
- 🎨 Giao diện thân thiện, dễ sử dụng

## 📋 Yêu cầu

- Python 3.8 trở lên
- Streamlit

## 🚀 Cài đặt

1. Clone repository hoặc tải mã nguồn về:
```bash
cd chatbotstreamlit
```

2. Cài đặt các thư viện cần thiết:
```bash
pip install -r requirements.txt
```

## ▶️ Chạy ứng dụng

Chạy lệnh sau trong terminal:

```bash
streamlit run app.py
```

Ứng dụng sẽ tự động mở trong trình duyệt tại địa chỉ: `http://localhost:8501`

## 📖 Hướng dẫn sử dụng

1. **Tạo chat mới**: Nhấn nút "➕ Chat mới" trên sidebar
2. **Gửi tin nhắn**: Nhập tin nhắn vào ô input ở cuối trang và nhấn Enter
3. **Xem lịch sử**: Các cuộc hội thoại được lưu tự động trong sidebar
4. **Chuyển đổi chat**: Click vào bất kỳ chat nào trong lịch sử để xem lại
5. **Xóa chat**: Nhấn nút 🗑️ bên cạnh mỗi chat để xóa

## 🔧 Tùy chỉnh

Bạn có thể tùy chỉnh ứng dụng bằng cách:

### Tích hợp AI thực tế

Thay thế hàm `get_ai_response()` trong `app.py` để kết nối với API AI thực tế:

```python
def get_ai_response(user_message):
    # Ví dụ: Tích hợp OpenAI API
    import openai
    
    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo",
        messages=[
            {"role": "user", "content": user_message}
        ]
    )
    
    return response.choices[0].message.content
```

### Thay đổi giao diện

Chỉnh sửa phần CSS trong `st.markdown()` để thay đổi màu sắc, font chữ, và layout.

### Thêm tính năng

- Export chat ra file
- Tìm kiếm trong lịch sử
- Thêm file đính kèm
- Voice input/output

## 📁 Cấu trúc project

```
chatbotstreamlit/
│
├── app.py              # File chính của ứng dụng
├── requirements.txt    # Danh sách thư viện cần thiết
└── README.md          # File hướng dẫn này
```

## 💡 Lưu ý

- Dữ liệu lịch sử chat được lưu trong session state của Streamlit và sẽ mất khi refresh trang
- Để lưu dữ liệu vĩnh viễn, bạn có thể tích hợp với database (SQLite, PostgreSQL, MongoDB, etc.)
- Response hiện tại là mẫu demo - cần tích hợp với AI API thực tế để có trải nghiệm hoàn chỉnh

## 🤝 Đóng góp

Mọi đóng góp đều được chào đón! Hãy tạo pull request hoặc mở issue nếu bạn có ý tưởng cải tiến.

## 📝 License

MIT License - Tự do sử dụng và chỉnh sửa theo nhu cầu của bạn.

---

**Chúc bạn sử dụng vui vẻ! 🎉**
