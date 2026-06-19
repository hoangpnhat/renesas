import logging
import os
import platform
import shutil
import subprocess
import tempfile
import time
from contextlib import contextmanager
from io import BytesIO
from pathlib import Path
from typing import Optional, List, Dict, Any, Generator
from const.upload import FILE_ALLOWED_EXTENSIONS

from pdf2image import convert_from_bytes

from core import settings

logger = logging.getLogger(__name__)

SPIRE_AFFECTED_EXTENSIONS = {'.xlsm', '.xlsx'}

class LibreOfficeNotFoundError(Exception):
    pass


class ConversionError(Exception):
    pass


class FileConverterService:

    LIBREOFFICE_PATHS = {
        "Windows": [
            r"C:\Program Files\LibreOffice\program\soffice.exe",
            r"C:\Program Files (x86)\LibreOffice\program\soffice.exe",
            r"C:\Program Files\LibreOffice 7\program\soffice.exe",
            r"C:\Program Files\LibreOffice 24\program\soffice.exe",
        ],
        "Darwin": [
            "/Applications/LibreOffice.app/Contents/MacOS/soffice",
            "/usr/local/bin/soffice",
        ],
        "Linux": ["/usr/local/bin/soffice", "/usr/bin/soffice"],
    }


    MAX_RETRIES = 3
    RETRY_DELAY = 1

    def __init__(
        self,
        poppler_path: Optional[str] = None,
        dpi: int = 300,
        image_format: str = "PNG"
    ):
        self.poppler_path = poppler_path if poppler_path is not None else '/usr/bin'
        self.dpi = dpi
        self.image_format = image_format.upper()
        self._libreoffice_path: Optional[str] = None
        self._libreoffice_available: Optional[bool] = None

    @property
    def libreoffice_path(self) -> Optional[str]:
        if self._libreoffice_path is None:
            self._libreoffice_path = self._detect_libreoffice()
        return self._libreoffice_path

    def _detect_libreoffice(self) -> Optional[str]:
        system = platform.system()

        paths_to_check = self.LIBREOFFICE_PATHS.get(system, [])
        for path in paths_to_check:
            if os.path.isfile(path):
                logger.info(f"LibreOffice found at: {path}")
                return path

        for cmd in ['soffice', 'libreoffice']:
            soffice_path = shutil.which(cmd)
            if soffice_path:
                logger.info(f"LibreOffice found in PATH: {soffice_path}")
                return soffice_path

        logger.warning("LibreOffice not found")
        return None

    def is_libreoffice_available(self) -> bool:
        if self._libreoffice_available is not None:
            return self._libreoffice_available

        path = self.libreoffice_path
        if not path:
            self._libreoffice_available = False
            return False

        if not os.path.isfile(path):
            self._libreoffice_available = False
            return False

        self._libreoffice_available = True
        return True

    def get_file_type(self, file_name: str) -> str | None:
        ext = Path(file_name).suffix.lower()
        return FILE_ALLOWED_EXTENSIONS.get(ext)

    @contextmanager
    def _temp_file(self, suffix: str) -> Generator[str, None, None]:
        temp = tempfile.NamedTemporaryFile(delete=False, suffix=suffix)
        temp_path = temp.name
        temp.close()

        try:
            yield temp_path
        finally:
            if os.path.exists(temp_path):
                try:
                    os.remove(temp_path)
                except Exception as e:
                    logger.warning(f"Failed to remove temp file {temp_path}: {e}")

    @contextmanager
    def _temp_directory(self) -> Generator[str, None, None]:
        temp_dir = tempfile.mkdtemp()

        try:
            yield temp_dir
        finally:
            if os.path.exists(temp_dir):
                try:
                    shutil.rmtree(temp_dir)
                except Exception as e:
                    logger.warning(f"Failed to remove temp directory {temp_dir}: {e}")

    def _run_libreoffice_conversion(
        self,
        input_path: str,
        output_dir: str,
        timeout: int = 180
    ) -> subprocess.CompletedProcess:
        if not self.libreoffice_path:
            raise LibreOfficeNotFoundError(
                "LibreOffice is not installed. "
                "Install from: https://www.libreoffice.org/download/"
            )

        ext = Path(input_path).suffix.lower()
        if ext in SPIRE_AFFECTED_EXTENSIONS:
            json_filter = (
                '{"SinglePageSheets":{"type":"boolean","value":"true"},'
                '"ScaleToPages":{"type":"integer","value":"1"}}'
            )
            convert_to = f'pdf:calc_pdf_Export:{json_filter}'
        else:
            convert_to = 'pdf'

        cmd = [
            self.libreoffice_path,
            '--headless',
            '--convert-to', convert_to,
            '--outdir', output_dir,
            input_path
        ]

        logger.info(f"Running LibreOffice conversion: {Path(input_path).name}")

        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=timeout
            )

            if result.returncode != 0:
                raise ConversionError(
                    f"LibreOffice conversion failed with exit code {result.returncode}: "
                    f"{result.stderr}"
                )

            return result

        except subprocess.TimeoutExpired as e:
            raise ConversionError(
                f"LibreOffice conversion timed out after {timeout}s"
            ) from e

    def convert_office_to_pdf_bytes(
        self,
        office_bytes: bytes,
        file_extension: str
    ) -> bytes:
        if not self.is_libreoffice_available():
            raise LibreOfficeNotFoundError(
                "LibreOffice is required for Office file conversion. "
                "Install from: https://www.libreoffice.org/download/"
            )

        if not office_bytes:
            raise ValueError("Empty file content")

        for attempt in range(self.MAX_RETRIES):
            try:
                with self._temp_file(file_extension) as input_path, \
                     self._temp_directory() as output_dir:

                    with open(input_path, 'wb') as f:
                        f.write(office_bytes)

                    logger.info(
                        f"Created temp input: {Path(input_path).name} "
                        f"({len(office_bytes)} bytes)"
                    )

                    self._run_libreoffice_conversion(input_path, output_dir)

                    pdf_files = list(Path(output_dir).glob('*.pdf'))
                    if not pdf_files:
                        all_files = list(Path(output_dir).glob('*'))
                        raise ConversionError(
                            f"No PDF created. Files in output: "
                            f"{[f.name for f in all_files]}"
                        )

                    pdf_path = pdf_files[0]
                    pdf_size = pdf_path.stat().st_size

                    if pdf_size == 0:
                        raise ConversionError("Generated PDF is empty")

                    logger.info(f"PDF created: {pdf_path.name} ({pdf_size} bytes)")

                    with open(pdf_path, 'rb') as pdf_file:
                        pdf_bytes = pdf_file.read()

                    logger.info(f"Converted to PDF: {len(pdf_bytes)} bytes")
                    return pdf_bytes

            except (ConversionError, OSError) as e:
                if attempt < self.MAX_RETRIES - 1:
                    delay = self.RETRY_DELAY * (2 ** attempt)
                    logger.warning(
                        f"Conversion attempt {attempt + 1} failed: {e}. "
                        f"Retrying in {delay}s..."
                    )
                    time.sleep(delay)
                else:
                    logger.error(f"Conversion failed after {self.MAX_RETRIES} attempts")
                    raise
        raise ConversionError("Conversion failed after all retries")

    def _check_poppler_available(self) -> bool:
        import subprocess
        try:
            result = subprocess.run(
                ['pdftoppm', '-v'],
                capture_output=True,
                timeout=5
            )
            return result.returncode == 0
        except (FileNotFoundError, subprocess.TimeoutExpired):
            return False
        except Exception as e:
            logger.warning(f"Could not verify Poppler: {e}")
            return False

    def _convert_pdf_to_images(
        self,
        pdf_bytes: bytes,
        pages: Optional[List[int]] = None
    ) -> List:
        if not pdf_bytes:
            raise ValueError("Empty PDF content")

        if not self._check_poppler_available():
            raise ConversionError(
                "Poppler (pdftoppm) is not installed or not in PATH. "
                "Install with: sudo apt-get install -y poppler-utils"
            )

        logger.info(
            f"Converting PDF to images (DPI: {self.dpi}, Format: {self.image_format})"
        )

        try:
            if pages:
                pages_sorted = sorted(pages)
                first_page = pages_sorted[0]
                last_page = pages_sorted[-1]

                logger.info(f"Converting pages {first_page}-{last_page}")

                pil_images = convert_from_bytes(
                    pdf_bytes,
                    dpi=self.dpi,
                    first_page=first_page,
                    last_page=last_page,
                    poppler_path=self.poppler_path
                )

                page_offset = first_page - 1
                filtered_images = []
                actual_page_numbers = []

                for i, img in enumerate(pil_images):
                    actual_page = i + first_page
                    if actual_page in pages:
                        filtered_images.append(img)
                        actual_page_numbers.append(actual_page)

                return list(zip(filtered_images, actual_page_numbers))
            else:
                logger.info("Converting all pages")
                pil_images = convert_from_bytes(
                    pdf_bytes,
                    dpi=self.dpi,
                    poppler_path=self.poppler_path
                )
                page_numbers = list(range(1, len(pil_images) + 1))
                return list(zip(pil_images, page_numbers))
        except Exception as e:
            logger.exception(f"PDF to image conversion failed: {e}")
            raise ConversionError(f"PDF conversion failed: {str(e)}")

    def convert_file_to_image_bytes(
        self,
        file_bytes: bytes,
        file_name: str,
        pages: Optional[List[int]] = None
    ) -> Dict[str, Any]:
        try:
            file_type = self.get_file_type(file_name)
            if not file_type:
                return {
                    "success": False,
                    "error": f"Unsupported file type: {Path(file_name).suffix}",
                    "images": [],
                    "total_pages": 0
                }

            if file_type != 'pdf' and not self.is_libreoffice_available():
                return {
                    "success": False,
                    "error": "LibreOffice is required for Office file conversion",
                    "images": [],
                    "total_pages": 0
                }

            logger.info(f"Converting file: {file_name} (type: {file_type})")

            if file_type == 'pdf':
                pdf_bytes = file_bytes
                logger.info(f"Using original PDF: {len(pdf_bytes)} bytes")
            else:
                file_extension = Path(file_name).suffix
                logger.info(f"Converting {file_extension} to PDF")
                pdf_bytes = self.convert_office_to_pdf_bytes(file_bytes, file_extension)

            if not pdf_bytes:
                raise ValueError("PDF conversion produced empty result")

            images_with_pages = self._convert_pdf_to_images(pdf_bytes, pages)

            image_bytes_list = []
            file_stem = Path(file_name).stem

            for pil_image, page_num in images_with_pages:
                img_bytes = BytesIO()
                pil_image.save(img_bytes, format=self.image_format)
                img_bytes.seek(0)

                image_bytes_list.append({
                    'bytes': img_bytes,
                    'page_num': page_num,
                    'filename': f"{file_stem}_page_{page_num}.{self.image_format.lower()}"
                })

                logger.info(f"Converted page {page_num} to {self.image_format}")

            logger.info(
                f"Conversion complete: {len(image_bytes_list)} images created in memory"
            )

            return {
                "success": True,
                "images": image_bytes_list,
                "total_pages": len(image_bytes_list),
                "error": None
            }

        except (LibreOfficeNotFoundError, ConversionError) as e:
            logger.error(f"Conversion failed: {e}")
            return {
                "success": False,
                "error": str(e),
                "images": [],
                "total_pages": 0
            }
        except Exception as e:
            logger.exception(f"Unexpected conversion error: {e}")
            return {
                "success": False,
                "error": f"Conversion failed: {str(e)}",
                "images": [],
                "total_pages": 0
            }


