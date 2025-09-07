#include <ministd.h>
#include <ministd_io.h>
#include <ministd_fmt.h>
#include <ministd_memory.h>
#include <ministd_poll.h>

char linebuf[1024];

void
main(err_t ref err_out)
{
	poll_list_t poll_list = poll_list_new(NULL);
	poll_list_add_file(&poll_list, stdin, PF_IN, NULL);

	(void) err_out;

	prints("Press ctrl+D with no input to exit.\n", NULL);

	for (;;) {
		usz n;

		n = poll_list_poll(&poll_list, 5000, NULL);

		if (n == 0) {
			prints("Another five seconds has passed...\n", NULL);
		} else {
			getline(linebuf, 1024, NULL);
			if (*linebuf == 0) return;
			prints("Got input: ", NULL);
			prints(linebuf, NULL);
			flush(stdout, NULL);
		}
	}
}
