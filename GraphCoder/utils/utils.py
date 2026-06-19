import glob
import networkx as nx
import pickle
import json
import os
from networkx.readwrite import json_graph
import tiktoken

device = "cuda"


class CodeGenTokenizer:
    def __init__(self, tokenizer_raw):
        self.tokenizer = tokenizer_raw

    def tokenize(self, text):
        return self.tokenizer(text, return_tensors="pt")['input_ids'][0].to(device)

    def decode(self, token_ids):
        return self.tokenizer.decode(token_ids, skip_special_tokens=True)


class StarCoderTokenizer:
    def __init__(self, tokenizer_raw):
        self.tokenizer = tokenizer_raw

    def tokenize(self, text):
        return self.tokenizer.encode(text, return_tensors="pt")[0].to(device)

    def decode(self, token_ids):
        return self.tokenizer.decode(token_ids, skip_special_tokens=True)


class CONSTANTS:
    max_hop = 5
    max_search_top_k = 10
    max_statement = 20
    repo_base_dir = f"./repositories"
    graph_database_save_dir = f"./context_database"
    query_graph_save_dir = "./graph_based_query"
    dataset_dir = "./RepoEval-Updated"
    # repos =["devchat"]
    # repos_language ={
    #                  "devchat":"python"}
    repos =["versa"]
    repos_language ={
                     "versa":"c"}
    

    # repos = ["devchat",
    #          "nemo_aligner",
    #          "awslabs_fortuna",
    #          "task_weaver",
    #          "huggingface_diffusers",
    #          "opendilab_ACE",
    #          "metagpt",
    #          "apple_axlearn",
    #          "QingruZhang_AdaLoRA",
    #          "nerfstudio-project_nerfstudio",
    #          "itlemon_chatgpt4j",
    #          "Aelysium-Group_rusty-connector",
    #          "neoforged_NeoGradle",
    #          "mybatis-flex_mybatis-flex",
    #          "Guiqu1aixi_rocketmq",
    #          "SimonHalvdansson_Harmonic-HN",
    #          "Open-DBT_open-dbt",
    #          "QuasiStellar_custom-pixel-dungeon",
    #          "gentics_cms-oss",
    #          "FloatingPoint-MC_MIN"
    #          ]

    # repos_language = {
    #      "devchat": "python",
    #      "nemo_aligner": "python",
    #      "awslabs_fortuna": "python",
    #      "task_weaver": "python",
    #      "huggingface_diffusers": "python",
    #      "opendilab_ACE": "python",
    #      "metagpt": "python",
    #      "apple_axlearn": "python",
    #      "QingruZhang_AdaLoRA": "python",
    #      "nerfstudio-project_nerfstudio": "python",
    #      "itlemon_chatgpt4j": "java",
    #      "Aelysium-Group_rusty-connector": "java",
    #      "neoforged_NeoGradle": "java",
    #      "mybatis-flex_mybatis-flex": "java",
    #      "Guiqu1aixi_rocketmq": "java",
    #      "SimonHalvdansson_Harmonic-HN": "java",
    #      "Open-DBT_open-dbt": "java",
    #      "QuasiStellar_custom-pixel-dungeon": "java",
    #      "gentics_cms-oss": "java",
    #      "FloatingPoint-MC_MIN": "java"
    # }


class CodexTokenizer:
    def __init__(self):
        # self.tokenizer = tiktoken.encoding_for_model("gpt-3.5-turbo-instruct")
        # self.tokenizer = tiktoken.get_encoding("p50k_base")
        self.tokenizer = tiktoken.encoding_for_model("gpt-3.5")
    def tokenize(self, text):
        # return self.tokenizer.encode(text)
        return self.tokenizer.encode(text, allowed_special={'<|endoftext|>'})

    def decode(self, token_ids):
        return self.tokenizer.decode(token_ids)

