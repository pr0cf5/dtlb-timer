#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <err.h>
#include <stdio.h>
#include <stdbool.h>
#include <emmintrin.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "libpfc.h"
#include "tlb-info.h"
#include "counters.h"
#include "colors.h"

//https://download.01.org/perfmon/

#define PGCNT (TLB_SETS*TLB_WAYS*3)
#define PGSIZE 0x1000
#define CACHE_LINESIZE 0x40
#define HPGSIZE (1 << 22)

#define REPS 1000

struct log_entry {
    uint8_t seq_len;
    uint8_t idx_seq[62];
    uint8_t color;
} __attribute__((packed));

static void *g_pages;
static void *g_hpage;
static size_t recursion_maxdepth = 4;

static int pages_init() {
    if ((g_pages = (void *)mmap(NULL, PGCNT * PGSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1 ,0)) == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    if ((g_hpage = (void *)mmap(NULL, HPGSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON|MAP_HUGETLB, -1, 0)) == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    // touch the pages 
    memset(g_pages, 0, PGCNT * PGSIZE);
    memset(g_hpage, 0, HPGSIZE);
    return 0;
}

static int pfc_init() {
    PFC_CFG cfg;
    int rv;
    // pin thread to core
    if (pfcPinThread(3) != 0) {
        fprintf(stderr, "Failed to pin thread\n");
        return -1;
    }
    // initialize PMC
    if (pfcInit() < 0) {
        fprintf(stderr, "PFC kernel modules are not installed\n");
        return -1;
    }
    // program GP0 to ld1.replacement
    if ((cfg = pfcParseCfg("dtlb_load_misses.stlb_hit")) == 0) {
        fprintf(stderr, "Invalid counter event/umask name\n");
        return -1;
    }
    // GP starts from index 3
    if ((rv = pfcWrCfgs(3, 1, &cfg)) != 0) {
        fprintf(stderr, "Failed to configure counter\n");
        return -1;
    }
    return rv;
}

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
    trash = pages[3][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];
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
    trash = pages[0][trash];
    trash = pages[3][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];

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
    trash = pages[3][trash];
    trash = pages[1][trash];
    trash = pages[0][trash];
    trash = pages[2][trash];
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
    trash = pages[0][trash];
    trash = pages[3][trash];
    trash = pages[0][trash];
    trash = pages[1][trash];

    return trash;
}

int main(int argc, char **argv) {
    uint64_t ctr1, ctr2;
    size_t i;
    char *pages[10];
    if (pages_init() < 0) {
        exit(-1);
    }
    if (pfc_init() < 0) {
        exit(-1);
    }
    for (i = 0; i < 10; i++) {
        pages[i] = (char *)g_pages + (i * PGSIZE * TLB_SETS) + (i * CACHE_LINESIZE % PGSIZE);
    }
    // warmup
    for (i = 0; i < 0x1000; i++) {
        run_with_x(pages);
    }
    ctr1 = rdtsc_begin(); 
    run_without_x(pages);    
    ctr1 = rdtsc_end() - ctr1;
    ctr2 = rdtsc_begin();
    run_with_x(pages);
    ctr2 = rdtsc_end() - ctr2;
    printf("without X: %ld, with X: %ld\n", ctr1, ctr2);
    return 0;
}