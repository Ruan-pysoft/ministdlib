#include "ministd.h"
#include "ministd_io.h"

#define BUFCAP (64 * 1024)

char buf[BUFCAP];

int
main(int n, char ref ref v)
{
	isz bytes_read;

	for (
		;(bytes_read = read(stdin, buf, BUFCAP, NULL))
		;write(stdout, buf, bytes_read, NULL)
	) if (!-~bytes_read) {
		write(stderr, "Oops, something went wrong!", 27, NULL);
		return -1;
	}

	return 0;
}
