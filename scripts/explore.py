#!/usr/bin/python3
import graphviz
import sys, os
import struct

def read_result(fname):
    with open(fname, "rb") as f:
        data = f.read()
    ofs = 0
    out = []
    data_len = len(data)//64
    for i in range(data_len):
        seq_len = struct.unpack("<B", data[i*64:i*64+1])[0]
        idx_seq = [x for x in data[i*64+1:i*64+1+seq_len]]
        color = data[i*64+63]
        out.append((idx_seq, color))
        ofs += 64
    return out

def save_vgraph(vgraph, file_name):
    vgraph.format = 'svg'
    vgraph.filename = file_name
    vgraph.directory = 'graphs'
    vgraph.render(view=False)

def seqs_to_graph(seqs):
    adjacency_list = {}
    for (seq, color) in seqs:
        src = str(seq[:-1])
        dst = str(seq[:])
        if not src in adjacency_list:
            adjacency_list[src] = [(dst, color)]
        else:
            adjacency_list[src].append((dst, color))
    return adjacency_list
    
def dfs_rec(graph, current_path, good_paths, is_good_func):
    current_pos = current_path[-1][0]
    # we've reached a leaf
    if not current_pos in graph:
        if is_good_func(current_path):
            good_paths.append(current_path[-1][0])
        return
    for (child, color) in graph[current_pos]:
        current_path.append((child, color))
        dfs_rec(graph, current_path, good_paths, is_good_func)
        current_path.pop()

def graph_find_good_paths(graph, begin, cost_thresh):
    def is_good_func(path):
        cost = sum([color for (pos,color) in path])
        return cost <= cost_thresh
    good_paths = []
    current_path = [(begin,0)]
    dfs_rec(graph, current_path, good_paths, is_good_func)
    good_paths = [eval(x) for x in good_paths]
    return good_paths

def write_to_preamble(path, file_name="preamble.txt"):
    buf = b""
    for x in path:
        buf += struct.pack("<B", x)
    with open(file_name, "wb") as f:
        f.write(buf)

if __name__ == "__main__":

    # run without preamble to find initial stack
    stack = []
    os.system("./tlb-explorer")
    seqs = read_result("result.txt")
    begin = str(seqs[0][0][:-1])
    graph = seqs_to_graph(seqs)
    # find all paths that have a cost less than 1*3+2 (we allow 1 miss)
    good_paths = graph_find_good_paths(graph, begin, 1*2+2*2)
    stack += good_paths

    while len(stack) > 0:
        # remove the first 5 elements (5,6,7,8,0,1,2,3,4)
        nextpath = stack.pop()[9:]
        write_to_preamble(nextpath)
        os.system("./tlb-explorer ./preamble.txt")
        seqs = read_result("result.txt")
        begin = str(seqs[0][0][:-1])
        graph = seqs_to_graph(seqs)
        good_paths = graph_find_good_paths(graph, begin, 1*2+2*2)
        # backtrack
        if len(good_paths) == 0:
            continue
        # if a sufficiently long path has been discovered, stop the algorithm
        if len(good_paths[0]) >= 50:
            print(good_paths)
            break
        stack += good_paths
