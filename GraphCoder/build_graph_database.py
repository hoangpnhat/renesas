import os
import json
import re
from tqdm import tqdm
from networkx.readwrite import json_graph
from utils.utils import CONSTANTS, CodexTokenizer, DatabricksEmbedding
from utils.slicing import Slicing
# from utils.cgg import create_graph
from utils.cgg_c import create_graph
from utils.utils import iterate_repository_file, make_needed_dir, set_default, dump_jsonl, graph_to_json
from pyvis.network import Network

def separate_code_and_comment(line):
    """
    Separates a C code line containing a comment into two lines.

    Parameters:
        line (str): The input line of C code.

    Returns:
        tuple: A tuple containing the code part and the comment part as separate strings.
    """
    # Regular expression to match code and comment (// or /* */)
    match = re.match(r'(.*?)(//.*|/\*.*?\*/)', line)
    if match:
        code = match.group(1).rstrip() + "\n"  # Code part with newline added
        comment = match.group(2).lstrip() + "\n"  # Comment part with newline added
        return code, comment
    return line.strip() + "\n", None  # Return the line with newline if no comment found

def process_c_file(file_path, repo_name):
    """
    Process C file through a pipeline of transformations while preserving indentation:
    1. Clean printf statements (remove \n)
    2. Separate code and comments
    3. Convert block comments to single-line comments
    4. Save the processed file
    """
    repo_dump = os.path.join('data', f"{repo_name}")
    os.makedirs(repo_dump, exist_ok=True)
    
    file_name = os.path.basename(file_path)
    output_path = os.path.join(repo_dump, f"{file_name}")

    # Read input file
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()

    # Step 1: Clean printf statements while preserving indentation
    printf_cleaned_lines = []
    i = 0
    while i < len(lines):
        current_line = lines[i]
        # Preserve original indentation by not stripping the line
        stripped_line = current_line.rstrip('\n').strip()
        indent = current_line[:len(current_line) - len(current_line.lstrip())]
        
        if 'printf' in stripped_line and not stripped_line.endswith(';'):
            # Handle multi-line printf
            full_printf = stripped_line
            j = i + 1
            while j < len(lines) and ';' not in lines[j]:
                full_printf += ' ' + lines[j].strip()
                j += 1
            if j < len(lines):
                full_printf += ' ' + lines[j].strip()
            
            if 'printf' in full_printf:
                if 'printf("\\n")' in full_printf or 'printf("\\n\\n")' in full_printf:
                    full_printf = full_printf.replace('printf("\\n")', 'printf("")').replace('printf("\\n\\n")', 'printf("")')
                else:
                    parts = full_printf.split('printf')
                    prefix = parts[0]
                    printf_part = 'printf' + parts[1]
                    if '"' in printf_part:
                        start_quote = printf_part.find('"')
                        end_quote = printf_part.rfind('"')
                        if start_quote != -1 and end_quote != -1:
                            string_content = printf_part[start_quote:end_quote+1]
                            string_content = string_content.replace('\\n"', '"').replace('"\\n', '"').replace('\\n', '')
                            printf_part = printf_part[:start_quote] + string_content + printf_part[end_quote+1:]
                    full_printf = prefix + printf_part
            
            printf_cleaned_lines.append(indent + full_printf + '\n')
            i = j + 1
        else:
            if 'printf' in stripped_line:
                if 'printf("\\n");' in stripped_line or 'printf("\\n\\n");' in stripped_line:
                    stripped_line = stripped_line.replace('printf("\\n");', 'printf("");').replace('printf("\\n\\n");', 'printf("");')
                else:
                    parts = stripped_line.split('printf')
                    prefix = parts[0]
                    printf_part = 'printf' + parts[1]
                    if '"' in printf_part:
                        start_quote = printf_part.find('"')
                        end_quote = printf_part.rfind('"')
                        if start_quote != -1 and end_quote != -1:
                            string_content = printf_part[start_quote:end_quote+1]
                            string_content = string_content.replace('\\n"', '"').replace('"\\n', '"').replace('\\n', '')
                            printf_part = printf_part[:start_quote] + string_content + printf_part[end_quote+1:]
                    stripped_line = prefix + printf_part
                printf_cleaned_lines.append(indent + stripped_line + '\n')
            else:
                printf_cleaned_lines.append(current_line)
            i += 1

    # Step 2: Modify separate_comment_c_code_lines to preserve indentation
    def separate_comment_c_code_lines(lines):
        processed_lines = []
        for line in lines:
            original_indent = line[:len(line) - len(line.lstrip())]
            code, comment = separate_code_and_comment(line)
            if code:
                processed_lines.append(original_indent + code.lstrip())
            if comment:
                processed_lines.append(original_indent + comment.lstrip())
        return processed_lines

    # Step 3: Modify convert_docs_to_comments to preserve indentation
    def convert_docs_to_comments_with_indent(lines):
        merged_lines = []
        is_in_block_comment = False
        current_comment = ''
        original_indent = ''
        
        single_line_comment = '//'
        block_comment_start = '/*'
        block_comment_end = '*/'
        
        i = 0
        while i < len(lines):
            line = lines[i]
            original_indent = line[:len(line) - len(line.lstrip())]
            
            if block_comment_start in line and block_comment_end in line:
                parts = line.split('/*')
                code_part = parts[0]
                comment_part = parts[1].split('*/')[0].strip()
                rest_of_line = line[line.index('*/') + 2:]
                
                new_line = ''
                if code_part.strip():
                    new_line += original_indent + code_part.lstrip()
                
                if comment_part.strip():
                    if code_part.strip():
                        if len(code_part.rstrip()) < 40:
                            new_line = new_line.rstrip() + ' ' * (40 - len(new_line.rstrip()))
                        else:
                            new_line = new_line.rstrip() + ' ' * 4
                    else:
                        new_line += original_indent
                    new_line += f"{single_line_comment} {comment_part}"
                
                if rest_of_line.strip():
                    new_line += rest_of_line
                
                if line.endswith('\n'):
                    new_line += '\n'
                
                merged_lines.append(new_line)
                
            elif block_comment_start in line and not is_in_block_comment:
                is_in_block_comment = True
                code_part = line[:line.index(block_comment_start)]
                if code_part.strip():
                    merged_lines.append(original_indent + code_part.lstrip())
                start_idx = line.index(block_comment_start)
                current_comment = line[start_idx + len(block_comment_start):].strip()
                
            elif block_comment_end in line and is_in_block_comment:
                is_in_block_comment = False
                end_idx = line.index(block_comment_end)
                current_comment += ' ' + line[:end_idx].strip()
                
                comment_line = f"{original_indent}{single_line_comment} {current_comment.strip()}"
                if line.endswith('\n'):
                    comment_line += '\n'
                merged_lines.append(comment_line)
                
                code_after = line[end_idx + 2:]
                if code_after.strip():
                    merged_lines.append(original_indent + code_after.lstrip())
                
                current_comment = ''
                
            elif is_in_block_comment:
                current_comment += ' ' + line.strip().lstrip('* ')
                
            else:
                merged_lines.append(line)
            
            i += 1
        
        return merged_lines

    # Apply the pipeline while preserving indentation
    separated_lines = separate_comment_c_code_lines(printf_cleaned_lines)
    final_lines = convert_docs_to_comments_with_indent(separated_lines)

    # Write the processed content to output file
    with open(output_path, 'w', encoding='utf-8') as f:
        f.writelines(final_lines)

    return output_path


