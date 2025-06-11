#include "ministd.h"
#include "ministd_io.h"
#include "ministd_memory.h"
#include "ministd_string.h"

char *allocs[1024];
const char *test_string = "Hai #.... :3";

int
main(int argc, char **argv)
{
	int i;

	puts("Starting memory test...");

	for (i = 0; i < 1024; ++i) {
		int j;

		allocs[i] = alloc(16);
		for (j = 0; j < strlen(test_string); ++j) {
			allocs[i][j] = test_string[j];
		}
		allocs[i][5] = '0' + (i/1000)%10;
		allocs[i][6] = '0' + (i/100)%10;
		allocs[i][7] = '0' + (i/10)%10;
		allocs[i][8] = '0' + i%10;
	}

	for (i = 0; i < 1024; i += 128) {
		puts(allocs[i]);
	}
	puts(allocs[13]);
	puts(allocs[1023]);

	for (i = 0; i < 1024; ++i) {
		free(allocs[i]);
	}

	return 0;
}
