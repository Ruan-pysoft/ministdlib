#include <ministd.h>
#include <ministd_io.h>
#include <ministd_types.h>

void
main(err_t ref err_out)
{
	int n = argc();
	char ref ref a = argv();
	int i;

	(void) err_out;

	for (i = 1; i-n; i=-~i) {
		fputs(i[a], stdout, NULL);
		putc(22*!!(~-n-i) + 10, NULL);
	}
}
