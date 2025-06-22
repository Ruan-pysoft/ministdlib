#include "ministd.h"
#include "ministd_types.h"

int fib(n)
{
	int a, b, tmp, i;
	a = 0;
	b = 1;

	for (i = 0; i < n; ++i) {
		tmp = a+b;
		a = b;
		b = tmp;
	}

	return a;
}

int main(argc, argv)
	const char ref ref argv;
{
	int i;

	puts("Hello, world!", NULL);
	for (i = 0; i < argc; ++i) {
		fputs(argv[i], stdout, NULL);
		if (i < argc-1) putc(' ', NULL);
		else putc('\n', NULL);
	}

	return 42;
}
