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

//https://download.01.org/perfmon/

#define PAGES_CNT (TLB_SETS*TLB_WAYS)
#define PGSIZE 0x1000

void *g_pages;

static int pages_init() {
    if ((g_pages = (void *)mmap(NULL, PAGES_CNT * PGSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1 ,0)) == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    // touch the pages 
    memset(g_pages, 0, PAGES_CNT * PGSIZE);
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

static uint64_t measure_tlb_miss(char **ptrs) {
    register char trash;
    register uint64_t ctr;
    register char *a0, *a1, *a2, *a3, *a4;
    a0 = ptrs[0];
    a1 = ptrs[1];
    a2 = ptrs[2];
    a3 = ptrs[3];
    a4 = ptrs[4];
    _mm_lfence();
    // populate the STLB
    trash = 0;
    trash &= a0[trash];
    _mm_lfence();
    trash &= a1[trash];
    _mm_lfence();
    trash &= a2[trash];
    _mm_lfence();
    trash &= a3[trash];
    _mm_lfence();
    trash &= a4[trash];
    _mm_lfence();

    // Start the sequence
    trash &= a0[trash];
    _mm_lfence();
    trash &= a1[trash];
    _mm_lfence();
    trash &= a2[trash];
    _mm_lfence();
    trash &= a3[trash];
    _mm_lfence();
    ctr = rdpmc_begin();
    trash &= a4[trash];
    ctr = rdpmc_end() - ctr;
    return ctr | trash;
}

static void test_tlb_evset() {
    // due to the linear hash function of the L1 dTLB, 
    // base + PGSIZE * TLB_SETS * i, (i = 0,1,2,3,4) should case eviction
    char *ptrs[5];
    uint64_t ctr;
    for (int i = 0; i < 5; i++) {
        ptrs[i] = (char *)g_pages + i * TLB_SETS * PGSIZE;
    }
    ctr = measure_tlb_miss(ptrs);
    printf("%ld\n", ctr);
}

int main(int argc, char **argv) {
    if (pages_init() < 0) {
        exit(-1);
    }
    if (pfc_init() < 0) {
        exit(-1);
    }
    test_tlb_evset();
    return 0;
}