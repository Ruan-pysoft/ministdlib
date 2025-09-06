#ifndef _RM_STD_TESTS_H
#define _RM_STD_TESTS_H
#ifdef TEST

#include <_ministd_prelude.h>

#include <ministd_fmt.h>

#define TEST_FN_NAME(name) _test_ ## name
#define TEST_FN(name) void TEST_FN_NAME(name)(FILE ref outf, err_t ref err_out)
#define TEST_OUTPUT_NAME(name) _test_output_ ## name
#define TEST_OUTPUT(name) char TEST_OUTPUT_NAME(name)[]

#define out(suff, val, err) fprint ## suff(val, outf, err)

#define OUT(suff, val) do { out(suff, val, &err); TRY_VOID(err); } while (0)

#define ASSERT_EQ(type, a, b) do { \
		if (a != b) { \
			err_t err = ERR_OK; \
			OUT(s, "Assertion failed " __FILE__ ":"); \
			OUT(i, __LINE__); \
			OUT(s, " :: " #a " ("); \
			OUT(type, a); \
			OUT(s, ") != " #b " ("); \
			OUT(type, b); \
			OUT(s, ")\n"); \
			ERR_VOID(ERR_ASSERT); \
		} \
	} while (0)

void test_main(err_t ref err_out);

#endif /* TEST */
#endif /* _RM_STD_TESTS_H */
