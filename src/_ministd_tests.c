#include <_ministd_tests.h>
#include <ministd_io.h>
#include <ministd_memory.h>
#include <ministd_string.h>
#include <ministd_term.h>

#ifdef TEST

#undef ASSERT_LINE
#define ASSERT_LINE -1
static TEST_FN(assert_helper_eq)
{
	ASSERT_EQ(i, 6 * 7, 13 * 4);
}
static TEST_FN(assert_helper_ne)
{
	ASSERT_NE(i, 6<<1|1, 1337 / 100);
}
static TEST_FN(assert_helper_msg)
{
	err_t err = ERR_OK;
	/* do something ... */
	err = ERR_AGAIN;
	ASSERT(err == ERR_OK, "Some important operation produced an error!");
}
#undef ASSERT_LINE
#define ASSERT_LINE __LINE__
static TEST_FN(assert)
{
	err_t err = ERR_OK;
	TEST_FN_NAME(assert_helper_eq)(outf, &err);
	ASSERT(err == ERR_ASSERT, err_repr(err));

	err = ERR_OK;
	TEST_FN_NAME(assert_helper_ne)(outf, &err);
	ASSERT(err == ERR_ASSERT, err_repr(err));

	err = ERR_OK;
	TEST_FN_NAME(assert_helper_msg)(outf, &err);
	ASSERT(err == ERR_ASSERT, err_repr(err));
}
static TEST_OUTPUT(assert) =
	"Assertion failed src/_ministd_tests.c:-1 :: 6 * 7 (42) == 13 * 4 (52)\n"
	"Assertion failed src/_ministd_tests.c:-1 :: 6<<1|1 (13) != 1337 / 100 (13)\n"
	"Assertion failed src/_ministd_tests.c:-1 :: err == ERR_OK\n"
	"Some important operation produced an error!\n"
;

#define X(name) TEST_FN(name); extern TEST_OUTPUT(name);
#include <_ministd_tests.def>
#undef X

static struct test {
	const char ref name;
	void(ref test)(FILE ref outf, err_t ref err_out);
	const char ref expected_output;
} tests[] = {
#define X(name) { #name, TEST_FN_NAME(name), TEST_OUTPUT_NAME(name) },
#include <_ministd_tests.def>
#undef X
};
static const usz tests_len = sizeof(tests)/sizeof(*tests);

static bool
string_cmp_cstring(const String ref str, const char ref cstring)
{
	usz i;
	usz len = strlen(cstring);
	char own str_cstr = s_cstr(str, NULL);

	if (len != s_len(str)) {
		free(str_cstr);
		return false;
	}

	for (i = 0; i < len; ++i) {
		if (str_cstr[i] != cstring[i]) {
			free(str_cstr);
			return false;
		}
	}

	free(str_cstr);
	return true;
}

void
test_main(err_t ref err_out)
{
	usz i;
	String own str = s_new(NULL);
	FILE own file = (FILE ref)sf_open(str, NULL);
	usz tests_passed = 0;

	fprints("Running ", stderr, NULL);
	fprintuz(tests_len, stderr, NULL);
	fprints(" tests\n", stderr, NULL);

	for (i = 0; i < tests_len; ++i) {
		err_t err = ERR_OK;

		s_clear(str);

		fprints("Running test ", stderr, NULL);
		term_set_graphics(FG_YELLOW, stderr, NULL);
		fprints(tests[i].name, stderr, NULL);
		term_set_graphics(TG_RESET, stderr, NULL);
		fprints("...\n", stderr, NULL);

		tests[i].test(file, &err);
		SET_ERR(err);

		if (err == ERR_OK && !string_cmp_cstring(str, tests[i].expected_output)) {
			term_set_graphics(FG_RED, stderr, NULL);
			fprints("TEST ", stderr, NULL);
			term_set_graphics(FG_YELLOW, stderr, NULL);
			fprints(tests[i].name, stderr, NULL);
			term_set_graphics(FG_RED, stderr, NULL);
			fprints(" FAILED WITH INCORRECT OUTPUT!\n", stderr, NULL);
			term_set_graphics(TG_RESET, stderr, NULL);

			fprints("GOT:\n", stderr, NULL);
			s_fprint(str, stderr, NULL);
			fprints("EXPECTED:\n", stderr, NULL);
			fprints(tests[i].expected_output, stderr, NULL);
		} else if (err == ERR_OK) {
			++tests_passed;

			term_set_graphics(FG_GREEN, stderr, NULL);
			fprints("TEST ", stderr, NULL);
			term_set_graphics(FG_YELLOW, stderr, NULL);
			fprints(tests[i].name, stderr, NULL);
			term_set_graphics(FG_GREEN, stderr, NULL);
			fprints(" PASSED\n", stderr, NULL);
			term_set_graphics(TG_RESET, stderr, NULL);
		} else {
			term_set_graphics(FG_RED, stderr, NULL);
			fprints("TEST ", stderr, NULL);
			term_set_graphics(FG_YELLOW, stderr, NULL);
			fprints(tests[i].name, stderr, NULL);
			term_set_graphics(FG_RED, stderr, NULL);
			fprints(" FAILED WITH CODE ", stderr, NULL);
			fprints(err_repr(err), stderr, NULL);
			fprints(":\n", stderr, NULL);
			fprints(err_str(err), stderr, NULL);
			fprintc('\n', stderr, NULL);
			term_set_graphics(TG_RESET, stderr, NULL);
		}
	}

	close(file, NULL);
	free(file);
	s_free(str);

	fprints("Ran a total of ", stderr, NULL);
	fprintuz(tests_len, stderr, NULL);
	fprints(" tests.\n", stderr, NULL);

	fprints("Tests failed: ", stderr, NULL);
	fprintuz(tests_len - tests_passed, stderr, NULL);
	fprintc('\n', stderr, NULL);

	fprints("Tests passed: ", stderr, NULL);
	fprintuz(tests_passed, stderr, NULL);
	fprintc('\n', stderr, NULL);

	if (tests_passed != tests_len) exit(1);
}

#endif /* TEST */
