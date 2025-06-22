#include "ministd.h"
#include "ministd_io.h"
#include "ministd_error.h"

char buf[1024];

int
main(void)
{
	puts("Hello, world!", NULL); /* output to stdout with newline */
	putc('!', NULL);             /* output single char to stdout */
	fputs("And no auto newline here...", stdout, NULL);
	fputc('*', stdout, NULL);
	fputc('!', stdout, NULL);

	{
		err_t err;
		perror(ERR_OK, "Oh look, not-an-error");
		fd_close(1337, &err);
		if (err) perror(err, "Error writing to file");
		else fputs("Should've gotten an error closing fd 1337??\n", stderr, NULL);
	}

	/* TODO: test stdin */
	/* TODO: open file */

	return 0;
}
