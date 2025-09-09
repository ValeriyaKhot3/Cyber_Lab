#include "util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef MAP_HUGE_SHIFT
#  define MAP_HUGE_SHIFT 26    /* where the size-encoding lives in flags */
#endif
#ifndef MAP_HUGE_MASK
#  define MAP_HUGE_MASK  0x3f
#endif
#ifndef MAP_HUGE_2MB
#  define MAP_HUGE_2MB   (21 << MAP_HUGE_SHIFT)
#endif

//long virt_to_phys (void *virt);

uint64_t get_set ( uint64_t  *virt ){
    return ((uintptr_t)virt >> 6) & ((1u << 10) - 1);
}

int main(int argc, char const *argv[]) {
    int flag = -1;

    // Put your capture-the-flag code here

    //assign 2 MB page
    uint64_t *p = mmap(NULL, 2UL*1024*1024, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_2MB,
                       -1, 0);

    if (p == MAP_FAILED) {
        // HugeTLB failed: print why and fall back to normal mmap (+THP hint)
        perror("mmap MAP_HUGETLB 2MB failed");
        p = mmap(NULL,  2UL*1024*1024, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED) {
            perror("mmap fallback failed");
            return 1;
        }
        // Transparent HugePages hint (best-effort)
        madvise(p,  2UL*1024*1024, MADV_HUGEPAGE);
    }

    volatile char tmp;

    for(int i = 0; i<2UL*1024*128 ; i+=8){
        tmp = p[i];
    }

    int i_max = 0;
    uint64_t max_time = 0;
    uint64_t curr_time = 0;

    for(int i = 0; i<2UL*1024*128 ; i+=8){
        curr_time = measure_one_block_access_time((uint64_t)(p + i));
        if(curr_time > max_time){
            max_time = curr_time;
            i_max = i;
        }
    }

    flag = get_set(p + i_max);


    printf("Flag: %d\n", flag);
    munmap(p, 2UL*1024*1024);
    return 0;
}
