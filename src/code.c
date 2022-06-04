
static uint32_t run_with_x(char **pages) {
    register uint32_t trash;
    trash = 0;
    trash = pages[5][trash];
    trash = pages[6][trash];
    trash = pages[7][trash];
    trash = pages[8][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];
    trash = pages[2][trash];
    trash = pages[3][trash];
    trash = pages[4][trash];
    trash = pages[0][trash];
    trash = pages[3][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];
    trash = pages[0][trash];
    trash = pages[3][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];

    return trash;
}
    

static uint32_t run_without_x(char **pages) {
    register uint32_t trash;
    trash = 0;
    trash = pages[5][trash];
    trash = pages[6][trash];
    trash = pages[7][trash];
    trash = pages[8][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];
    trash = pages[2][trash];
    trash = pages[3][trash];
    trash = pages[0][trash];
    trash = pages[3][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];
    trash = pages[0][trash];
    trash = pages[3][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];

    return trash;
}
    
