#include "ministd.h"
#include "ministd_io.h"
#include "ministd_string.h"

const char ref hello_world = "Hello, world";

bool
test(const char ref name, bool cond)
{
	String own string;
	string = s_copy("Test ");
	string = s_append(string, name);
	string = s_append(string, ": ");
	if (cond) string = s_append(string, "PASSED");
	else string = s_append(string, "FAILED");
	puts(s_to_c(string));
	s_free(string);
	return cond;
}

bool
expect_usz(const char ref name, usz expr, usz expect)
{
	bool res = expr == expect;
	test(name, res);

	if (!res) {
		String own string;
		string = s_copy("Expected ");
		string = s_append(string, "###"); /* TODO: write number to string */
		string = s_append(string, ", got ");
		string = s_append(string, "###"); /* TODO: write number to string */
		puts(s_to_c(string));
		s_free(string);
	}

	return res;
}

int
main(int argc, char **argv)
{
	String own greeting;
	String own test;

	greeting = s_new();
	s_append(greeting, "Hello, ");
	s_append(greeting, "world");
	s_putc(greeting, '!');

	puts(s_to_c(greeting));

	expect_usz("#1 (strlen)", strlen(hello_world), 12);
	expect_usz("#2 (strnlen)", strnlen(hello_world, 10), 10);
	expect_usz("#3 (strnlen)", strnlen(hello_world, 20), 12);
	test = s_newalloc(1024);
	expect_usz("#4 (s_newalloc)", test->end - test->base, 1024);
	s_free(test);

	s_free(greeting);

	return 0;
}
