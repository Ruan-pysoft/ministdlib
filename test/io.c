#include "ministd.h"
#include "ministd_io.h"

char buf[1024];

int
main(int argc, char **argv)
{
	puts("Hello, world!"); /* output to stdout with newline */
	putc('!');             /* output single char to stdout */
	fputs("And no auto newline here...", stdout);
	fputc('*', stdout);
	fputc('!', stdout);

	/* TODO: test stdin */
	/* TODO: open file */

	return 0;
}
