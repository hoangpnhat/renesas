import streamlit as st
from datetime import datetime
import json
import os

# Đường dẫn file lưu lịch sử
HISTORY_FILE = "chat_history.json"

# Hàm lưu lịch sử vào file JSON
def save_history_to_file():
    try:
        with open(HISTORY_FILE, 'w', encoding='utf-8') as f:
            json.dump(st.session_state.chat_history, f, ensure_ascii=False, indent=2)
    except Exception as e:
        st.error(f"Lỗi khi lưu lịch sử: {e}")

# Hàm load lịch sử từ file JSON
def load_history_from_file():
    try:
        if os.path.exists(HISTORY_FILE):
            with open(HISTORY_FILE, 'r', encoding='utf-8') as f:
                return json.load(f)
    except Exception as e:
        st.error(f"Lỗi khi đọc lịch sử: {e}")
    return []

# Cấu hình trang
st.set_page_config(
    page_title="ChatBot AI",
    page_icon="🤖",
    layout="wide",
    initial_sidebar_state="expanded"
)

# CSS tùy chỉnh để làm giao diện giống ChatGPT
st.markdown("""
<style>
    /* Ẩn menu và footer mặc định */
    #MainMenu {visibility: hidden;}
    footer {visibility: hidden;}
    
    /* Tùy chỉnh sidebar */
    [data-testid="stSidebar"] {
        background-color: #202123;
    }
    
    [data-testid="stSidebar"] [data-testid="stMarkdownContainer"] {
        color: white;
    }
    
    /* Tùy chỉnh nút trong sidebar */
    .sidebar-button {
        background-color: transparent;
        border: 1px solid #4d4d4f;
        color: white;
        padding: 10px;
        border-radius: 5px;
        cursor: pointer;
        margin: 5px 0;
        width: 100%;
        text-align: left;
    }
    
    .sidebar-button:hover {
        background-color: #2a2b32;
    }
    
    /* Tùy chỉnh khung chat */
    .chat-message {
        padding: 1.5rem;
        border-radius: 0.5rem;
        margin-bottom: 1rem;
        display: flex;
        flex-direction: column;
    }
    
    .chat-message.user {
        background-color: #f7f7f8;
    }
    
    .chat-message.assistant {
        background-color: #ffffff;
        border: 1px solid #e5e5e5;
    }
    
    .chat-message .message-content {
        margin-top: 0.5rem;
    }
    
    .chat-message .role {
        font-weight: bold;
        margin-bottom: 0.5rem;
    }
    
    /* Tùy chỉnh input */
    .stTextInput > div > div > input {
        background-color: white;
        border: 1px solid #d1d5db;
        border-radius: 0.5rem;
        padding: 0.75rem;
    }
    
    /* Tùy chỉnh nút gửi */
    .stButton > button {
        background-color: #10a37f;
        color: white;
        border: none;
        border-radius: 0.375rem;
        padding: 0.5rem 1rem;
        font-weight: 500;
    }
    
    .stButton > button:hover {
        background-color: #0d8c6d;
    }
    
    /* History item */
    .history-item {
        padding: 0.75rem;
        margin: 0.25rem 0;
        border-radius: 0.375rem;
        cursor: pointer;
        color: white;
        background-color: transparent;
        border: 1px solid transparent;
    }
    
    .history-item:hover {
        background-color: #2a2b32;
    }
    
    .history-item.active {
        background-color: #343541;
        border-color: #4d4d4f;
    }
</style>
""", unsafe_allow_html=True)

# Khởi tạo session state
if "messages" not in st.session_state:
    st.session_state.messages = []

if "chat_history" not in st.session_state:
    # Load lịch sử từ file khi khởi động
    st.session_state.chat_history = load_history_from_file()

if "current_chat_id" not in st.session_state:
    st.session_state.current_chat_id = None

if "chat_counter" not in st.session_state:
    # Tính chat_counter dựa trên lịch sử đã có
    st.session_state.chat_counter = len(st.session_state.chat_history)

# Hàm tạo chat mới
def create_new_chat():
    st.session_state.chat_counter += 1
    chat_id = f"chat_{st.session_state.chat_counter}_{datetime.now().strftime('%Y%m%d%H%M%S')}"
    st.session_state.current_chat_id = chat_id
    st.session_state.messages = []
    
    # Thêm vào history
    st.session_state.chat_history.append({
        "id": chat_id,
        "title": f"Chat mới {st.session_state.chat_counter}",
        "messages": [],
        "created_at": datetime.now().isoformat()
    })
    
    # Lưu vào file
    save_history_to_file()

# Hàm load chat từ history
def load_chat(chat_id):
    for chat in st.session_state.chat_history:
        if chat["id"] == chat_id:
            st.session_state.current_chat_id = chat_id
            st.session_state.messages = chat["messages"].copy()
            break

# Hàm lưu tin nhắn vào chat hiện tại
def save_message_to_current_chat():
    if st.session_state.current_chat_id:
        for chat in st.session_state.chat_history:
            if chat["id"] == st.session_state.current_chat_id:
                chat["messages"] = st.session_state.messages.copy()
                # Cập nhật title nếu là tin nhắn đầu tiên
                if len(chat["messages"]) > 0 and chat["title"].startswith("Chat mới"):
                    first_message = chat["messages"][0]["content"][:50]
                    chat["title"] = first_message if len(first_message) < 50 else first_message + "..."
                break
        
        # Lưu vào file
        save_history_to_file()

