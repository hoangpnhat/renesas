export const GET_FILE_FROM_URL_REGEX = /[^/]+\.pptx\b/;
export const GET_EXTENSION_FROM_URL = /\.([a-zA-Z0-9]+)$/;
export const GET_FILE_NAME_FROM_URL = /[^/]+\.[A-Za-z0-9]+$/;
export const URL_VALIDATION = new RegExp(
  "^(https?:\\/\\/)?" + // Protocol (optional)
    "((([a-z\\d]([a-z\\d-]*[a-z\\d])*)\\.)+[a-z]{2,}|" + // Domain name
    "((\\d{1,3}\\.){3}\\d{1,3}))" + // OR IPv4 address
    "(\\:\\d+)?" + // Port (optional)
    "(\\/[\\-a-z\\d%_.~+():]*)*" + // Path (added parentheses)
    "(\\?[;&a-z\\d%_.~+=-]*)?" + // Query string
    "(\\#[-a-z\\d_]*)?$", // Fragment
  "i", // Case-insensitive
);
