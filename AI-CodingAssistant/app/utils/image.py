from PIL import Image
import io
import base64
import os
import requests
from typing import Tuple

from app.common.config import cfg

def convert_image_to_base64(image: str|Image.Image, quality: int=80, max_size: Tuple=(512, 512)) -> str:
    """
    This function converts an image file to a base64 encoded string with transformation.

    Args:
        image_path (str): The path to the image file.
        quality (int): The quality of the image (default is 80).
        max_size (tuple): The maximum size of the image (default is (512, 512)).

    Returns:
        str: The base64 encoded image.
    """
    # Open the image file
    if isinstance(image, str):
        image = Image.open(image)
    if image.size[0] > max_size[0] or image.size[1] > max_size[1]:
        image.thumbnail(max_size, Image.Resampling.LANCZOS)

    image_data = io.BytesIO()
    image.save(image_data, format="PNG", optimize=True, quality=quality)
    image_data.seek(0)
    base64_encoded = base64.b64encode(image_data.getvalue()).decode("utf-8")
    return base64_encoded


def is_image(image_path: str) -> bool:
    """
    This function checks if a file is an image file.

    Args:
        image_path (str): The path to the image file.

    Returns:
        bool: True if the file is an image file, False otherwise.
    """
    return os.path.splitext(image_path)[1].lower() in cfg.image_extensions


def download_image(image_url: str, image_dir: str=None) -> Tuple[Image.Image, str]:
    """
    This function downloads an image file from a URL.

    Args:
        image_url (str): The URL of the image.
        image_path (str): The path to save the image file.

    Returns:
        Image.Image: The image object.
        str: The path to the image file.
    """

    image_name = image_url.split("/")[-1]
    if not is_image(image_name):
        raise ValueError("Invalid image file extension")
    response = requests.get(image_url)
    if response.status_code != 200:
        return None, None
    image_data = io.BytesIO(response.content)
    try:
        image = Image.open(image_data)
        image_path = None
        if image_dir:
            image_path = os.path.join(image_dir, image_name)
            image.save(image_path)
        return image, image_path
    except Exception as e:
        return None, None