class DatabricksEmbedding:
    def __init__(self, databricks_token=None, base_url=None):
        """
        Initialize the DatabricksEmbedding class.
        Args:
            databricks_token (str, optional): Databricks API token. If None, will try to get from environment
            base_url (str, optional): Base URL for the Databricks endpoint
        """
        from openai import OpenAI
        import os
        
        self.token = databricks_token or os.environ.get('DATABRICKS_TOKEN')
        self.base_url = base_url or "https://adb-379144824042062.2.azuredatabricks.net/serving-endpoints"
        
        self.client = OpenAI(
            api_key=self.token,
            base_url=self.base_url
        )
        
    def get_embedding(self, text, model="databricks-bge-large-en"):
        """
        Get embeddings for the input text.
        Args:
            text (str): Input text to get embeddings for
            model (str): Model name to use for embeddings
        Returns:
            list: The embedding vector
        """
        response = self.client.embeddings.create(
            input=text,
            model=model
        )
        return response.data[0].embedding

    def get_batch_embeddings(self, texts, model="databricks-bge-large-en"):
        """
        Get embeddings for multiple texts.
        Args:
            texts (list): List of input texts to get embeddings for
            model (str): Model name to use for embeddings
        Returns:
            list: List of embedding vectors
        """
        response = self.client.embeddings.create(
            input=texts,
            model=model
        )
        return [item.embedding for item in response.data]
class CodeGenTokenizer:
    def __init__(self, tokenizer):
        self.tokenizer = tokenizer

    def tokenize(self, text):
        return self.tokenizer.encode(text)

    def decode(self, token_ids):
        return self.tokenizer.decode(token_ids)

def iterate_repository_file(base_dir, repo):
    """
    Iterates through the files in the repository based on the language of the repository.

    Args:
        base_dir (str): The base directory containing repositories.
        repo (str): The name of the repository.

    Returns:
        list: A list of file paths that match the repository's language.
    """
    if CONSTANTS.repos_language[repo] == "python":
        extensions = [".py"]
    elif CONSTANTS.repos_language[repo] == "java":
        extensions = [".java"]
    elif CONSTANTS.repos_language[repo] == "c":
        extensions = [".c",".C",".h",".H"]
    else:
        raise NotImplementedError(f"Language '{CONSTANTS.repos_language[repo]}' is not supported.")
    
    # For python/java cases, if pattern is defined, search for files
    file_paths = []

    for root, _, files in os.walk(f'{base_dir}/{repo}'):
        for file in files:
            if os.path.splitext(file)[1] in extensions:
                file_paths.append(os.path.join(root, file))
    
    return file_paths

def iterate_repository_json_file(base_dir, repo):
    pattern = os.path.join(f'{base_dir}/{repo}', "**", "*.json")
    files = glob.glob(pattern, recursive=True)
    return files


def make_needed_dir(file_path):
    dir_path = os.path.dirname(file_path)
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)
    return


def load_pickle(fname):
    with open(fname, 'rb') as f:
        return pickle.load(f)


def set_default(obj):
    if isinstance(obj, set):
        return list(obj)
    raise TypeError


def dump_jsonl(obj, fname):
    with open(fname, 'w', encoding='utf8') as f:
        for item in obj:
            f.write(json.dumps(item, default=set_default) + '\n')


def load_jsonl(fname):
    with open(fname, 'r', encoding='utf8') as f:
        lines = []
        for line in f:
            lines.append(json.loads(line))
        return lines


def graph_to_json(obj: nx.MultiDiGraph):
    return json.dumps(json_graph.node_link_data(obj), default=set_default)


def json_to_graph(json_format):
    graph_js = json.loads(json_format)
    graph = json_graph.node_link_graph(graph_js)
    return graph


def tokenize(code):
    tokenizer = CodexTokenizer()
    return tokenizer.tokenize(code)

def merge_docstrings(lines):
    """
    Read a Python file and merge multi-line docstrings into single lines.
    
    Args:
        lines (list): line string of file
        
    Returns:
        list: List of processed lines with merged docstrings
    """

    
    merged_lines = []
    is_in_docstring = False
    current_docstring = ''
    
    for line in lines:
        # Check for docstring delimiters
        if '"""' in line:
            if not is_in_docstring:
                # Start of docstring
                is_in_docstring = True
                current_docstring = line
            else:
                # End of docstring
                is_in_docstring = False
                # Add the ending part of the line after the closing quotes
                current_docstring += line[line.index('"""') + 3:]
                # Clean up whitespace and add to merged lines
                # merged_line = current_docstring.replace('    ', ' ').replace('  ', ' ').strip()
                merged_lines.append(current_docstring)
                current_docstring = ''
        elif is_in_docstring:
            # Inside docstring, append to current docstring
            current_docstring += ' ' + line.strip()
        else:
            # Regular line
            merged_lines.append(line)
    
    return merged_lines


