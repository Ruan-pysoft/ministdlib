#include <ministd_io.h>

void
main(err_t ref err_out)
{
	(void) err_out;

	puts("This test should fail.", NULL);
	puts("The expected output should be a single line \"hi.\"", NULL);
	puts("Meanwhile, the actual output should be these three lines.", NULL);
}
