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

static int read_file(const char *file_name, uint8_t **data, size_t *data_len) {
    FILE *fp;
    size_t file_len;
    uint8_t *file_data;
    if ((fp = fopen(file_name, "rb")) == NULL) {
        perror("fopen");
        return -1;
    }
    fseek(fp, 0L, SEEK_SET);
    fseek(fp, 0L, SEEK_END);
    // I need to check if file_len < 0, but who cares anyway
    file_len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    if ((file_data = calloc(1, file_len)) == NULL) {
        perror("calloc");
        return -1;
    }
    if (fread(file_data, 1, file_len, fp) != file_len) {
        return -1;
    }
    *data = file_data;
    *data_len = file_len;
    return 0;
}

// yields the counter difference of the last element load
// make sure no stack variables are used at all, because it may interfere with the TLB
static uint64_t run_seq_single(register char **load_seq, register size_t load_seq_len) {
    register char trash;
    register uint64_t ctr;
    register size_t i;
    register char *ptr;
    ptr = load_seq[load_seq_len-1];
    trash = 0;
    _mm_lfence();

    // it is asserted that load_seq_len is always larger than 0
    for (i = 0; i < load_seq_len - 1; i++) {
        trash &= load_seq[i][trash];
        _mm_lfence();
    }
    ctr = rdpmc_begin();
    trash &= ptr[trash];
    ctr = rdpmc_end() - ctr;
    return ctr | trash;
}

static void test_seq_example() {
    uint8_t idx_seq[] = {5,6,7,8,0, 1, 2, 3, 4, 3, 1, 0, 2, 0, 3, 0, 1, 0, 2, 0, 3, 2, 0, 1, 3, 0, 3, 2, 0, 2, 1, 2, 3, 0, 1, 2, 0, 3, 0, 2, 0, 2, 1, 2, 0, 3, 1, 3, 0, 2, 0, 1, 3, 1, 3, 0};
    char **ptr_seq;
    size_t ptr_seq_len, i, j, cnt;
    double conf;
    ptr_seq = g_hpage;
    ptr_seq_len = sizeof(idx_seq)/sizeof(idx_seq[0]);
    for (i = 0; i < ptr_seq_len; i++) {
        j = idx_seq[i];
        ptr_seq[i] = (char *)g_pages + (j * PGSIZE * TLB_SETS) + (j * CACHE_LINESIZE % PGSIZE);
    }
    cnt = 0;
    for (i = 0; i < REPS; i++) {
        if (run_seq_single(ptr_seq, ptr_seq_len) > 0) {
            cnt++;
        }
    }
    conf = (double)cnt / REPS;
    printf("conf: %lf\n", conf);
}

int explore_rec(uint8_t *idx_current_seq, char **ptr_current_seq, size_t current_seq_len, size_t depth, FILE *log_file) {
    size_t idx, j, trials, acc;
    double confidence;
    uint8_t color;
    struct log_entry le = {0, };

    if (depth >= recursion_maxdepth) {
        return 0;
    }
    for (idx = 0; idx < 4; idx++) {     
        // two consecutive identical element access is considered meaningless (this may be wrong, so I need to check)
        if (idx_current_seq[current_seq_len-1] == idx) {
            continue;
        }
        idx_current_seq[current_seq_len] = idx;
        // this is a trick abusing the fact that the first 4~8 elements are {0,1,2,3}
        ptr_current_seq[current_seq_len] = ptr_current_seq[idx+4];
        acc = 0;
        for (trials = 0; trials < REPS; trials++) {
            if (run_seq_single(ptr_current_seq, current_seq_len+1) > 0) {
                acc++;
            }
        }
        confidence = (double)acc / REPS;
        if (confidence > 0.9) {
            color = COLOR_RED;
        }
        else if (confidence < 0.1) {
            color = COLOR_BLACK;
        }
        else {
            color = COLOR_GREY;
            fprintf(stderr, "[-] Warning: low confidence path detected (conf=%lf)\n", confidence);
        }

        // write the result
        le.color = color;
        le.seq_len = current_seq_len+1;
        memcpy(&le.idx_seq, idx_current_seq, current_seq_len+1);
        if (fwrite(&le, 1, sizeof(le), log_file) != sizeof(le)) {
            perror("fwrite");
            return -1;
        }

        // recursively explore the current path
        if (explore_rec(idx_current_seq, ptr_current_seq, current_seq_len+1, depth+1, log_file) < 0) {
            return -1;
        }
    }
    return 0;
}

// run the algorith that explores the DFA states of the L1 dTLB
int explore(uint8_t *preamble, size_t preamble_len) {
    // baseline sequence: {0,1,2,3,4}
    // access elements 0~3 afterwards
    static uint8_t idx_seq[0x1000];
    uint8_t initial_seq[] = {5,6,7,8,0,1,2,3,4};
    size_t i, j;
    FILE *fp;
    char **ptr_seq;
    size_t len;

    len = sizeof(initial_seq)/sizeof(initial_seq[0]) + preamble_len;
    memcpy(idx_seq, initial_seq, sizeof(initial_seq));
    memcpy(idx_seq + len - preamble_len, preamble, preamble_len * sizeof(preamble[0]));
    ptr_seq = (char **)g_hpage;
    for (i = 0; i < len; i++) {
        j = idx_seq[i];
        ptr_seq[i] = (char *)g_pages + (j * PGSIZE * TLB_SETS) + (j * CACHE_LINESIZE % PGSIZE);
    }
    if ((fp = fopen("result.txt", "wb")) == NULL) {
        perror("fopen");
        return -1;
    }
    if (explore_rec(idx_seq, ptr_seq, len, 0, fp) < 0) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {
    uint8_t *preamble;
    size_t preamble_len;
    if (pages_init() < 0) {
        exit(-1);
    }
    if (pfc_init() < 0) {
        exit(-1);
    }
    
    if (argc < 2) {
        preamble = NULL;
        preamble_len = 0;
    }
    else {
        if (read_file((const char *)argv[1], &preamble, &preamble_len) < 0) {
            fprintf(stderr, "[-] Could not read preamble file\n");
            exit(-1);
        }
    }
    if (explore(preamble, preamble_len) < 0) {
        exit(-1);
    }
    free(preamble);
    return 0;
}