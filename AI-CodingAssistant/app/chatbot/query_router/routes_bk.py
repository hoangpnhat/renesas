from semantic_router import Route

update_customer_info = Route(
    name="update_customer_info",
    utterances=[
        "Cập nhật ngày sinh của khách hàng",
        "cập nhật thông tin khách hàng",
        "cập nhật thông tin KH",
        "cập nhật sđt khách hàng",
        "cập nhật sđt KH",
        "cập nhật ngày sinh KH",
        "cập nhật ngày sinh khách hàng",
        "cập nhật sô điện thoại khách hàng",
        "Cập nhật sinh nhật khách hàng",
        "cập nhật sn khách hàng",
        "cập nhật sn KH",
        "KH đôi sđt",
        "KH đổi số điện thoại",
        "KH đổi ngày sinh",
        "KH đổi sn",
        "Khách muốn đổi số điện thoại",
        "Hỗ trợ khách hàng đổi số điện thoại",
        "Hỗ trợ khách hàng đổi ngày sinh",
        "Khách hàng muốn đổi ngày sinh",
        "KH muốn đổi ngày sinh"
    ],
    description="Cập nhật thông tin khách hàng"
)


material_warranty = Route(
    name="material_warranty",
    utterances=[
        "bảo hành chất liệu",
        "áo bị bay màu",
        "áo bị bay màu sau khi giặt",
        "sản phẩm bị bay màu",
        "sản phẩm bị bạc màu",
        "áo bị bay màu sau khi giặt",
        "sp bị bay màu",
        "vải bị dãn",
        "áo bị dãn",
        "vải kém chất lượng",
        "áo kém chất lượng",
        "vải bị xù lông",
        "da bị bong tróc",
        "vải bị xù lông và muốn đổi sản phẩm khác",
        "chất vải kém chất lượng và muốn đổi trả",
        "khách bảo vải bạn dãn và xù lông, kém chất lượng, muốn đổi trả",
        "áo bị bay màu và muốn đổi trả",
        "áo bị bay màu sau khi giặt và muốn đổi",
        "sản phẩm bị bay màu và muốn đổi sản phẩm khác",
        "sản phẩm bị bạc màu và muốn đổi trả",
        "áo bị bay màu sau khi giặt và muốn đổi"
    ]
)

repair_warranty = Route(
    name="repair_warranty",
    utterances=[
        "bảo hành sản phẩm",
        "áo có vết rách",
        "hỏng dây kéo",
        "áo bị rách",
        "áo có vết bẩn",
        "sản phẩm bị bong tróc",
        "sản phẩm bị hỏng khi vân chuyển",
        "sp bị hỏng khi vận chuyển",
        "sp bị hỏng khi giao hàng",
        "Các vấn đề liên quan đến bảo hành sản phẩm",
        "dây kéo bị hỏng",
        "cúc áo bị hỏng",
        "nút áo bị hỏng"
        "khách hàng muốn đổi hàng do lỗi sản phẩm",
        "KH muốn đổi hàng do lỗi sản phẩm",
        "KH muốn đổi hàng do lỗi của cửa hàng"
    ])

store_operation = Route(
    name="store_operation",
    utterances=[
        "cửa hàng đóng cửa",
        "CH mất điện",
        "cửa hàng mở cửa lúc mấy giờ",
        "Camera cửa hàng bị hỏng",
        "CH mất mạng",
        "CH mất điện",
        "CH mất nước",
        "CH mất wifi",
        "Máy tính cửa hàng bị hỏng",
        "Máy in cửa hàng bị hỏng",
        "Máy in hóa đơn bị hỏng",
        "Máy in hóa đơn không in được",
        "Lỗi camera cửa hàng",
        "Hỗ trợ giấy tờ",
        "giấy phép kinh doanh",
        "tất cả các vấn đề liên quan đến vận hành cửa hàng",
        "Đổi nhạc cửa hàng",
        "Đổi bài hát cửa hàng",
        "Đổi nhạc đang phát tại cửa hàng",
        "Không mở được nhạc tại cửa hàng",
    ]
)

adminitration = Route(
    name="adminitration",
    utterances=[
        "không thể đăng nhập",
        "Cấp lại mật khẩu Unicorn",
        "không thể đăng nhập Unicorn",
        "Không thể chấm công",
        "Cấp quyền xem báo cáo, tài liệu",
        "Hỗ trợ vấn đề liên quan đến đánh giá năng lực chuyên môn",
        "Cấp đồng phục nhân viên mới",
        "Cấp lại thẻ nhân viên",
        "cấp thẻ nv mới",
        "Cấp lại thẻ nv bị mất",
        "Cấp đồng phục cho nhân viên mới",
        "Không thể chấm công trên Unicorn"
    ]
)


