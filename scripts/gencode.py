#!/usr/bin/python3
import sys,os

if __name__ == "__main__":
    init_without_x = [5, 6, 7, 8, 0, 1, 2, 3]
    init_with_x = [5, 6, 7, 8, 0, 1, 2, 3, 4]
    #3, 1, 0, 2
    seq = [0, 3, 0, 1, 0, 2, 0, 3, 0, 1, 0, 2]
    template_without_x = """
static uint32_t run_without_x(char **pages) {
    register uint32_t trash;
    trash = 0;
%s
    return trash;
}
    """
    template_with_x = """
static uint32_t run_with_x(char **pages) {
    register uint32_t trash;
    trash = 0;
%s
    return trash;
}
    """
    code_with_x = ""
    for x in init_with_x:
        code_with_x += "    trash = pages[{}][trash];\n".format(x)
    for x in seq:
        code_with_x += "    trash = pages[{}][trash];\n".format(x)
    print(template_with_x%code_with_x)

    code_without_x = ""
    for x in init_without_x:
        code_without_x += "    trash = pages[{}][trash];\n".format(x)
    for x in seq:
        code_without_x += "    trash = pages[{}][trash];\n".format(x)
    print(template_without_x%code_without_x)

