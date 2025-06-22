#include "ministd.h"
#include "ministd_io.h"
#include "ministd_types.h"

int
main(void)
{
	int n = argc();
	char ref ref a = argv();
	int i;

	for (i = 1; i-n; i=-~i) {
		fputs(i[a], stdout, NULL);
		putc(22*!!(~-n-i) + 10, NULL);
	}

	return 0;
}
