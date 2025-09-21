#include "util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>


#define BUFF_SIZE (1<<21)

#define SIGNAL_SIZE 20

static inline uint32_t rdtsc32(void) {
    uint32_t lo;
    // Execute the RDTSC instruction and output only the lower 32 bits (in EAX).
    asm volatile ("rdtsc" : "=a"(lo) : : "edx");
    return lo;
}

int main(int argc, char **argv)
{
    int flag = -1;
	// Put your covert channel setup code here
    int num_l1_set = 128;
    int num_l1_assoc = 8;
    int num_l2_set = 1024;
    int num_l2_assoc = 4;
    int line_size = 64;
    int int64_per_line = line_size / sizeof(uint64_t);
	void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);
	uint64_t *target_buffer = (uint64_t *) buf;
	target_buffer[0] = 0;
    uint64_t* access_time_arr = (uint64_t*)malloc(num_l2_set * sizeof(uint64_t));
	uint64_t access_time = 0;
	uint64_t tmp;
    int arr[SIGNAL_SIZE];




	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	bool listening = true;
    int num_signals = 0;

	while (listening) {
		int flag = -1;
        for(int i = 0; i < 64; i++){
            for(int j = 0; j < num_l2_set * num_l2_assoc; j++){
                tmp = target_buffer[j * 8];
            }
        }

		asm volatile("lfence\n\t");
		// for(int i = 0; i < 1000; i++){
		// 	asm("pause");
		// }

		int64_t baseline = 0;
		for(int j = 0; j < 1024; j++){
            int before = rdtsc32();
            asm volatile("lfence\n\t");
            int index = j*8;
            for(int repeat = 0; repeat < 10000; repeat++){
                int inner_index = index;
                for(int k = 0; k < num_l2_assoc; k++){
                    tmp = target_buffer[inner_index];
                    inner_index += num_l2_set * 8;
                }
            }

            asm volatile("lfence\n\t");
            int after = rdtsc32();
            baseline += (after - before);
		}
        // printf("Baseline: %d\n", baseline / 10);
        baseline = baseline / 1024;
        int threshold = baseline / 6;

        int count = 0;
		for(int j = 0; j < num_l2_set; j++){
            int before = rdtsc32();
            asm volatile("lfence\n\t");
            int index = j*8;
            for(int repeat = 0; repeat < 10000; repeat++){
                int inner_index = index;
                for(int k = 0; k < num_l2_assoc; k++){
                    tmp = target_buffer[inner_index];
                    inner_index += num_l2_set * 8;
                }
            }

            asm volatile("lfence\n\t");
            int after = rdtsc32();
            count = (after - before);
            if(count - baseline > threshold) {
                flag = j;
                break;
            } 
		}

        if(flag != -1) {
            arr[num_signals] = flag;
            num_signals++;
        }

        if(num_signals == SIGNAL_SIZE) {
            listening = false;
            printf("Receiver finished.\n");
            break;
        }
	}


    // Put your capture-the-flag code here

    // printf("Flag: %d\n", flag);
    for(int i = 0; i < SIGNAL_SIZE; i++){
        printf("%d ", arr[i]);
    }

    flag = find_mode(arr, SIGNAL_SIZE);
    printf("\nFlag: %d\n", flag);
	return 0;
}