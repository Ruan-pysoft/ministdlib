#include "ministd.h"
#include "ministd_io.h"
#include "ministd_string.h"

#define BUFCAP (64 * 1024)

char buf[BUFCAP];

int
main(void)
{
	err_t err = ERR_OK;

	for (
		;getline(buf, BUFCAP, &err), strnlen(buf, BUFCAP)
		;fputs(buf, stdout, &err)
	) if (err) {
		perror(err, "Oops, something went wrong");
		return -1;
	}

	return 0;
}