def convert_docs_to_comments(lines, language):
    """Convert documentation to single-line comments for Python, C, and Java files."""
    # Determine file type from extension
    
    merged_lines = []
    is_in_block_comment = False
    current_comment = ''
    
    # Set documentation and comment patterns based on file type
    if language == 'python':
        single_line_comment = '#'
        block_comment_start = '"""'
        block_comment_end = '"""'
    elif language == 'c':
        single_line_comment = '//'
        block_comment_start = '/*'
        block_comment_end = '*/'
    elif language == 'java':
        single_line_comment = '//'
        block_comment_start = '/**'
        block_comment_end = '*/'
    else:
        raise ValueError(f"Unsupported file type: {language}")
    
    i = 0
    while i < len(lines):
        line = lines[i]  # Keep original line with newline
        stripped_line = line.rstrip()
        
        # Handle single-line block comments with code on the same line
        if block_comment_start in line and block_comment_end in line:
            # Split the line into code and comment parts
            parts = line.split('/*')
            code_part = parts[0]
            comment_part = parts[1].split('*/')[0].strip()
            rest_of_line = line[line.index('*/') + 2:]
            
            # Build the new line with code and converted comment
            new_line = ''
            if code_part.strip():
                new_line += code_part
            
            if comment_part.strip():
                if code_part.strip():
                    # If there's code, align comment with proper spacing
                    if len(code_part.rstrip()) < 40:
                        # Pad to column 40 if original code is shorter
                        new_line = new_line.rstrip() + ' ' * (40 - len(code_part.rstrip()))
                    else:
                        # Add standard spacing if code is longer
                        new_line = new_line.rstrip() + ' ' * 4
                else:
                    # If no code, use original indentation
                    new_line += get_indentation(line)
                new_line += f"{single_line_comment} {comment_part}"
            
            # Add any remaining code after the comment
            if rest_of_line.strip():
                new_line += rest_of_line
            
            # Preserve original newline
            if line.endswith('\n'):
                new_line += '\n'
            
            merged_lines.append(new_line)
            
        # Handle multi-line block comments
        elif block_comment_start in line and not is_in_block_comment:
            is_in_block_comment = True
            # Keep any code before the comment
            code_part = line[:line.index(block_comment_start)]
            if code_part.strip():
                merged_lines.append(code_part)
            start_idx = line.index(block_comment_start)
            current_comment = line[start_idx + len(block_comment_start):].strip()
            
        elif block_comment_end in line and is_in_block_comment:
            is_in_block_comment = False
            end_idx = line.index(block_comment_end)
            current_comment += ' ' + line[:end_idx].strip()
            
            # Convert to single-line comment
            indentation = get_indentation(line)
            comment_line = f"{indentation}{single_line_comment} {current_comment.strip()}"
            if line.endswith('\n'):
                comment_line += '\n'
            merged_lines.append(comment_line)
            
            # Keep any code after the comment
            code_after = line[end_idx + 2:]
            if code_after.strip():
                merged_lines.append(code_after)
            
            current_comment = ''
            
        elif is_in_block_comment:
            current_comment += ' ' + stripped_line.lstrip('* ')
            
        else:
            merged_lines.append(line)
        
        i += 1
    
    return merged_lines

def get_indentation(line):
    """Return the indentation (spaces) at the start of the line."""
    return ' ' * (len(line) - len(line.lstrip()))

def clean_doc_text(text):
    """Clean up documentation text by removing extra whitespace and normalizing spacing."""
    # Remove extra whitespace
    text = ' '.join(text.split())
    # Remove common documentation patterns
    text = text.replace('@param', 'param:')
    text = text.replace('@return', 'return:')
    text = text.replace('@throws', 'throws:')
    return text.strip()

