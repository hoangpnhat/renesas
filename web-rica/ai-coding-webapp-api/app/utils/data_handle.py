from bson import ObjectId
from datetime import datetime
import json

# Custom serializer to handle ObjectId and datetime objects
def custom_serializer(obj):
    if isinstance(obj, ObjectId):
        return str(obj)
    elif isinstance(obj, datetime):
        return obj.isoformat()
    raise TypeError(f"Type {type(obj)} not serializable")


def serialize_data(data):
    if isinstance(data, dict):
        return json.dumps(data, default=custom_serializer)
    if isinstance(data, (list, str, int, float)):
        return json.dumps(data)
    try:
        if hasattr(data, '__dict__'):
            model_data = data.model_dump()
            return json.dumps(model_data, default=custom_serializer)
    except Exception as e:
        raise ValueError(f"Error serializing data: {e}")
    raise TypeError(f"Unsupported data type: {type(data)}")