import networkx as nx
from tree_sitter import Language, Parser
import tree_sitter_c as tsc
from utils.utils import CONSTANTS
import re



def c_control_dependence_graph(root_node, CCG, src_lines, parent, level=0):
    if level >= 5:
        return
    node_id = len(CCG.nodes)

    if root_node.type in ['preproc_if', 'preproc_ifdef', 'preproc_ifndef']:
    # if root_node.type in ['preproc_if']:

        start_row = root_node.start_point[0]
        end_row = root_node.start_point[0]
        # if root_node.type == 'preproc_ifdef':
        #     end_row = root_node.start_point[0]


        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        defSet=set(),
                        useSet=set())
            parent = node_id
        else:
            if not (CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row):
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row + 1],
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

        # Process identifiers in conditions
        if root_node.type == 'preproc_if':
            condition = root_node.child_by_field_name('condition')
        else: # ifdef/ifndef
            condition = root_node.child_by_field_name('name')
            
        if condition:
            for child in condition.children:
                if child.type == 'identifier':
                    row = child.start_point[0]
                    col_start = child.start_point[1]
                    col_end = child.end_point[1]
                    identifier_name = src_lines[row][col_start:col_end].strip()
                    CCG.nodes[parent]['useSet'].add(identifier_name)
    
    elif root_node.type in ['preproc_elif', 'preproc_else', 'preproc_endif']:
        start_row = root_node.start_point[0] 
        end_row = root_node.end_point[0]
        
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row, 
                        sourceLines=src_lines[start_row:end_row + 1],
                        defSet=set(),
                        useSet=set())
            parent = node_id
        else:
            if not (CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row):
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row + 1], 
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id
                
        # Handle condition for elif
        if root_node.type == 'preproc_elif':
            condition = root_node.child_by_field_name('condition')
            if condition:
                for child in condition.children:
                    if child.type == 'identifier':
                        row = child.start_point[0]
                        col_start = child.start_point[1]
                        col_end = child.end_point[1]
                        identifier_name = src_lines[row][col_start:col_end].strip()
                        CCG.nodes[parent]['useSet'].add(identifier_name)
    # Handle C-specific control structures
    elif root_node.type in ['preproc_include', 'preproc_def']:
        start_row = root_node.start_point[0]
        end_row = root_node.end_point[0]
        
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row +1],
                        defSet=set(),
                        useSet=set())
            parent = node_id
        else:
            if not (CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row):
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row+1],
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id
                
    elif root_node.type in ['function_definition', 'struct_specifier', 'enum_specifier']:
        start_row = root_node.start_point[0]
        end_row = root_node.child_by_field_name('declarator').end_point[0] if root_node.child_by_field_name('declarator') else root_node.start_point[0]
        
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        defSet=set(),
                        useSet=set())
            parent = node_id
        else:
            if not (CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row):
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row + 1],
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id
                
    elif root_node.type in ['while_statement', 'for_statement', 'do_statement']:
        start_row = root_node.start_point[0]
        if root_node.type == 'for_statement':
            end_row = root_node.child_by_field_name('initializer').end_point[0]
        elif root_node.type in ['while_statement', 'do_statement']:
            end_row = root_node.child_by_field_name('condition').end_point[0]
            
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        defSet=set(),
                        useSet=set())
            parent = node_id
        else:
            if not (CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row):
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row + 1],
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

    elif root_node.type in ['if_statement', 'else_clause', 'switch_statement', 'case_statement']:
        start_row = root_node.start_point[0]
        if root_node.type in ['if_statement', 'switch_statement']:
            end_row = root_node.child_by_field_name('condition').end_point[0]
        else:
            end_row = root_node.start_point[0]
            
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        defSet=set(),
                        useSet=set())
            parent = node_id
        else:
            if not (CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row):
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row + 1],
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id
    elif root_node.type == 'declaration':
        start_row = root_node.start_point[0]
        end_row = root_node.end_point[0]

        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,  
                        sourceLines=src_lines[start_row:end_row + 1],
                        defSet=set(),
                        useSet=set())
            parent = node_id 
        else:
            if not (CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row):
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row + 1],
                            defSet=set(), 
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id
    
    elif ('statement' in root_node.type and root_node.type != 'compound_statement') or 'ERROR' in root_node.type:
        start_row = root_node.start_point[0]
        end_row = root_node.end_point[0]
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                         startRow=start_row, endRow=end_row,
                         sourceLines=src_lines[start_row:end_row + 1],
                         defSet=set(),
                         useSet=set())
            parent = node_id
        else:
            if CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row:
                pass
            else:
                CCG.add_node(node_id, nodeType=root_node.type,
                             startRow=start_row, endRow=end_row,
                             sourceLines=src_lines[start_row:end_row + 1],
                             defSet=set(),
                             useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

    # Handle variable definitions and usage
    for child in root_node.children:
        if child.type == 'identifier':
            row = child.start_point[0]
            col_start = child.start_point[1]
            col_end = child.end_point[1]
            identifier_name = src_lines[row][col_start:col_end].strip()
        
            if parent is None:
                continue
                
            if CCG.nodes[parent]['nodeType'] in ['function_definition', 'struct_specifier', 'enum_specifier']:
                if child.parent.type == 'parameter_declaration':
                    CCG.nodes[parent]['defSet'].add(identifier_name)
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] == 'for_statement':
                p = child
                while p.parent.type != 'for_statement':
                    p = p.parent
                if p.parent.type == 'for_statement' and (
                    p.prev_sibling and p.prev_sibling.type == 'for'
                ):
                    CCG.nodes[parent]['defSet'].add(identifier_name)
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] == 'expression_statement':
                p = child
                while p.parent and p.parent.type not in ['assignment_expression', 'expression_statement']:
                    p = p.parent
                if p.parent and p.parent.type == 'assignment_expression' and p.prev_sibling is None:
                    CCG.nodes[parent]['defSet'].add(identifier_name) 
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] in ['preproc_include', 'preproc_def']:
                CCG.nodes[parent]['defSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] == 'init_declarator':
               CCG.nodes[parent]['defSet'].add(identifier_name)
            else:
                CCG.nodes[parent]['useSet'].add(identifier_name)
        c_control_dependence_graph(child, CCG, src_lines, parent,level+1)

    return

def c_control_flow_graph(CCG):
    CFG = nx.MultiDiGraph()
    
    next_sibling = dict()
    first_children = dict()
    
    # Build sibling relationships
    start_nodes = [v for v in CCG.nodes if len(list(CCG.predecessors(v))) == 0]
    start_nodes.sort()
    
    for i in range(len(start_nodes) - 1):
        next_sibling[start_nodes[i]] = start_nodes[i + 1]
    next_sibling[start_nodes[-1]] = None
    
    # Build parent-child relationships
    for v in CCG.nodes:
        children = list(CCG.neighbors(v))
        if children:
            children.sort()
            for i in range(len(children) - 1):
                if CCG.nodes[v]['nodeType'] == 'switch_statement' and CCG.nodes[children[i+1]]['nodeType'] == 'case_statement':
                    next_sibling[children[i]] = None
                else:
                    next_sibling[children[i]] = children[i + 1]
            next_sibling[children[-1]] = None
            first_children[v] = children[0]
        else:
            first_children[v] = None
    
    edge_list = []
    
    # Build control flow edges
    for v in CCG.nodes:
        # Handle C-specific control flow
        if CCG.nodes[v]['nodeType'] in ['return_statement', 'break_statement', 'continue_statement']:
            if CCG.nodes[v]['nodeType'] == 'break_statement':
                preds = list(CCG.predecessors(v))
                if not preds:  # Empty predecessors list
                    # Look for enclosing control structure based on source line position
                    enclosing_node = None
                    cur_row = CCG.nodes[v]['startRow']
                    
                    # Find the closest enclosing control structure
                    for node in CCG.nodes:
                        if CCG.nodes[node]['nodeType'] in ['for_statement', 'while_statement', 'do_statement', 'switch_statement']:
                            if CCG.nodes[node]['startRow'] <= cur_row and \
                               CCG.nodes[node]['endRow'] >= cur_row:
                                if enclosing_node is None or \
                                   (CCG.nodes[node]['startRow'] > CCG.nodes[enclosing_node]['startRow']):
                                    enclosing_node = node
                    
                    if enclosing_node and next_sibling[enclosing_node]:
                        edge_list.append((v, next_sibling[enclosing_node], 'CFG'))
                    continue

                # Original logic for when predecessors exist
                p = preds[0]
                
                # Handle break in case statement
                if CCG.nodes[p]['nodeType'] == 'case_statement':
                    # Find the enclosing switch statement
                    switch_found = False
                    for node in CCG.nodes:
                        if CCG.nodes[node]['nodeType'] == 'switch_statement' and \
                           CCG.nodes[node]['startRow'] <= CCG.nodes[p]['startRow'] and \
                           CCG.nodes[node]['endRow'] >= CCG.nodes[p]['endRow']:
                            if next_sibling[node]:
                                edge_list.append((v, next_sibling[node], 'CFG'))
                            switch_found = True
                            break
                    if not switch_found:
                        continue
                else:
                    # Original logic for other break statements
                    while CCG.nodes[p]['nodeType'] not in ['for_statement', 'while_statement', 'do_statement', 'switch_statement']:
                        if len(list(CCG.predecessors(p))) == 0:
                            break
                        p = list(CCG.predecessors(p))[0]
                    if next_sibling[p]:
                        edge_list.append((v, next_sibling[p], 'CFG'))
            elif CCG.nodes[v]['nodeType'] == 'continue_statement':
                p = list(CCG.predecessors(v))[0]
                while CCG.nodes[p]['nodeType'] not in ['for_statement', 'while_statement', 'do_statement']:
                    p = list(CCG.predecessors(p))[0]
                edge_list.append((v, p, 'CFG'))
        elif CCG.nodes[v]['nodeType'] in ['while_statement', 'for_statement', 'do_statement']:
            if first_children[v]:
                edge_list.append((v, first_children[v], 'CFG'))
            if next_sibling[v]:
                edge_list.append((v, next_sibling[v], 'CFG'))
        elif CCG.nodes[v]['nodeType'] in ['if_statement', 'switch_statement']:
            if first_children[v]:
                edge_list.append((v, first_children[v], 'CFG'))
            for u in CCG.neighbors(v):
                if CCG.nodes[u]['nodeType'] in ['else_clause', 'case_statement']:
                    edge_list.append((v, u, 'CFG'))
        
        # Handle loop back edges
        u = next_sibling[v]
        if u is None:
            p = v
            while list(CCG.predecessors(p)):
                p = list(CCG.predecessors(p))[0]
                if CCG.nodes[p]['nodeType'] in ['while_statement', 'for_statement', 'do_statement']:
                    edge_list.append((v, p, 'CFG'))
                    break
                if CCG.nodes[p]['nodeType'] in ['if_statement', 'switch_statement']:
                    if next_sibling[p]:
                        edge_list.append((v, next_sibling[p], 'CFG'))
                        break
        if u is not None:
            edge_list.append((v, u, 'CFG'))
            
    CFG.add_edges_from(edge_list)
    for v in CCG.nodes:
        if v not in CFG.nodes:
            CFG.add_node(v)
            
    return CFG, edge_list

def c_data_dependence_graph(CFG, CCG,max_path_length=10):
    for v in CCG.nodes:
        for u in CCG.nodes:
            if v == u or CCG.nodes[v]['nodeType'] in ['preproc_include', 'preproc_def']:
                continue
                
            # Find definition scope for u
            u_def = u
            u_def_set = set()
            while list(CCG.predecessors(u_def)):
                u_def = list(CCG.predecessors(u_def))[0]
                if CCG.nodes[u_def]['nodeType'] in ['function_definition', 'struct_specifier', 'enum_specifier']:
                    u_def_set.add(u_def)
                    
            # Skip if v is a definition not in u's scope
            if CCG.nodes[v]['nodeType'] in ['function_definition', 'struct_specifier', 'enum_specifier'] and v not in u_def_set:
                continue
                
            # Check for data dependencies
            if CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet'] and nx.has_path(CFG, v, u):
                has_path = False
                try:
                    # Use cutoff parameter to limit path length
                    paths = list(nx.all_simple_paths(CFG, source=v, target=u, cutoff=max_path_length))
                    variables = CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet']
                    
                    for var in variables:
                        for path in paths:
                            if not any(var in CCG.nodes[p]['defSet'] for p in path[1:-1]):
                                has_path = True
                                CCG.add_edge(v, u, 'DDG')
                                break
                        if has_path:
                            break
                except nx.NetworkXError:
                    # Handle any path-finding errors
                    continue
def create_graph(code_lines, repo_name):
    """
    Creates a program dependence graph for C code.
    
    Args:
        code_lines: List of strings containing the source code
        repo_name: Name of the repository (used for language identification)
        
    Returns:
        nx.MultiDiGraph: Combined control and data dependence graph, or None if parsing fails
    """
    # Set up tree-sitter parser
    C_LANGUAGE = Language(tsc.language())
    parser = Parser(C_LANGUAGE)

    # Handle input code
    src_lines = "".join(code_lines).encode('ascii', errors='ignore').decode('ascii')
    src_lines = src_lines.splitlines(keepends=True)
    if len(src_lines) != 0:
        src_lines[-1] = src_lines[-1].rstrip().strip('(').strip('[').strip(',')

    comment_prefix = "//"
    if len(src_lines) == 0:
        return None

    comment_lines = []

    for i in range(0, len(src_lines)):
            line = src_lines[i]
            if line.lstrip().startswith(comment_prefix):
                src_lines[i] = '\n'
                comment_lines.append(i)

    # Parse the code
    def read_callable(byte_offset, point):
        row, column = point
        if row >= len(src_lines) or column >= len(src_lines[row]):
            return None
        return src_lines[row][column:].encode('utf8', errors='ignore')
    
    tree = parser.parse(read_callable,encoding="utf8")

    # Check if file contains only comments
    all_comment = True
    if tree.root_node.type == "translation_unit":
        for child in tree.root_node.children:
            if child.type not in ['comment']:
                all_comment = False
                break
        if all_comment:
            return None
    else:
        for child in tree.root_node.children[0].children:
            if child.type not in ['comment']:
                all_comment = False
        if all_comment:
            return None

    # Initialize program dependence graph
    ccg = nx.MultiDiGraph()

    # Construct control dependence edge

    for child in tree.root_node.children:
        c_control_dependence_graph(child, ccg, src_lines, None)

    # Construct control flow graph
    cfg, cfg_edge_list = c_control_flow_graph(ccg)

    # Construct data dependence graph
    c_data_dependence_graph(cfg, ccg)

    # Add control flow edges to the final graph
    ccg.add_edges_from(cfg_edge_list)

    # Handle comments in the graph
    # node_list = list(ccg.nodes)
    # node_list.sort()
    # comment_lines.reverse()
    # max_comment_line = 0
    
    # # Associate comments with their relevant nodes
    # for comment_line_num in comment_lines:
    #     insert_id = -1
    #     for v in ccg.nodes:
    #         if ccg.nodes[v]['startRow'] > comment_line_num:
    #             insert_id = v
    #             break
                
    #     if insert_id == -1:
    #         max_comment_line = max(max_comment_line, comment_line_num)
    #     else:
    #         ccg.nodes[insert_id]['startRow'] = comment_line_num
    #         end_row = ccg.nodes[insert_id]['endRow']
    #         ccg.nodes[insert_id]['sourceLines'] = code_lines[comment_line_num: end_row + 1]
            
    # # Handle trailing comments
    # if max_comment_line != 0:
    #     last_node_id = node_list[-1]
    #     ccg.nodes[last_node_id]['endRow'] = max_comment_line
    #     start_row = ccg.nodes[last_node_id]['startRow']
    #     ccg.nodes[last_node_id]['sourceLines'] = code_lines[start_row: max_comment_line + 1]

    return ccg