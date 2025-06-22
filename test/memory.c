#include "ministd.h"
#include "ministd_memory.h"

char *allocs[1024];
const char *test_string = "Hai #.... :3";

int
strlen(const char ref str)
{
	int res;

	for (res = 0; str[res] != 0; ++res);

	return res;
}

int
main(void)
{
	err_t err = ERR_OK;
	int i;

	puts("Starting memory test...", NULL);

	for (i = 0; i < 1024; ++i) {
		int j;

		allocs[i] = alloc(16, &err);
		if (err) {
			perror(err, "Something went wrong while allocating memory");
			exit(127);
		}
		for (j = 0; j < strlen(test_string); ++j) {
			allocs[i][j] = test_string[j];
		}
		allocs[i][5] = '0' + (i/1000)%10;
		allocs[i][6] = '0' + (i/100)%10;
		allocs[i][7] = '0' + (i/10)%10;
		allocs[i][8] = '0' + i%10;
	}

	for (i = 0; i < 1024; i += 128) {
		puts(allocs[i], NULL);
	}
	puts(allocs[13], NULL);
	puts(allocs[1023], NULL);

	for (i = 0; i < 1024; ++i) {
		free(allocs[i]);
	}

	return 0;
}