class DatabricksStorageUploader:

    def __init__(
        self,
        workspace_url: str,
        token: str,
        volume_path: str = "/Volumes/catalog/schema/volume"
    ):
        self.workspace_url = workspace_url.rstrip('/')
        self.token = token
        self.volume_path = volume_path.rstrip('/')
        self._session: Any = None

    def _get_session(self):
        if self._session is None:
            import requests
            self._session = requests.Session()
            self._session.headers.update({
                "Authorization": f"Bearer {self.token}"
            })
        return self._session

    def upload_file_bytes(
        self,
        file_bytes: BytesIO,
        file_path: str,
        overwrite: bool = True
    ) -> str:
        full_path = f"{self.volume_path}/{file_path}"
        url = f"{self.workspace_url}/api/2.0/fs/files{full_path}"

        session = self._get_session()
        response = session.put(
            url,
            data=file_bytes.getvalue(),
            params={"overwrite": str(overwrite).lower()}
        )

        if response.status_code != 200:
            raise Exception(
                f"Databricks upload failed: {response.status_code} - {response.text}"
            )

        logger.info(f"Uploaded to Databricks: {full_path}")
        return full_path

    def upload_images(
        self,
        images: List[Dict[str, Any]],
        file_id: str
    ) -> List[str]:
        uploaded_paths = []

        for img_data in images:
            try:
                relative_path = f"{file_id}/{img_data['filename']}"
                full_path = self.upload_file_bytes(
                    img_data['bytes'],
                    relative_path
                )
                uploaded_paths.append(full_path)

            except Exception as e:
                logger.error(f"Failed to upload page {img_data['page_num']}: {e}")
                raise

        logger.info(f"Uploaded {len(uploaded_paths)} images to Databricks Volume")
        return uploaded_paths


_converter_services: dict[int, FileConverterService] = {}

def get_converter_service(dpi: int = 200) -> FileConverterService:
    if dpi not in _converter_services:
        _converter_services[dpi] = FileConverterService(
            poppler_path=getattr(settings, 'POPPLER_PATH', None),
            dpi=dpi,
            image_format=getattr(settings, 'IMAGE_FORMAT', 'PNG')
        )
    return _converter_services[dpi]
