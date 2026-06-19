import os
import argparse
import re
import json
import glob


def extract_code_elements(file_content):
    # Updated regular expression to match C function signatures
    header_pattern = r"^[a-zA-Z_]\w*\s+[*\w]+\s*\([^)]*\)\s*{"

    # Find all function headers
    headers = [
        match.start()
        for match in re.finditer(header_pattern, file_content, re.MULTILINE)
    ]

    functions = []

    for i in range(len(headers)):
        start = headers[i]
        # Determine the end of the current function
        if i + 1 < len(headers):
            end = headers[i + 1]
        else:
            end = len(file_content)

        # Extract function body
        function_body = file_content[start:end]

        # Count braces to find the function's end
        brace_count = 1  # Start with 1 because we've already found the opening brace
        function_end = function_body.find("{") + 1

        while brace_count > 0 and function_end < len(function_body):
            if function_body[function_end] == "{":
                brace_count += 1
            elif function_body[function_end] == "}":
                brace_count -= 1
            function_end += 1

        # Append the function (including the closing brace)
        functions.append(function_body[:function_end].strip())

    include_pattern = r"^\s*#include.*$"
    includes = re.finditer(include_pattern, file_content, re.MULTILINE)
    include_statements = [match.group() for match in includes]

    # variable_pattern = r'^\s*(?:static\s+)?(?:const\s+)?(?:unsigned\s+)?\w+(?:\s+\w+)*(?:\s*\[\s*\w*\s*\])*\s*(?:=\s*[^;]+)?\s*;'
    # variables = re.finditer(variable_pattern, file_content, re.MULTILINE)
    # variable_declarations = [match.group() for match in variables]

    # # Find other non-function code (excluding whitespace-only lines)
    # lines = file_content.split('\n')
    # other_code = [line.strip() for line in lines if line.strip() and
    #               not any(line.strip() in s for s in (functions + include_statements + variable_declarations))]
    function_dict = [{"original_code": func} for func in functions]
    return {
        "functions": function_dict,
        "includes": include_statements,
        # 'variables': variable_declarations,
        # 'other_code': other_code
    }


def chunk_function(c_code):
    # Initialize storage for chunks
    chunks = []

    # Split lines for easier processing
    lines = c_code.split("\n")

    # Variables to help in chunking
    current_chunk = []
    in_block = False

    # Define regex for detecting function signatures (common C types)
    function_signature_regex = (
        r"^\s*(void|int|char|float|double|long|short|bool)\s+\w+\s*\(.*\)\s*\{"
    )

    # Define control structures that indicate a new logical block
    control_block_start = ("for", "while", "if", "else", "switch", "case", "do")

    # Control flow statements that should not be chunked separately
    control_flow_statements = ("break", "continue")

    for line in lines:
        stripped_line = line.strip()

        # Detect function signature
        if re.match(function_signature_regex, stripped_line):
            if current_chunk:
                chunks.append("\n".join(current_chunk))
                current_chunk = []
            current_chunk.append(line)

        # Detect control block start
        elif any(stripped_line.startswith(block) for block in control_block_start):
            if current_chunk:
                chunks.append("\n".join(current_chunk))
                current_chunk = []
            current_chunk.append(line)
            in_block = True

        # Detect control flow statements like 'break' and 'continue'
        elif any(
            stripped_line.startswith(statement) for statement in control_flow_statements
        ):
            current_chunk.append(line)

        # Detect the end of control blocks
        elif stripped_line == "}":
            current_chunk.append(line)
            in_block = False
            chunks.append("\n".join(current_chunk))
            current_chunk = []

        # Add regular lines
        else:
            current_chunk.append(line)

    # Add any remaining chunk
    if current_chunk:
        chunks.append("\n".join(current_chunk))

    return chunks


def merge_chunks(chunks, max_lines=15):
    merged_chunks = []
    current_chunk = []

    for chunk in chunks:
        chunk_lines = chunk.split("\n")

        # If adding this chunk will not exceed the line limit, add it
        if len(current_chunk) + len(chunk_lines) <= max_lines:
            current_chunk.extend(chunk_lines)
        else:
            # If adding exceeds limit, finalize current chunk and start a new one
            if len(current_chunk) > 0:
                merged_chunks.append("\n".join(current_chunk))
            current_chunk = chunk_lines

    # Add any remaining lines in the final chunk
    if current_chunk and len(current_chunk) > 0:
        merged_chunks.append("\n".join(current_chunk))

    return merged_chunks


def extract_and_chunk_code(folder_path, output_path):
    # List all .c files in the folder with full paths
    c_files = glob.glob(folder_path + "/**/*.c", recursive=True)
    json_code = []

    # Iterate over each .c file
    for file_path in c_files:
        # Extract the file name from the full path
        file_name = os.path.basename(file_path)

        # Read the file content
        with open(file_path, "r", encoding="latin-1") as file:
            file_content = file.read()

        # Assuming extract_code_elements is a function that extracts the required elements
        elements = extract_code_elements(file_content)

        # Process each function and chunk it
        for func in elements["functions"]:
            chunks = chunk_function(func["original_code"])
            func["chunks"] = merge_chunks(chunks, max_lines=25)

        # Append extracted code elements to the json_code list
        for code_type, code_list in elements.items():
            for code in code_list:
                if code_type == "functions":  # Handle functions with 'chunks'
                    if len(code["chunks"]) >= 2:
                        json_code.append(
                            {
                                "original_code": code.get("original_code"),
                                "chunks": code.get("chunks"),
                                "code_type": code_type,
                                "original_file": file_name,
                            }
                        )
                    else:
                        json_code.append(
                            {
                                "original_code": code.get("original_code"),
                                "chunks": None,
                                "code_type": code_type,
                                "original_file": file_name,
                            }
                        )
                else:
                    json_code.append(
                        {
                            "original_code": code,
                            "chunks": None,
                            "code_type": code_type,
                            "original_file": file_name,
                        }
                    )

    # Write the JSON output to the specified file
    with open(output_path, "w", encoding="utf-8") as file:
        json.dump(json_code, file, ensure_ascii=False, indent=4)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--folder_path",
        type=str,
        default=r"AI-CodingAssistant\data\RC04EX\Code\RC04EX_Io_Versa",
    )
    parser.add_argument(
        "--output_path", type=str, default=r"AI-CodingAssistant\data\code_elements.json"
    )
    args = parser.parse_args()
    extract_and_chunk_code(args.folder_path, args.output_path)


if __name__ == "__main__":
    main()
