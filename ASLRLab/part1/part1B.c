/*
 * Address Space Layout Randomization
 * Part 1B: Prefetch
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <mmintrin.h>
#include <xmmintrin.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <x86intrin.h>
#include "lab.h"


void prefetch(void* p)
{
  asm volatile ("prefetchnta (%0)" : : "r" (p));
  asm volatile ("prefetcht2 (%0)" : : "r" (p));
}

static inline uint64_t rdtscp() {
    unsigned aux;
    return __rdtscp(&aux);   // reads TSC and serializes
}

static inline uint64_t time_one_prefetch(void *p) {
    // mfence; rdtscp; cpuid; prefetch; cpuid; rdtscp; mfence
    asm volatile ("mfence" ::: "memory");
    uint64_t t1 = rdtscp();
    asm volatile ("cpuid"  ::: "rax","rbx","rcx","rdx","memory");

    prefetch(p);

    asm volatile ("cpuid"  ::: "rax","rbx","rcx","rdx","memory");
    uint64_t t2 = rdtscp();
    asm volatile ("mfence" ::: "memory");
    return t2 - t1;
}



/*
 * Part 1
 * Find and return the single mapped address within the range [low_bound, upper_bound).
 */
uint64_t find_address(uint64_t low_bound, uint64_t high_bound) {

    uint64_t min_time = 1000000;
    uint64_t min_addr = 0;
    for (uint64_t addr = low_bound; addr < high_bound; addr += PAGE_SIZE) {
        // TODO: Figure out if "addr" is the correct address or not.
        uint64_t sum = 0;
        uint64_t time_mean = 0;
        for(int i = 0; i < 10 ; i++){
            sum += time_one_prefetch((void*) addr);
        }

        time_mean = sum/10;
        

        if(time_mean < min_time){
            min_addr = addr;
            min_time = time_mean;
        }

    }

    return min_addr;
}