# Hàm xóa chat
def delete_chat(chat_id):
    st.session_state.chat_history = [chat for chat in st.session_state.chat_history if chat["id"] != chat_id]
    if st.session_state.current_chat_id == chat_id:
        st.session_state.current_chat_id = None
        st.session_state.messages = []
    
    # Lưu vào file
    save_history_to_file()

# Hàm mô phỏng response từ AI (bạn có thể thay thế bằng API thực tế)
def get_ai_response(user_message):
    # Đây là response mẫu - bạn có thể tích hợp với OpenAI API, Google AI, v.v.
    responses = [
        f"Tôi đã nhận được tin nhắn của bạn: '{user_message}'. Đây là câu trả lời mẫu từ chatbot.",
        f"Cảm ơn bạn đã hỏi về '{user_message}'. Tôi là một chatbot demo và đang học cách trả lời tốt hơn.",
        f"Rất thú vị! Về vấn đề '{user_message}', tôi nghĩ rằng đây là một câu hỏi hay. Đây là phản hồi mẫu.",
        f"Đối với câu hỏi '{user_message}', tôi xin đưa ra ý kiến: Đây là một ứng dụng chatbot demo sử dụng Streamlit.",
    ]
    import random
    return random.choice(responses)

# Sidebar
with st.sidebar:
    st.title("💬 ChatBot AI")
    
    # Nút tạo chat mới
    if st.button("➕ Chat mới", use_container_width=True):
        create_new_chat()
        st.rerun()
    
    st.markdown("---")
    
    # Hiển thị lịch sử chat
    st.subheader("📚 Lịch sử")
    
    if len(st.session_state.chat_history) == 0:
        st.info("Chưa có cuộc hội thoại nào")
    else:
        # Hiển thị các chat từ mới đến cũ
        for chat in reversed(st.session_state.chat_history):
            col1, col2 = st.columns([4, 1])
            
            with col1:
                is_active = chat["id"] == st.session_state.current_chat_id
                button_type = "primary" if is_active else "secondary"
                
                if st.button(
                    f"💬 {chat['title']}", 
                    key=f"chat_{chat['id']}",
                    use_container_width=True,
                    type=button_type
                ):
                    load_chat(chat["id"])
                    st.rerun()
            
            with col2:
                if st.button("🗑️", key=f"delete_{chat['id']}", use_container_width=True):
                    delete_chat(chat["id"])
                    st.rerun()
    
    st.markdown("---")
    
    # Thông tin thêm
    with st.expander("ℹ️ Thông tin"):
        st.markdown("""
        **Chatbot AI Demo**
        
        Tính năng:
        - 💬 Chat tương tác
        - 📝 Lưu lịch sử
        - 🔄 Nhiều cuộc hội thoại
        - 🎨 Giao diện ChatGPT
        
        Phát triển bởi Streamlit
        """)
    
    # Nút xóa toàn bộ lịch sử
    if st.button("🗑️ Xóa toàn bộ lịch sử", use_container_width=True):
        st.session_state.chat_history = []
        st.session_state.messages = []
        st.session_state.current_chat_id = None
        save_history_to_file()
        st.rerun()

# Khu vực chat chính
st.title("🤖 ChatBot AI Assistant")

# Tạo chat mới nếu chưa có
if st.session_state.current_chat_id is None:
    create_new_chat()

# Hiển thị các tin nhắn
chat_container = st.container()
with chat_container:
    if len(st.session_state.messages) == 0:
        st.info("👋 Xin chào! Tôi là ChatBot AI. Hãy bắt đầu cuộc trò chuyện bằng cách gửi tin nhắn bên dưới.")
    else:
        for message in st.session_state.messages:
            with st.chat_message(message["role"]):
                st.markdown(message["content"])

# Input cho tin nhắn mới
prompt = st.chat_input("Nhập tin nhắn của bạn...")

if prompt:
    # Thêm tin nhắn của user
    st.session_state.messages.append({"role": "user", "content": prompt})
    
    # Hiển thị tin nhắn user
    with chat_container:
        with st.chat_message("user"):
            st.markdown(prompt)
    
    # Tạo và hiển thị response
    with chat_container:
        with st.chat_message("assistant"):
            with st.spinner("Đang suy nghĩ..."):
                response = get_ai_response(prompt)
                st.markdown(response)
    
    # Thêm response vào messages
    st.session_state.messages.append({"role": "assistant", "content": response})
    
    # Lưu vào history
    save_message_to_current_chat()
    
    # Rerun để cập nhật giao diện
    st.rerun()

# Footer
st.markdown("---")
st.markdown(
    "<div style='text-align: center; color: gray; padding: 1rem;'>"
    "💡 Tip: Nhấn 'Chat mới' để bắt đầu cuộc hội thoại mới"
    "</div>", 
    unsafe_allow_html=True
)