promotion_partnership = Route(
    name="promotion_partnership",
    utterances=[
        "không áp dụng được chương trình khuyến mãi của Partnership của các đối tác như ONEMOUNT, ZALOPAY, MB BANK", 
        "không áp dụng được chương trình khuyến mãi của Partnership của các đối tác như VNPAY, YODY x FPTTELECOM, BE, LynkID, Landing page",
        "không áp dụng được ctkm của Partnership",
        "KH không nhận được tin nhắn hoặc mã khuyến mãi từ Partnership",
        "không nhận được mã khuyến mãi từ Partnership",
        "KH không nhận được mã khuyến mãi từ các đối tác",
        "KH không sử dụng được mã khuyến mãi từ các đối tác",
        "KH không hài lòng với chương trình khuyến mãi của các đối tác",
        "KH không hài lòng với quy trình áp dụng khuyến mãi của Partnership",
        "Vấn đề về việc áp dụng chương trình khuyến mãi của các đối tác",
        "Các vấn đề liên quan đến chương trình khuyến mãi của các đối tác",
        "Các vấn đề liên quan đến chương trình khuyến mãi của Partnership",
    ]
)

promotion_marketing = Route(
    name="promotion_marketing",
    utterances=[
        "không áp được mã của ctkm", 
        "không áp được mã của chương trình khuyến mãi",
        "Áp mã chương trình khuyến mãi nhưng không được giảm giá",
        "không áp được voucher giảm giá cho sinh nhật",
        "không sử dụng được mã giảm giá",
        "KH không nhận được mã giảm giá qua SMS",
        "KH không sử dụng được mã khuyến mãi từ các đối tác",
        "KH không hài lòng với chương trình khuyến mãi",
        "Vấn đề về việc áp dụng chương trình khuyến mãi trong tháng",
        "Các vấn đề liên quan đến chương trình khuyến mãi của YODY",
        "Không áp dụng được chương trình khuyến mãi",
        "không áp dụng được ctkm",
        "cho e hỏi CTKM Tháng 5 - Siêu hội Freeship - Online có áp dụng cho nhân viên Yody, Yoki, FGG, YGG không ạ",
        "đã tạo CTKM trên uni, nhưng khi áp vào nó ra như này"
        "không áp dụng được mã giảm giá của ctkm"
    ]
)


query_promotion = Route(
    name="query_promotion",
    utterances=[
        "Thông tin về chương trình khuyến mãi",
        "Thông tin về ctkm",
        "Điều kiện áp dụng ctkm",
        "Nội dung ctkm",
        "Thời gian áp dụng ctkm",
        "Phạm vi áp dụng ctkm",
        "Mã voucher ctkm",
        "Voucher khuyến mãi",
        "Mã voucher chương trình khuyến mãi",
        "Phạm vi áp dụng chuong trình khuyến mãi",
        "Thời gian áp dụng chương trình khuyến mãi",
        "Nội dung chương trình khuyến mãi",
        "Điều kiện áp dụng chương trình khuyến mãi",
        "Thông tin chi tiết chương trình khuyến mãi",
    ]
)

search_product = Route(
    name="search_product",
    utterances=[
        "Kiểm tra sản phẩm",
        "Check thông tin sản phẩm",
        "Kiểm tra size sản phẩm",
        "Check size sản phẩm",
        "Kiểm tra số lượng sản phẩm",
        "Check tồn kho sản phẩm",
        "Kiểm tra màu sắc sản phẩm",
        "Check tất cả màu sắc của sản phẩm"
    ]
)


close_issue = Route(
    name="close_issue",
    utterances=[
        "đã giải quyết được rồi",
        "cảm ơn",
        "đã xong",
        "không cần giúp nữa",
        "đã xử lý xong",
        "đã xử lý",
        "hoàn tất",
        "đóng vân đề",
        "đóng vđ"
    ]
)

unclear_issue = Route(
    name="unclear_issue",
    utterances=[
        "Hỗ trợ",
        "Hỗ trợ cửa hàng",
        "Hỗ trợ CH",
        "Hỗ trợ đơn hàng",
        "Hỗ trợ đơn hàng online",
        "HT CH",
        "HT cửa hàng",
        "HT đơn hàng"
    ]
)

chitchat = Route(
    name="chitchat",
    utterances=[
        "Chào bạn",
        "Xin chào",
        "Chào",
        "Hi",
        "Hello",
        "Bạn khỏe không"
    ]
)

routes = [
    update_customer_info,
    material_warranty,
    repair_warranty,
    store_operation,
    adminitration,
    promotion_partnership,
    promotion_marketing,
    query_promotion,
    close_issue,
    unclear_issue,
    search_product
]