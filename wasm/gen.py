#!/usr/bin/python3
import sys,os

PLACEHOLDER = "/*--WASMCODE--*/"

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("[*] usage: ./gen.py <template.js> <code.wasm>")
        exit(-1)

    with open(sys.argv[1], "r") as f:
        template = f.read()
    
    with open(sys.argv[2], "rb") as f:
        wasmBytes = f.read()

    if not PLACEHOLDER in template:
        print("[-] Template does not contain placeholder string")
        exit(-1)
    
    wasmHex = "["
    for x in wasmBytes:
        wasmHex += "0x{:02X},".format(x)
    wasmHex = wasmHex[:-1] + "]"
    template = template.replace(PLACEHOLDER, wasmHex)
    print(template)

