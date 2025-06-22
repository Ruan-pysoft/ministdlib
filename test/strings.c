#include "ministd.h"
#include "ministd_io.h"
#include "ministd_string.h"

const char ref hello_world = "Hello, world";

#define ASSERT(msg) do { if (err) { perror(err, "Something went wrong " msg); exit(127); } } while (0)

bool
test(const char ref name, bool cond)
{
	err_t err = ERR_OK;
	String own string;

	string = s_copy("Test ", &err);
	ASSERT("while copying string");
	string = s_append(string, name, &err);
	ASSERT("while appending string");
	string = s_append(string, ": ", &err);
	ASSERT("while appending string");
	if (cond) string = s_append(string, "PASSED", &err);
	else string = s_append(string, "FAILED", &err);
	ASSERT("while appending string");
	puts(s_to_c(string), NULL);
	s_free(string);
	return cond;
}

bool
expect_usz(const char ref name, usz expr, usz expect)
{
	err_t err = ERR_OK;
	bool res = expr == expect;
	test(name, res);

	if (!res) {
		String own string;
		string = s_copy("Expected ", &err);
		ASSERT("while copying string");
		string = s_append(string, "###", &err); /* TODO: write number to string */
		ASSERT("while appending string");
		string = s_append(string, ", got ", &err);
		ASSERT("while appending string");
		string = s_append(string, "###", &err); /* TODO: write number to string */
		ASSERT("while appending string");
		puts(s_to_c(string), NULL);
		s_free(string);
	}

	return res;
}

int
main(int argc, char **argv)
{
	err_t err = ERR_OK;
	String own greeting;
	String own test;

	greeting = s_new(&err);
	ASSERT("while creating new string");
	s_append(greeting, "Hello, ", &err);
	ASSERT("while appending string");
	s_append(greeting, "world", &err);
	ASSERT("while appending string");
	s_putc(greeting, '!', &err);
	ASSERT("while appending character");

	puts(s_to_c(greeting), NULL);

	expect_usz("#1 (strlen)", strlen(hello_world), 12);
	expect_usz("#2 (strnlen)", strnlen(hello_world, 10), 10);
	expect_usz("#3 (strnlen)", strnlen(hello_world, 20), 12);
	test = s_newalloc(1024, &err);
	ASSERT("while allocating new string");
	expect_usz("#4 (s_newalloc)", test->end - test->base, 1024);
	s_free(test);

	s_free(greeting);

	return 0;
}
