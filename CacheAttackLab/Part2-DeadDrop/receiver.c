
#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

#define BUFF_SIZE (1<<21)
#define SIZE 1024

int main(int argc, char **argv)
{
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
	uint64_t access_time = 0;
	uint64_t tmp;
    int arr[SIZE];

	for (int i = 0; i < SIZE; i++) {
        arr[i] = i;
    }

    srand((unsigned int)time(NULL));

    for (int i = SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Swap arr[i] and arr[j].
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	bool listening = true;
	int count = 0;
	while (listening) {

		// Put your covert channel code here

		int success = 0;

		asm volatile("lfence\n\t");
		for(int i = 0; i < 100000; i++){
			asm("pause");
		}
		int affected_set = 0;
		for(int j = num_l2_set - 1; j > -1; j--){
			count = 0;
			for(int repeat = 0; repeat < 500; repeat++){
				for(int k = 0; k < num_l2_assoc; k++){
					access_time = measure_one_block_access_time((uint64_t)target_buffer + 64 * arr[j] + k * num_l2_set * 64);
					if (access_time > 50) {
						count += 1;
					}
				}
			}
			if(count > 250) {
				affected_set++;
			}
		}
			printf("Receiver: %d\n", affected_set);

	}

	printf("Receiver finished.\n");

	return 0;
}


