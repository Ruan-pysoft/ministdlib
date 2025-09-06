#include <_ministd_tests.h>
#include <ministd_io.h>
#include <ministd_memory.h>
#include <ministd_string.h>

#ifdef TEST

static TEST_FN(assert_eq_helper)
{
	ASSERT_EQ(i, 6 * 7, 13 * 4);
}

static TEST_FN(assert_eq)
{
	err_t err = ERR_OK;
	TEST_FN_NAME(assert_eq_helper)(outf, &err);
	ASSERT_EQ(i, err, ERR_ASSERT);
}
TEST_OUTPUT(assert_eq) =
	"Assertion failed src/_ministd_tests.c:10 :: 6 * 7 (42) != 13 * 4 (52)\n"
;

#define X(name) TEST_FN(name); extern TEST_OUTPUT(name);
#include <_ministd_tests.def>
#undef X

struct test {
	const char ref name;
	void(ref test)(FILE ref outf, err_t ref err_out);
	const char ref expected_output;
} tests[] = {
#define X(name) { #name, TEST_FN_NAME(name), TEST_OUTPUT_NAME(name) },
#include <_ministd_tests.def>
#undef X
};
const usz tests_len = sizeof(tests)/sizeof(*tests);

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

	for (i = 0; i < tests_len; ++i) {
		err_t err = ERR_OK;

		s_clear(str);

		fprints("Running test ", stderr, NULL);
		fprints(tests[i].name, stderr, NULL);
		fprints("...\n", stderr, NULL);

		tests[i].test(file, &err);
		SET_ERR(err);

		if (err == ERR_OK && !string_cmp_cstring(str, tests[i].expected_output)) {
			fprints("TEST ", stderr, NULL);
			fprints(tests[i].name, stderr, NULL);
			fprints(" FAILED WITH INCORRECT OUTPUT:\n", stderr, NULL);
			fprints("GOT:\n", stderr, NULL);
			s_fprint(str, stderr, NULL);
			fprints("EXPECTED:\n", stderr, NULL);
			fprints(tests[i].expected_output, stderr, NULL);
		} else if (err == ERR_OK) {
			fprints("TEST ", stderr, NULL);
			fprints(tests[i].name, stderr, NULL);
			fprints(" SUCCEEDED\n", stderr, NULL);
		} else {
			fprints("TEST ", stderr, NULL);
			fprints(tests[i].name, stderr, NULL);
			fprints(" FAILED WITH CODE ", stderr, NULL);
			fprints(err_repr(err), stderr, NULL);
			fprints(":\n", stderr, NULL);
			fprints(err_str(err), stderr, NULL);
			fprintc('\n', stderr, NULL);
		}
	}

	close(file, NULL);
	free(file);
	s_free(str);
}

#endif /* TEST */