def save_graph_to_html(ccg, repo_name, file_path, base_save_dir):
    """
    Save a NetworkX MultiDiGraph to an HTML file using pyvis visualization.
    
    Args:
        ccg (networkx.MultiDiGraph): The graph to visualize
        repo_name (str): Name of the repository
        file_path (str): Original file path (used for naming)
        base_save_dir (str): Base directory where visualizations will be saved
    """
    # Create repository-specific folder
    repo_viz_dir = os.path.join(base_save_dir, f"{repo_name}_visualizations")
    os.makedirs(repo_viz_dir, exist_ok=True)
    
    # Create filename based on the original file path
    file_name = os.path.basename(file_path)
    output_path = os.path.join(repo_viz_dir, f"{file_name}.html")
    
    # Create a pyvis network
    net = Network(notebook=False, directed=True)
    
    # Add nodes
    for node in ccg.nodes():
        # Get node attributes if they exist
        node_attrs = ccg.nodes[node]
        title = f"line {node_attrs.get('startRow', 'N/A')}"
        label = "".join(node_attrs.get('sourceLines'))
        nodeType = node_attrs.get('nodeType')
        # Add node with attributes
        net.add_node(node, 
                    # title=nodeType +'-'+title,
                    label=label)
    
    # Add edges
    for edge in ccg.edges:
        source, target, data = edge
        # Add edge with any attributes if they exist
        net.add_edge(source = source, to=target,title =data)
    
    # Configure physics layout
    net.set_options("""
    {
        "physics": {
            "barnesHut": {
                "gravitationalConstant": -2000,
                "centralGravity": 0.3,
                "springLength": 95
            },
            "maxVelocity": 50,
            "minVelocity": 0.75
        }
    }
    """)
    
    # Save the visualization
    net.save_graph(output_path)
    return output_path

