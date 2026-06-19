import json
import re
def extract_and_remove_dict_from_string(s):
    # Xử lý trường hợp dict được bọc trong ```json và ```
    pattern_json = r'```json\s*({.*?})\s*```'
    match_json = re.search(pattern_json, s, re.DOTALL)
    # import pdb; pdb.set_trace()
    if match_json:
        dict_str = match_json.group(1)
        modified_str = s[:match_json.start()] + s[match_json.end():]
    else:
        return s, None

    # Chuyển chuỗi dict thành dict thực
    try:
        dict_data = json.loads(dict_str.replace("'", "\""))
    except json.JSONDecodeError as e:
        print(f"Lỗi khi phân tích chuỗi: {e}")
        return s, None

    return modified_str.strip(), dict_data


