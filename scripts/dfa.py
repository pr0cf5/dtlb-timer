#!/usr/bin/python3
import graphviz
import sys
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
    nodes = []
    edges = []
    for (seq, color) in seqs:
        src = str(seq[:-1])
        dst = str(seq[:])
        if not src in nodes:
            nodes.append(src)
        edges.append((src,dst,seq[-1],color))
    return (nodes, edges)
    
def graph_to_vgraph(nodes, edges):
    digraph = graphviz.Digraph()
    color_table = {1 : "red", 2 : "black", 255: "grey"}
    for node in nodes:
        digraph.node(node)
    for (src,dst,elem,color) in edges:
        digraph.edge(src,dst,label=str(elem), color=color_table[color])
    return digraph

if __name__ == "__main__":
    seqs = read_result(sys.argv[1])
    nodes,edges = seqs_to_graph(seqs)
    vgraph = graph_to_vgraph(nodes, edges)
    save_vgraph(vgraph, "DFA-3")