class GraphDatabaseBuilder:

    def __init__(self, repo_base_dir=CONSTANTS.repo_base_dir,
                 graph_database_save_dir=CONSTANTS.graph_database_save_dir):
        self.repo_base_dir = repo_base_dir
        self.graph_database_save_dir = graph_database_save_dir
        return

    def build_full_graph_database(self, repo_name):
        code_files = iterate_repository_file(self.repo_base_dir, repo_name)
        file_num = 0
        make_needed_dir(os.path.join(self.graph_database_save_dir, repo_name))
        with tqdm(total=len(code_files)) as pbar:
            for file in code_files:
                with open(file, 'r') as f:
                    src_lines = f.readlines()
                ccg = create_graph(src_lines, repo_name)
                if ccg is None:
                    pbar.update(1)
                    continue
                save_path = os.path.join(self.graph_database_save_dir, repo_name, f"{file_num}.json")
                file_num += 1
                make_needed_dir(save_path)
                with open(save_path, 'w') as f:
                    f.write(json.dumps(json_graph.node_link_data(pdg), default=set_default))
                pbar.update(1)
        return

    def build_slicing_graph_database(self, repo_name):

        slicer = Slicing()
        repo_dict = []

        # Get all file
        code_files = iterate_repository_file(self.repo_base_dir, repo_name)
        repo_base_dir_len = len(self.repo_base_dir.split('/'))
        tokenizer = CodexTokenizer()
        databrickEmbed= DatabricksEmbedding(databricks_token=os.environ.get("DATABRICKS_TOKEN", ""))
        # Create repository-specific folder
        output_path_html = os.path.join(self.graph_database_save_dir, f"{repo_name}_visualizations.html")
                
        # Create a pyvis network
        net = Network(notebook=False, directed=True)
        # Configure physics layout
        net.set_options("""
        {
            "physics": {
                "barnesHut": {
                    "gravitationalConstant": -2000,
                    "centralGravity": 0.3,
                    "springLength": 95
                },
                "maxVelocity": 50,
                "minVelocity": 0.75
            }
        }
        """)
        with tqdm(total=len(code_files)) as pbar:
            for file in code_files:
                # read file
                # if file =='./repositories/versa\RC04EX_WT.c':
                    file = process_c_file(file,repo_name)
                    pbar.set_description(file)
                    with open(file, 'r',encoding='utf-8') as f:
                        src_lines = f.readlines()

                    # get graph
                    ccg = create_graph(src_lines, repo_name)
                    if ccg is None:
                        pbar.update(1)
                        continue
                    save_graph_to_html(ccg=ccg,repo_name=repo_name,file_path=file,base_save_dir=self.graph_database_save_dir) 
                    # slicing for each statement
                    for v in ccg.nodes:
                        curr_dict = dict()
                        forward_context, forward_line, forward_graph = slicer.forward_dependency_slicing(v, ccg,contain_node=False)
                        curr_dict['key_forward_graph'] = graph_to_json(forward_graph)
                        curr_dict['key_forward_context'] = forward_context
                        curr_dict['key_forward_encoding'] = tokenizer.tokenize(forward_context)
                        curr_dict['statement'] = "".join(ccg.nodes[v]['sourceLines'])
                        statement_line_row = ccg.nodes[v]['startRow']
                        start_line_row = max(0, statement_line_row-11)
                        end_line_row = min(statement_line_row+10, len(src_lines))
                        curr_dict['val'] = "".join(src_lines[start_line_row:end_line_row])

                        curr_dict['key_forward_embedding']= databrickEmbed.get_embedding(curr_dict['val'])

                        # curr_dict['fpath_tuple'] = file.split('/')[repo_base_dir_len:]
                        curr_dict['fpath_tuple']= file.split('\\')[repo_base_dir_len:]
                        max_forward_line = 0
                        if len(forward_line) != 0:
                            max_forward_line = max(forward_line)
                        curr_dict['max_line_no'] = max(max_forward_line, end_line_row)
                        repo_dict.append(curr_dict.copy())


                        # Get node attributes if they exist
                        node_attrs = ccg.nodes[v]
                        title = f"Line {node_attrs.get('startRow', 'N/A')}"
                        label = "".join(node_attrs['sourceLines'])
                        
                        # Add node with attributes
                        net.add_node(v, 
                                    title=title,
                                    label=label)
                    pbar.update(1)
                    # Add edges
                    for edge in ccg.edges:
                        source, target, data = edge
                        # Add edge with any attributes if they exist
                        net.add_edge(source = source, to=target,title =data)
                    

        # Save the visualization
        net.save_graph(output_path_html)
        save_name = os.path.join(self.graph_database_save_dir, f"{repo}_test.jsonl")
        make_needed_dir(save_name)
        dump_jsonl(repo_dict, save_name)
        return


if __name__ == '__main__':
    graph_db_builder = GraphDatabaseBuilder()
    for repo in CONSTANTS.repos:
        print(f'Processing repo {repo}')
        graph_db_builder.build_slicing_graph_database(repo)


