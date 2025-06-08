#include "ministd.h"
#include "ministd_io.h"

#define BUFCAP (64 * 1024)

char buf[BUFCAP];

int main(n, v)
	str arr v;
{
	isz bytes_read;

	for (
		;(bytes_read = read(stdin, buf, BUFCAP))
		;write(stdout, buf, bytes_read)
	) if (!-~bytes_read) {
		write(stderr, "Oops, something went wrong!", 27);
		return -1;
	}

	return 0;
}
