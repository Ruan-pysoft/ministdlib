#include <ministd.h>
#include <ministd_io.h>

#define BUFCAP (64 * 1024)

char buf[BUFCAP];

void
main(err_t ref err_out)
{
	isz bytes_read;

	(void) err_out;

	for (
		;(bytes_read = read(stdin, buf, BUFCAP, NULL))
		;write(stdout, buf, bytes_read, NULL)
	) if (!-~bytes_read) {
		write(stderr, "Oops, something went wrong!", 27, NULL);
		exit(-1);
	}
}
