import networkx as nx
from tree_sitter import Language, Parser
from utils.utils import CONSTANTS, merge_docstrings,convert_docs_to_comments


def python_control_dependence_graph(root_node, CCG, src_lines, parent):
    node_id = len(CCG.nodes)

    if root_node.type in ['import_from_statement', 'import_statement']:
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
    elif root_node.type in ['class_definition', 'decorated_definition', 'function_definition']:
        if root_node.type == 'function_definition':
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('parameters').end_point[0]
        elif root_node.type == 'decorated_definition':
            def_node = root_node.child_by_field_name('definition')
            start_row = root_node.start_point[0]
            parameter_node = def_node.child_by_field_name('parameters')
            if parameter_node is not None:
                end_row = parameter_node.end_point[0]
            else:
                end_row = def_node.start_point[0]
        elif root_node.type == 'class_definition':
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('name').end_point[0]
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
    elif root_node.type in ['while_statement', 'for_statement']:
        if root_node.type == 'for_statement':
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('right').end_point[0]
        if root_node.type == 'while_statement':
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('condition').end_point[0]
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
    elif root_node.type == 'if_statement':
        start_row = root_node.start_point[0]
        end_row = root_node.child_by_field_name('condition').end_point[0]
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
    elif root_node.type == 'elif_clause':
        start_row = root_node.start_point[0]
        end_row = root_node.child_by_field_name('condition').end_point[0]
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
    elif root_node.type in ['else_clause', 'except_clause']:
        start_row = root_node.start_point[0]
        end_row = root_node.start_point[0]
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
    elif root_node.type == 'with_statement':
        start_row = root_node.start_point[0]
        end_row = root_node.children[1].end_point[0]
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
    elif 'statement' in root_node.type or 'ERROR' in root_node.type:
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

    for child in root_node.children:
        if child.type == 'identifier':
            row = child.start_point[0]
            col_start = child.start_point[1]
            col_end = child.end_point[1]
            identifier_name = src_lines[row][col_start:col_end].strip()
            if parent is None:
                continue
            if 'definition' in CCG.nodes[parent]['nodeType']:
                CCG.nodes[parent]['defSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] == 'for_statement':
                p = child
                while p.parent.type != 'for_statement':
                    p = p.parent
                if p.parent.type == 'for_statement' and p.prev_sibling.type == 'for':
                    CCG.nodes[parent]['defSet'].add(identifier_name)
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] == 'with_statement':
                if child.parent.type == 'as_pattern_target':
                    CCG.nodes[parent]['defSet'].add(identifier_name)
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] == 'expression_statement':
                p = child
                while p.parent.type != 'assignment' and p.parent.type != 'expression_statement':
                    p = p.parent
                if p.parent.type == 'assignment' and p.next_sibling is not None:
                    CCG.nodes[parent]['defSet'].add(identifier_name)
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif 'import' in CCG.nodes[parent]['nodeType']:
                CCG.nodes[parent]['defSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] in ['global_statement', 'nonlocal_statement']:
                CCG.nodes[parent]['defSet'].add(identifier_name)
            else:
                CCG.nodes[parent]['useSet'].add(identifier_name)
        python_control_dependence_graph(child, CCG, src_lines, parent)

    return


def python_control_flow_graph(CCG):
    CFG = nx.MultiDiGraph()

    next_sibling = dict()
    first_children = dict()

    start_nodes = []
    for v in CCG.nodes:
        if len(list(CCG.predecessors(v))) == 0:
            start_nodes.append(v)
    start_nodes.sort()
    for i in range(0, len(start_nodes) - 1):
        v = start_nodes[i]
        u = start_nodes[i + 1]
        next_sibling[v] = u
    next_sibling[start_nodes[-1]] = None

    for v in CCG.nodes:
        children = list(CCG.neighbors(v))
        if len(children) != 0:
            children.sort()
            for i in range(0, len(children) - 1):
                u = children[i]
                w = children[i + 1]
                if CCG.nodes[v]['nodeType'] == 'if_statement' and 'clause' in CCG.nodes[w]['nodeType']:
                    next_sibling[u] = None
                else:
                    next_sibling[u] = w
            next_sibling[children[-1]] = None
            first_children[v] = children[0]
        else:
            first_children[v] = None

    edge_list = []

    for v in CCG.nodes:
        # block start control flow
        if v in first_children.keys():
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        # block end control flow
        if CCG.nodes[v]['nodeType'] in ['return_statement', 'raise_statement']:
            pass
        elif CCG.nodes[v]['nodeType'] in ['break_statement', 'continue_statement']:
            u = None
            p = list(CCG.predecessors(v))[0]
            while CCG.nodes[p]['nodeType'] not in ['for_statement', 'while_statement']:
                p = list(CCG.predecessors(p))[0]
            if CCG.nodes[v]['nodeType'] == 'break_statement':
                u = next_sibling[p]
            if CCG.nodes[v]['nodeType'] == 'continue_statement':
                u = p
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        elif CCG.nodes[v]['nodeType'] == 'for_statement':
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        elif CCG.nodes[v]['nodeType'] == 'while_statement':
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            u = next_sibling[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        elif CCG.nodes[v]['nodeType'] in ['if_statement' or 'try_statement']:
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            for u in CCG.neighbors(v):
                if 'clause' in CCG.nodes[u]['nodeType']:
                    edge_list.append((v, u, 'CFG'))
        elif 'clause' in CCG.nodes[v]['nodeType']:
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))

        u = next_sibling[v]
        if u is None:
            p = v
            while len(list(CCG.predecessors(p))) != 0:
                p = list(CCG.predecessors(p))[0]
                if CCG.nodes[p]['nodeType'] == 'while_statement':
                        edge_list.append((v, p, 'CFG'))
                        break
                if CCG.nodes[p]['nodeType'] == 'for_statement':
                    edge_list.append((v, p, 'CFG'))
                    break
                if CCG.nodes[p]['nodeType'] in ['try_statement', 'if_statement']:
                    if next_sibling[p] is not None:
                        edge_list.append((v, next_sibling[p], 'CFG'))
                        break
        if u is not None:
            edge_list.append((v, u, 'CFG'))
    CFG.add_edges_from(edge_list)
    for v in CCG.nodes:
        if v not in CFG.nodes:
            CFG.add_node(v)
    return CFG, edge_list


