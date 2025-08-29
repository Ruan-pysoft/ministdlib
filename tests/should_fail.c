#include <ministd_io.h>

int
main(void)
{
	puts("This test should fail.", NULL);
	puts("The expected output should be a single line \"hi.\"", NULL);
	puts("Meanwhile, the actual output should be these three lines.", NULL);

	return 0;
}
