#define MINI_LIB_IMPL
#include "minilib.h"

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
	str arr argv;
{
	int i;

	puts("Hello, world!");
	for (i = 0; i < argc; ++i) {
		fputs(argv[i], stdout);
		if (i < argc-1) putc(' ');
		else putc('\n');
	}

	return 42;
}
