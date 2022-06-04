#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "tlb-info.h"

#define PGCNT (TLB_SETS*TLB_WAYS*3)
#define PGSIZE 0x1000
#define CACHE_LINESIZE 0x40
#define HPGSIZE (1 << 22)

static void *g_pages;

static int pages_init() {
    if ((g_pages = (void *)mmap(NULL, PGCNT * PGSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1 ,0)) == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    // touch the pages 
    memset(g_pages, 0, PGCNT * PGSIZE);
    return 0;
}


static uint32_t run_with_x(char **pages) {
    register uint32_t trash;
    register int i;
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
    trash = pages[3][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];
    for (i = 0; i < 1000; i++) {
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
    }
    return trash;
}
    

static uint32_t run_without_x(char **pages) {
    register uint32_t trash;
    register int i;
    trash = 0;
    trash = pages[5][trash];
    trash = pages[6][trash];
    trash = pages[7][trash];
    trash = pages[8][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];
    trash = pages[2][trash];
    trash = pages[3][trash];
    trash = pages[3][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];
    for (i = 0; i < 1000; i++) {
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
    }
    return trash;
}

int main(int argc, char **argv) {
    size_t i;
    char *pages[10];
    struct timeval tv1, tv2;
    if (pages_init() < 0) {
        exit(-1);
    }
    for (i = 0; i < 10; i++) {
        pages[i] = (char *)g_pages + (i * PGSIZE * TLB_SETS) + (i * CACHE_LINESIZE % PGSIZE);
    }
    // warmup
    for (i = 0; i < 0x1000; i++) {
        run_with_x(pages);
        run_without_x(pages);
    }
    if (gettimeofday(&tv1, NULL) < 0) {
        perror("gettimeofday");
        exit(-1);
    }
    run_with_x(pages);    
    if (gettimeofday(&tv2, NULL) < 0) {
        perror("gettimeofday");
        exit(-1);
    }
    printf("with X: %lds %ldms\n", tv2.tv_sec-tv1.tv_sec, tv2.tv_usec-tv1.tv_usec);
    
    if (gettimeofday(&tv1, NULL) < 0) {
        perror("gettimeofday");
        exit(-1);
    }
    run_without_x(pages);
    if (gettimeofday(&tv2, NULL) < 0) {
        perror("gettimeofday");
        exit(-1);
    }
    printf("without X: %lds %ldms\n", tv2.tv_sec-tv1.tv_sec, tv2.tv_usec-tv1.tv_usec);
    return 0;
}