def python_data_dependence_graph(CFG, CCG):
    for v in CCG.nodes:
        for u in CCG.nodes:
            if v == u or 'import' in CCG.nodes[v]['nodeType']:
                continue
            # find the definition of u
            u_def = u
            u_def_set = set()
            while len(list(CCG.predecessors(u_def))) != 0:
                u_def = list(CCG.predecessors(u_def))[0]
                if 'definition' in CCG.nodes[u_def]['nodeType']:
                    u_def_set.add(u_def)
            if 'definition' in CCG.nodes[v]['nodeType'] and v not in u_def_set:
                continue
            if len(CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet']) != 0 and nx.has_path(CFG, v, u):
                has_path = False
                paths = list(nx.all_shortest_paths(CFG, source=v, target=u))
                variables = CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet']
                for var in variables:
                    has_def = False
                    for path in paths:
                        for p in path[1:-1]:
                            if var in CCG.nodes[p]['defSet']:
                                has_def = True
                                break
                        if not has_def:
                            has_path = True
                            break
                    if has_path:
                        break
                if has_path:
                    CCG.add_edge(v, u, 'DDG')
    return


def java_control_dependence_graph(root_node, CCG, src_lines, parent):
    node_id = len(CCG.nodes)

    if root_node.type == 'import_declaration':
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
    elif root_node.type in ['class_declaration', 'method_declaration', 'enum_declaration', 'interface_declaration']:
        if root_node.type == 'method_declaration':
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('parameters').end_point[0]
        elif root_node.type in ['class_declaration', 'enum_declaration', 'interface_declaration']:
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('name').end_point[0]
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
    elif root_node.type in ['while_statement', 'for_statement']:
        if root_node.type == 'for_statement':
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('right').end_point[0]
        if root_node.type == 'while_statement':
            start_row = root_node.start_point[0]
            end_row = root_node.child_by_field_name('condition').end_point[0]
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
    elif root_node.type == 'if_statement':
        start_row = root_node.start_point[0]
        end_row = root_node.child_by_field_name('condition').end_point[0]
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
    elif root_node.type in ['else', 'except_clause', 'catch_clause', 'finally_clause']:
        start_row = root_node.start_point[0]
        end_row = root_node.start_point[0]
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
    elif 'statement' in root_node.type or 'ERROR' in root_node.type:
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

    for child in root_node.children:
        if child.type == 'identifier':
            row = child.start_point[0]
            col_start = child.start_point[1]
            col_end = child.end_point[1]
            identifier_name = src_lines[row][col_start:col_end].strip()
            if parent is None:
                continue
            if 'definition' in CCG.nodes[parent]['nodeType']:
                CCG.nodes[parent]['defSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] == 'for_statement':
                p = child
                while p.parent.type != 'for_statement':
                    p = p.parent
                if p.parent.type == 'for_statement' and p.prev_sibling.type == 'for':
                    CCG.nodes[parent]['defSet'].add(identifier_name)
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif CCG.nodes[parent]['nodeType'] in ['assignment_expression', 'local_variable_declaration']:
                if child.next_sibling is not None:
                    CCG.nodes[parent]['defSet'].add(identifier_name)
                else:
                    CCG.nodes[parent]['useSet'].add(identifier_name)
            elif 'import' in CCG.nodes[parent]['nodeType']:
                CCG.nodes[parent]['defSet'].add(identifier_name)
            else:
                CCG.nodes[parent]['useSet'].add(identifier_name)
        java_control_dependence_graph(child, CCG, src_lines, parent)

    return


def java_control_flow_graph(CCG):
    CFG = nx.MultiDiGraph()

    next_sibling = dict()
    first_children = dict()

    start_nodes = []
    for v in CCG.nodes:
        if len(list(CCG.predecessors(v))) == 0:
            start_nodes.append(v)
    start_nodes.sort()
    for i in range(0, len(start_nodes) - 1):
        v = start_nodes[i]
        u = start_nodes[i + 1]
        next_sibling[v] = u
    next_sibling[start_nodes[-1]] = None

    for v in CCG.nodes:
        children = list(CCG.neighbors(v))
        if len(children) != 0:
            children.sort()
            for i in range(0, len(children) - 1):
                u = children[i]
                w = children[i + 1]
                if CCG.nodes[v]['nodeType'] == 'if_statement' and 'clause' in CCG.nodes[w]['nodeType']:
                    next_sibling[u] = None
                else:
                    next_sibling[u] = w
            next_sibling[children[-1]] = None
            first_children[v] = children[0]
        else:
            first_children[v] = None

    edge_list = []

    for v in CCG.nodes:
        # block start control flow
        if v in first_children.keys():
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        # block end control flow
        if CCG.nodes[v]['nodeType'] == 'return_statement':
            pass
        elif CCG.nodes[v]['nodeType'] in ['break_statement', 'continue_statement']:
            u = None
            p = list(CCG.predecessors(v))[0]
            while CCG.nodes[p]['nodeType'] not in ['for_statement', 'while_statement']:
                p = list(CCG.predecessors(p))[0]
            if CCG.nodes[v]['nodeType'] == 'break_statement':
                u = next_sibling[p]
            if CCG.nodes[v]['nodeType'] == 'continue_statement':
                u = p
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        elif CCG.nodes[v]['nodeType'] == 'for_statement':
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        elif CCG.nodes[v]['nodeType'] == 'while_statement':
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            u = next_sibling[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
        elif CCG.nodes[v]['nodeType'] in ['if_statement' or 'try_statement']:
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            for u in CCG.neighbors(v):
                if 'clause' in CCG.nodes[u]['nodeType']:
                    edge_list.append((v, u, 'CFG'))
        elif 'clause' in CCG.nodes[v]['nodeType']:
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))

        u = next_sibling[v]
        if u is None:
            p = v
            while len(list(CCG.predecessors(p))) != 0:
                p = list(CCG.predecessors(p))[0]
                if CCG.nodes[p]['nodeType'] == 'while_statement':
                        edge_list.append((v, p, 'CFG'))
                        break
                if CCG.nodes[p]['nodeType'] == 'for_statement':
                    edge_list.append((v, p, 'CFG'))
                    break
                if CCG.nodes[p]['nodeType'] in ['try_statement', 'if_statement']:
                    if next_sibling[p] is not None:
                        edge_list.append((v, next_sibling[p], 'CFG'))
                        break
        if u is not None:
            edge_list.append((v, u, 'CFG'))
    CFG.add_edges_from(edge_list)
    for v in CCG.nodes:
        if v not in CFG.nodes:
            CFG.add_node(v)
    return CFG, edge_list


def java_data_dependence_graph(CFG, CCG):
    for v in CCG.nodes:
        for u in CCG.nodes:
            if v == u or 'import' in CCG.nodes[v]['nodeType']:
                continue
            # find the definition of u
            u_def = u
            u_def_set = set()
            while len(list(CCG.predecessors(u_def))) != 0:
                u_def = list(CCG.predecessors(u_def))[0]
                if 'declaration' in CCG.nodes[u_def]['nodeType']:
                    u_def_set.add(u_def)
            if 'declaration' in CCG.nodes[v]['nodeType'] and v not in u_def_set:
                continue
            if len(CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet']) != 0 and nx.has_path(CFG, v, u):
                has_path = False
                paths = list(nx.all_shortest_paths(CFG, source=v, target=u))
                variables = CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet']
                for var in variables:
                    has_def = False
                    for path in paths:
                        for p in path[1:-1]:
                            if var in CCG.nodes[p]['defSet']:
                                has_def = True
                                break
                        if not has_def:
                            has_path = True
                            break
                    if has_path:
                        break
                if has_path:
                    CCG.add_edge(v, u, 'DDG')
    return

def c_control_dependence_graph(root_node, CCG, src_lines, parent):
    node_id = len(CCG.nodes)
    if root_node.type in ['preproc_include']:
        start_row = root_node.start_point[0]
        end_row = root_node.end_point[0]
        
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row],
                        # sourceLines=root_node.text.decode(),
                        defSet=set(),
                        useSet=set())
            parent = node_id
        else:
            if CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row:
                pass
            else:
                CCG.add_node(node_id, nodeType=root_node.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row],
                            # sourceLines=root_node.text.decode(),
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id
    if root_node.type in ['preproc_include', 'preproc_ifdef', 'preproc_ifndef', 'preproc_def', 'preproc_endif', 'declaration', '#ifdef', '#ifndef']:
        start_row = root_node.start_point[0]
        end_row = root_node.end_point[0]
        
        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        # sourceLines=root_node.text.decode(),
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
                            # sourceLines=root_node.text.decode(),
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

    elif root_node.type in ['function_definition', 'struct_specifier', 'enum_specifier']:
        start_row = root_node.start_point[0]
        if root_node.type == 'function_definition':
            declarator = root_node.child_by_field_name('declarator')
            if declarator is not None:
                end_row = declarator.end_point[0]
            else:
                end_row = start_row
        else:
            # For struct and enum, try to get type_identifier
            type_id = root_node.child_by_field_name('type_identifier')
            if type_id is not None:
                end_row = type_id.end_point[0]
            else:
                # If no name/type_identifier is found, use the start_row
                end_row = start_row

        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        # sourceLines=root_node.text.decode(),
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
                            # sourceLines=root_node.text.decode(),
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

    elif root_node.type in ['while_statement', 'for_statement', 'do_statement']:
        start_row = root_node.start_point[0]
        if root_node.type == 'for_statement':
            end_row = root_node.child_by_field_name('condition').end_point[0]
        elif root_node.type == 'while_statement':
            end_row = root_node.child_by_field_name('condition').end_point[0]
        else:  # do_statement
            end_row = root_node.start_point[0]

        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        # sourceLines=root_node.text.decode(),
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
                            # sourceLines=root_node.text.decode(),
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

    elif root_node.type in ['if_statement', 'else_clause', 'switch_statement']:
        start_row = root_node.start_point[0]
        if root_node.type != 'else_clause':
            end_row = root_node.child_by_field_name('condition').end_point[0]
        else:
            end_row = root_node.start_point[0]

        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        # sourceLines=root_node.text.decode(),
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
                            # sourceLines=root_node.text.decode(),
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id
    elif root_node.type == 'case_statement':
        start_row = root_node.start_point[0]
        # Change 'value' to 'expression'
        case_expr = root_node.child_by_field_name('expression')
        if case_expr is not None:
            end_row = case_expr.end_point[0]
        else:
            # Fallback to using the start row if we can't find the expression
            end_row = start_row

        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        # sourceLines=root_node.text.decode(),
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
                            # sourceLines=root_node.text.decode(),
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

    # elif root_node.type in ['compound_statement'] or 'statement' in root_node.type or 'declaration' in root_node.type or 'ERROR' in root_node.type:
    elif 'statement' in root_node.type or 'declaration' in root_node.type or 'ERROR' in root_node.type:
        start_row = root_node.start_point[0]
        end_row = root_node.end_point[0]

        if parent is None:
            CCG.add_node(node_id, nodeType=root_node.type,
                        startRow=start_row, endRow=end_row,
                        sourceLines=src_lines[start_row:end_row + 1],
                        # sourceLines=root_node.text.decode(),
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
                            # sourceLines=root_node.text.decode(),
                            defSet=set(),
                            useSet=set())
                CCG.add_edge(parent, node_id, 'CDG')
                parent = node_id

    for child in root_node.children:
        if child.type == 'identifier':
            row = child.start_point[0]
            col_start = child.start_point[1]
            col_end = child.end_point[1]
            identifier_name = src_lines[row][col_start:col_end].strip()
            if parent is None:
                continue
            if root_node.type == 'function_definition' and child.parent.type == 'function_declarator':
                CCG.nodes[parent]['defSet'].add(identifier_name)
            elif root_node.type in ['struct_specifier', 'enum_specifier'] and child.parent.type == 'type_identifier':
                CCG.nodes[parent]['defSet'].add(identifier_name)
            elif child.parent.type == 'declaration':
                CCG.nodes[parent]['defSet'].add(identifier_name)
            elif child.parent.type == 'assignment_expression' and child.next_sibling is not None:
                CCG.nodes[parent]['defSet'].add(identifier_name)
            else:
                CCG.nodes[parent]['useSet'].add(identifier_name)
        elif child.type == 'compound_statement':
            # Handle compound statement (block of code in curly braces)
            start_row = child.start_point[0]
            end_row = child.end_point[0]
            
            # Create a new node for the compound statement
            if parent is None:
                CCG.add_node(node_id, nodeType=child.type,
                            startRow=start_row, endRow=end_row,
                            sourceLines=src_lines[start_row:end_row + 1],
                            defSet=set(),
                            useSet=set())
                parent = node_id
            else:
                # Only add a new node if this compound statement isn't already covered by parent
                if CCG.nodes[parent]['startRow'] <= start_row and CCG.nodes[parent]['endRow'] >= end_row:
                    pass
                else:
                    CCG.add_node(node_id, nodeType=child.type,
                                startRow=start_row, endRow=end_row,
                                sourceLines=src_lines[start_row:end_row + 1],
                                defSet=set(),
                                useSet=set())
                    CCG.add_edge(parent, node_id, 'CDG')
                    
                    # Update parent to this compound statement for its children
                    parent = node_id
            
            # Process all statements within the compound statement
           
        c_control_dependence_graph(child, CCG, src_lines, parent)

    return

