#include "ministd_error.h"
#include <ministd_io.h>
#include <ministd_fmt.h>
#include <ministd_memory.h>
#include <ministd_string.h>

const char ref hello_world = "Hello, world";

#define ASSERT(msg) do { if (err) { perror(err, "Something went wrong " msg); exit(127); } } while (0)

void
s_append_cstring(String ref this, const char ref str, err_t ref err_out)
{
	s_append(this, str, strlen(str), err_out);
}

bool
test(const char ref name, bool cond)
{
	err_t err = ERR_OK;
	String own string;

	string = s_from_cstring("Test ", &err);
	ASSERT("while creating string");
	s_append_cstring(string, name, &err);
	ASSERT("while appending string");
	s_append_cstring(string, ": ", &err);
	ASSERT("while appending string");
	if (cond) s_append_cstring(string, "PASSED\n", &err);
	else s_append_cstring(string, "FAILED\n", &err);
	ASSERT("while appending string");
	s_print(string, NULL);
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
		StringFile own file;

		string = s_from_cstring("Expected ", &err);
		ASSERT("while creating string");
		file = sf_open(string, &err);
		ASSERT("while opening string file");

		fprintuz(expect, (FILE ref)file, &err);
		ASSERT("while writing to string file");
		s_append_cstring(string, ", got ", &err);
		ASSERT("while appending string");
		fprintuz(expr, (FILE ref)file, &err);
		ASSERT("while writing to string file");
		s_print(string, NULL);

		close((FILE ref)file, NULL);
		free(file);
		s_free(string);
	}

	return res;
}

void
main(err_t ref err_out)
{
	err_t err = ERR_OK;
	String own greeting;
	String own test;

	(void) err_out;

	greeting = s_new(&err);
	ASSERT("while creating new string");
	s_append_cstring(greeting, "Hello, ", &err);
	ASSERT("while appending string");
	s_append_cstring(greeting, "world", &err);
	ASSERT("while appending string");
	s_push(greeting, '!', &err);
	s_push(greeting, '\n', &err);
	ASSERT("while appending character");

	s_print(greeting, NULL);

	expect_usz("#1 (strlen)", strlen(hello_world), 12);
	expect_usz("#2 (strnlen)", strnlen(hello_world, 10), 10);
	expect_usz("#3 (strnlen)", strnlen(hello_world, 20), 12);
	test = s_with_capacity(1024, &err);
	ASSERT("while allocating new string");
	expect_usz("#4 (s_newalloc)", s_capacity(test), 1024);
	s_free(test);

	s_free(greeting);
}
