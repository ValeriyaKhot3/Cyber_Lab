
#include "util.h"

/* Measure the time it takes to access a block with virtual address addr. */
CYCLES measure_one_block_access_time(ADDR_PTR addr)
{
	CYCLES cycles;

	asm volatile("mov %1, %%r8\n\t"
	"lfence\n\t"
	"rdtsc\n\t"
	"mov %%eax, %%edi\n\t"
	"mov (%%r8), %%r8\n\t"
	"lfence\n\t"
	"rdtsc\n\t"
	"sub %%edi, %%eax\n\t"
	: "=a"(cycles) /*output*/
	: "r"(addr)
	: "r8", "edi");	

	return cycles;
}

/*
 * CLFlushes the given address.
 * 
 * Note: clflush is provided to help you debug and should not be used in your
 * final submission
 */
void clflush(ADDR_PTR addr)
{
    asm volatile ("clflush (%0)"::"r"(addr));
}

// Function to find the mode of the array
int find_mode(int arr[], int n) {
    int mode = arr[0];
    int max_count = 0;
    
    // Loop over each element in the array
    for (int i = 0; i < n; i++) {
        int count = 0;
        // Count occurrences of arr[i]
        for (int j = 0; j < n; j++) {
            if (arr[j] == arr[i]) {
                count++;
            }
        }
        // Update mode if current element's count is higher than max_count
        if (count > max_count) {
            max_count = count;
            mode = arr[i];
        }
    }
    
    return mode;
}
