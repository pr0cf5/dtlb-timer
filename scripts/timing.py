#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import struct
import sys

def read_result(fname):
    with open(fname, "rb") as f:
        data = f.read()
    data_len = len(data)
    array_size = data_len // 8
    hits = np.zeros(array_size // 2)
    misses = np.zeros(array_size // 2)
    ofs = 0
    for i in range(array_size // 2):
        misses[i] = struct.unpack("<Q", data[ofs:ofs+8])[0]
        ofs += 8
    for i in range(array_size // 2):
        hits[i] = struct.unpack("<Q", data[ofs:ofs+8])[0]
        ofs += 8
    return (misses,hits)

if __name__ == "__main__":
    misses, hits = read_result(sys.argv[1])
    xx = np.linspace(0, 300, 300)
    plt.figure(1)
    plt.hist(misses, bins=xx, color='red')
    plt.savefig("TLB-timing-misses.png")

    plt.figure(2)
    plt.hist(hits, bins=xx, color='green')
    plt.savefig("TLB-timing-hits.png")

    print("[*] Miss: {}".format(np.average(misses)))
    print("[*] Hit: {}".format(np.average(hits)))