def c_control_flow_graph(CCG):
    CFG = nx.MultiDiGraph()
    
    next_sibling = dict()
    first_children = dict()
    
    # Find start nodes
    start_nodes = []
    for v in CCG.nodes:
        if len(list(CCG.predecessors(v))) == 0:
            start_nodes.append(v)
    start_nodes.sort()
    
    # Set up sibling relationships for start nodes
    for i in range(0, len(start_nodes) - 1):
        v = start_nodes[i]
        u = start_nodes[i + 1]
        next_sibling[v] = u
    next_sibling[start_nodes[-1]] = None

    # Process child relationships
    for v in CCG.nodes:
        children = list(CCG.neighbors(v))
        if len(children) != 0:
            children.sort()
            for i in range(0, len(children) - 1):
                u = children[i]
                w = children[i + 1]
                # Handle C-specific control structures
                if CCG.nodes[v]['nodeType'] in ['if_statement', 'switch_statement'] and \
                   CCG.nodes[w]['nodeType'] in ['case_label', 'default_label', 'else_clause']:
                    next_sibling[u] = None
                else:
                    next_sibling[u] = w
            next_sibling[children[-1]] = None
            first_children[v] = children[0]
        else:
            first_children[v] = None

    edge_list = []

    for v in CCG.nodes:
        # Block start control flow
        if v in first_children.keys():
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
                
        # Handle C-specific control structures
        if CCG.nodes[v]['nodeType'] == 'return_statement':
            pass
        elif CCG.nodes[v]['nodeType'] == 'break_statement':
            u = None
            p = list(CCG.predecessors(v))[0]
            # Look for enclosing loop or switch
            while CCG.nodes[p]['nodeType'] not in ['for_statement', 'while_statement', 
                                                  'do_while_statement', 'switch_statement']:
                p = list(CCG.predecessors(p))[0]
            u = next_sibling[p]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
                
        elif CCG.nodes[v]['nodeType'] == 'continue_statement':
            p = list(CCG.predecessors(v))[0]
            # Look for enclosing loop
            while CCG.nodes[p]['nodeType'] not in ['for_statement', 'while_statement', 
                                                  'do_while_statement']:
                p = list(CCG.predecessors(p))[0]
            edge_list.append((v, p, 'CFG'))
            
        elif CCG.nodes[v]['nodeType'] == 'goto_statement':
            # Find corresponding label
            label_name = CCG.nodes[v]['label']
            for node in CCG.nodes:
                if CCG.nodes[node]['nodeType'] == 'label' and \
                   CCG.nodes[node]['name'] == label_name:
                    edge_list.append((v, node, 'CFG'))
                    break
                    
        elif CCG.nodes[v]['nodeType'] in ['for_statement', 'while_statement']:
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            u = next_sibling[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
                
        elif CCG.nodes[v]['nodeType'] == 'do_while_statement':
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            # Add edge back to start of loop
            edge_list.append((v, first_children[v], 'CFG'))
            
        elif CCG.nodes[v]['nodeType'] == 'switch_statement':
            # Connect to first case
            for u in CCG.neighbors(v):
                if CCG.nodes[u]['nodeType'] in ['case_label', 'default_label']:
                    edge_list.append((v, u, 'CFG'))
                    
        elif CCG.nodes[v]['nodeType'] in ['case_label', 'default_label']:
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            # Fall through to next case if no break
            if next_sibling[v] is not None:
                edge_list.append((v, next_sibling[v], 'CFG'))
                
        elif CCG.nodes[v]['nodeType'] == 'if_statement':
            # Connect to then clause
            u = first_children[v]
            if u is not None:
                edge_list.append((v, u, 'CFG'))
            # Connect to else clause if it exists
            for u in CCG.neighbors(v):
                if CCG.nodes[u]['nodeType'] == 'else_clause':
                    edge_list.append((v, u, 'CFG'))

        # Handle normal control flow
        u = next_sibling[v]
        if u is None:
            p = v
            while len(list(CCG.predecessors(p))) != 0:
                p = list(CCG.predecessors(p))[0]
                if CCG.nodes[p]['nodeType'] in ['while_statement', 'for_statement', 
                                              'do_while_statement']:
                    edge_list.append((v, p, 'CFG'))
                    break
                if CCG.nodes[p]['nodeType'] in ['if_statement', 'switch_statement']:
                    if next_sibling[p] is not None:
                        edge_list.append((v, next_sibling[p], 'CFG'))
                        break
        if u is not None:
            edge_list.append((v, u, 'CFG'))

    # Add all edges to the graph
    CFG.add_edges_from(edge_list)
    
    # Add any isolated nodes
    for v in CCG.nodes:
        if v not in CFG.nodes:
            CFG.add_node(v)
            
    return CFG, edge_list

def c_data_dependence_graph(CFG, CCG):
    for v in CCG.nodes:
        for u in CCG.nodes:
            # Skip if same node or if v is a declaration/include statement
            if v == u or 'declaration' in CCG.nodes[v]['nodeType'] or 'include' in CCG.nodes[v]['nodeType']:
                continue
            
            # Find all definitions of u
            u_def = u
            u_def_set = set()
            while len(list(CCG.predecessors(u_def))) != 0:
                u_def = list(CCG.predecessors(u_def))[0]
                if 'definition' in CCG.nodes[u_def]['nodeType']:
                    u_def_set.add(u_def)
            
            # Skip if v is a definition but not in u's definition set
            if 'definition' in CCG.nodes[v]['nodeType'] and v not in u_def_set:
                continue
            
            # Check for data dependency
            if len(CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet']) != 0 and nx.has_path(CFG, v, u):
                has_path = False
                paths = list(nx.all_shortest_paths(CFG, source=v, target=u))
                variables = CCG.nodes[v]['defSet'] & CCG.nodes[u]['useSet']
                
                # Check each variable in the intersection
                for var in variables:
                    has_def = False
                    # Check each path
                    for path in paths:
                        # Check intermediate nodes
                        for p in path[1:-1]:
                            if var in CCG.nodes[p]['defSet']:
                                has_def = True
                                break
                        if not has_def:
                            has_path = True
                            break
                    if has_path:
                        break
                
                # Add edge if dependency exists
                if has_path:
                    CCG.add_edge(v, u, 'DDG')
    return CCG

def create_graph(code_lines, repo_name):
    # src_lines = code_lines

    src_lines = "".join(code_lines).encode('ascii', errors='ignore').decode('ascii')
    src_lines = src_lines.splitlines(keepends=True)
    # src_lines = convert_docs_to_comments(src_lines,CONSTANTS.repos_language[repo_name])
    if len(src_lines) != 0:
        src_lines[-1] = src_lines[-1].rstrip().strip('(').strip('[').strip(',')
    # Define tree-sitter parser
    Language.build_library('./my-languages.so', ['./tree-sitter-python', './tree-sitter-java','./tree-sitter-c'])
    language = Language('./my-languages.so', CONSTANTS.repos_language[repo_name])
    parser = Parser()
    parser.set_language(language)

    if len(src_lines) == 0:
        return None

    # remove comment
    comment_prefix = ""
    if language.name == "python":
        comment_prefix = "#"
    elif language.name == "java":
        comment_prefix = "//"
    elif language.name == "c":
        comment_prefix = "//"

    comment_lines = []
    for i in range(0, len(src_lines)):
        line = src_lines[i]
        if line.lstrip().startswith(comment_prefix):
            src_lines[i] = '\n'
            comment_lines.append(i)

    # Parser file to get a tree
    def read_callable(byte_offset, point):
        row, column = point
        if row >= len(src_lines) or column >= len(src_lines[row]):
            return None
        return src_lines[row][column:].encode('utf8', errors='ignore')
    tree = parser.parse(read_callable)

    # tree = parser.parse(bytes(code_lines,"utf8"))

    all_comment = True
    if tree.root_node.type=="translation_unit":
        for child in tree.root_node.children:
            if child.type not in 'comment':
                all_comment = False
                break
        if all_comment:
            return None
    else:
        for child in tree.root_node.children[0].children:
            if child.type not in 'comment':
                all_comment = False
        if all_comment:
            return None

    # Initialize program dependence graph
    ccg = nx.MultiDiGraph()

    if language.name == 'python':
        # Construct control dependence edge
        for child in tree.root_node.children:
            python_control_dependence_graph(child, ccg, code_lines, None)

        # Construct control flow graph
        cfg, cfg_edge_list = python_control_flow_graph(ccg)

        # Construct data dependence graph
        python_data_dependence_graph(cfg, ccg)

        ccg.add_edges_from(cfg_edge_list)
    elif language.name == "java":
        # Construct control dependence edge
        for child in tree.root_node.children:
            java_control_dependence_graph(child, ccg, code_lines, None)

        # Construct control flow graph
        cfg, cfg_edge_list = java_control_flow_graph(ccg)

        # Construct data dependence graph
        java_data_dependence_graph(cfg, ccg)

        ccg.add_edges_from(cfg_edge_list)
    elif language.name == "c":
        # Construct control dependence edge
        if tree.root_node.type == 'translation_unit':
            for child in tree.root_node.children:
                if child.type not in 'comment':
                    for grandchild in child.children:
                        c_control_dependence_graph(grandchild, ccg, src_lines, None)
        else:
            for child in tree.root_node.children:
                c_control_dependence_graph(child,ccg,src_lines,None)
        # Construct control flow graph
        cfg, cfg_edge_list = c_control_flow_graph(ccg)

        # Construct data dependence graph
        c_data_dependence_graph(cfg, ccg)

        ccg.add_edges_from(cfg_edge_list)

    # add comment
    node_list = list(ccg.nodes)
    node_list.sort()
    comment_lines.reverse()
    max_comment_line = 0
    for comment_line_num in comment_lines:
        insert_id = -1
        for v in ccg.nodes:
            if ccg.nodes[v]['startRow'] > comment_line_num:
                insert_id = v
                break
        if insert_id == -1:
            max_comment_line = max(max_comment_line, comment_line_num)
        else:
            ccg.nodes[insert_id]['startRow'] = comment_line_num
            end_row = ccg.nodes[insert_id]['endRow']
            ccg.nodes[insert_id]['sourceLines'] = code_lines[comment_line_num: end_row + 1]
    if max_comment_line != 0:
        last_node_id = node_list[-1]
        ccg.nodes[last_node_id]['endRow'] = max_comment_line
        start_row = ccg.nodes[last_node_id]['startRow']
        ccg.nodes[last_node_id]['sourceLines'] = code_lines[start_row: max_comment_line + 1]
    return ccg
