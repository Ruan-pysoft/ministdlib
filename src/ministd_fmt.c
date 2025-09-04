#include <ministd_fmt.h>

#include <ministd_io.h>
#include <ministd_memory.h>

/* define print* functions, alias to fprint*(..., stdout, ...) */
#define X(suff, type) \
	isz \
	print ## suff(type suff, err_t ref err_out) \
	{ return fprint ## suff(suff, stdout, err_out); }
LIST_OF_FMTTYPES_NUMERIC
LIST_OF_FMTTYPES_SPECIAL_PRINT
#undef X

/* define print* functions for hex and binary, alias to fprint*(..., stdout, ...) */
#define X(suff, type) \
	isz \
	print ## suff ## x(type suff, err_t ref err_out) \
	{ return fprint ## suff ## x(suff, stdout, err_out); } \
	isz \
	print ## suff ## b(type suff, err_t ref err_out) \
	{ return fprint ## suff ## b(suff, stdout, err_out); }
LIST_OF_FMTTYPES_NUMERIC
#undef X

/* define scan* functions, alias to fscan*(stdin, ...) */
#define X(suff, type) \
	type \
	scan ## suff(err_t ref err_out) \
	{ return fscan ## suff(stdin, err_out); }
LIST_OF_FMTTYPES_NUMERIC
LIST_OF_FMTTYPES_SPECIAL_SCAN
#undef X

isz
fprintc(char c, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	fputc(c, file, &err);
	TRY_WITH(err, 0);
	return 1;
}
isz
fprints(const char ref s, FILE ref file, err_t ref err_out)
{
	return fputs(s, file, err_out);
}

isz
fprintl(long l, FILE ref file, err_t ref err_out)
{
	/* max int is 2^63-1 ~= 10^18 * 8, so no more than 20 digits */
	err_t err = ERR_OK;
	char buf[21];
	isz len = 0;

	if (l == 0) return fprintc('0', file, err_out);
	if ((unsigned long)l == (~(unsigned long)l)+1) {
		/* l == -2^63 */
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintul((unsigned long)l, file, err_out)+1;
	}
	if (l < 0) {
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintl(-l, file, err_out)+1;
	}

	buf[20] = 0;
	len = 1;
	while (len <= 20 && l > 0) {
		buf[20-len] = '0' + l%10;
		l /= 10;
		++len;
	}
	--len;

	return fputs(buf+20-len, file, err_out);
}
isz
fprintul(unsigned long ul, FILE ref file, err_t ref err_out)
{
	/* max int is 2^64-1 ~= 10^18 * 16, so no more than 21 digits */
	char buf[22];
	isz len = 0;

	if (ul == 0) return fprintc('0', file, err_out);

	buf[21] = 0;
	len = 1;
	while (len <= 21 && ul > 0) {
		buf[21-len] = '0' + ul%10;
		ul /= 10;
		++len;
	}
	--len;

	return fputs(buf+21-len, file, err_out);
}
isz
fprintlb(long l, FILE ref file, err_t ref err_out)
{
	/* max int is 2^63-1 so at most 63 digits */
	err_t err = ERR_OK;
	char buf[64];
	isz len = 0;

	if (l == 0) return fprintc('0', file, err_out);
	if ((unsigned long)l == (~(unsigned long)l)+1) {
		/* l == -2^63 */
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintulb((unsigned long)l, file, err_out)+1;
	}
	if (l < 0) {
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintlb(-l, file, err_out)+1;
	}

	buf[63] = 0;
	len = 1;
	while (len <= 63 && l > 0) {
		buf[63-len] = '0' + (l&1);
		l >>= 1;
		++len;
	}
	--len;

	return fputs(buf+63-len, file, err_out);
}
isz
fprintulb(unsigned long ul, FILE ref file, err_t ref err_out)
{
	/* max int is 2^64-1  so at most 64 digits */
	char buf[65];
	isz len = 0;

	if (ul == 0) return fprintc('0', file, err_out);

	buf[64] = 0;
	len = 1;
	while (len <= 64 && ul > 0) {
		buf[64-len] = '0' + (ul&1);
		ul >>= 1;
		++len;
	}
	--len;

	return fputs(buf+64-len, file, err_out);
}
isz
fprintlx(long l, FILE ref file, err_t ref err_out)
{
	/* max int is 2^63-1 so at most 16 digits */
	err_t err = ERR_OK;
	char buf[17];
	isz len = 0;

	if (l == 0) return fprintc('0', file, err_out);
	if ((unsigned long)l == (~(unsigned long)l)+1) {
		/* l == -2^63 */
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintulx((unsigned long)l, file, err_out)+1;
	}
	if (l < 0) {
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintlx(-l, file, err_out)+1;
	}

	buf[16] = 0;
	len = 1;
	while (len <= 16 && l > 0) {
		int digit = l&0xF;
		if (digit < 10) buf[16-len] = '0' + digit;
		else buf[16-len] = 'a' - 10 + digit;
		l >>= 4;
		++len;
	}
	--len;

	return fputs(buf+16-len, file, err_out);
}
isz
fprintulx(unsigned long ul, FILE ref file, err_t ref err_out)
{
	/* max int is 2^64-1  so at most 16 digits */
	char buf[17];
	isz len = 0;

	if (ul == 0) return fprintc('0', file, err_out);

	buf[16] = 0;
	len = 1;
	while (len <= 16 && ul > 0) {
		int digit = ul&0xF;
		if (digit < 10) buf[16-len] = '0' + digit;
		else buf[16-len] = 'a' - 10 + digit;
		ul >>= 4;
		++len;
	}
	--len;

	return fputs(buf+16-len, file, err_out);
}

#define FPRINT_RAW_S_IMPL(type, suffix) \
	isz fprint##suffix(type suffix, FILE ref file, err_t ref err_out) \
	{ return fprintl((long)suffix, file, err_out); } \
	isz fprint##suffix##b(type suffix, FILE ref file, err_t ref err_out) \
	{ return fprintlb((long)suffix, file, err_out); } \
	isz fprint##suffix##x(type suffix, FILE ref file, err_t ref err_out) \
	{ return fprintlx((long)suffix, file, err_out); }
#define FPRINT_RAW_U_IMPL(type, suffix) \
	isz fprint##suffix(type suffix, FILE ref file, err_t ref err_out) \
	{ return fprintul((unsigned long)suffix, file, err_out); } \
	isz fprint##suffix##b(type suffix, FILE ref file, err_t ref err_out) \
	{ return fprintulb((unsigned long)suffix, file, err_out); } \
	isz fprint##suffix##x(type suffix, FILE ref file, err_t ref err_out) \
	{ return fprintulx((unsigned long)suffix, file, err_out); }
#define FPRINT_IMPL(type, suffix) \
	FPRINT_RAW_S_IMPL(type, suffix) \
	FPRINT_RAW_U_IMPL(unsigned type, u##suffix)

FPRINT_IMPL(short, h)

FPRINT_IMPL(int, i)

/* TODO: Pretty sure this is incorrect; long being smaller than isz/usz? */

FPRINT_RAW_S_IMPL(isz, z)
FPRINT_RAW_U_IMPL(usz, uz)

isz
fprintp(const ptr p, FILE ref file, err_t ref err_out)
{ return fprintuzx((usz)p, file, err_out); }

char
fscanc(FILE ref file, err_t ref err_out)
{
	return fgetc(file, err_out);
}
char own
fscans(FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	char own result;
	usz result_cap;

	result_cap = 128;
	result = alloc(result_cap, &err);
	TRY_WITH(err, NULL);

	if (fgets(result, result_cap, file, &err)) {
		return result;
	}

	for (;;) {
		if (err != ERR_OK) {
			free(result);
			ERR_WITH(err, NULL);
		}

		result = realloc(result, 2*result_cap, &err);
		TRY_WITH(err, NULL);

		if (fgets(result+result_cap, result_cap, file, &err)) {
			return result;
		}
		result_cap *= 2;
	}
}

#define X(suff, type)                                                \
	type                                                         \
	fscan ## suff(FILE ref file, err_t ref err_out)              \
	{                                                            \
		err_t err = ERR_OK;                                  \
		char c;                                              \
		type res = 0;                                        \
                                                                     \
		while ((c = peekc(file, &err)) != -1 && c <= ' ') {  \
			TRY_WITH(err, 0);                            \
			fgetc(file, &err);                           \
			TRY_WITH(err, 0);                            \
		}                                                    \
		TRY_WITH(err, 0);                                    \
		if (c == -1) ERR_WITH(ERR_EOF, 0);                   \
                                                                     \
		if (c < '0' || c > '9') ERR_WITH(ERR_PARSE, 0);      \
		while ((c = fgetc(file, &err)) >= '0' && c <= '9') { \
			const type oldres = res;                     \
			TRY_WITH(err, 0);                            \
                                                                     \
			res *= 10;                                   \
			res += c - '0';                              \
                                                                     \
			if (res < oldres) {                          \
				ERR_WITH(ERR_OVERFLOW, 0);           \
			}                                            \
		}                                                    \
		TRY_WITH(err, 0);                                    \
		if (c != -1) {                                       \
			ungetc(file, c, &err);                       \
			TRY_WITH(err, 0);                            \
		}                                                    \
                                                                     \
		return res;                                          \
	}
LIST_OF_FMTTYPES_NUMERIC_UNSIGNED
#undef X
#define X(suff, type)                                                \
	type                                                         \
	fscan ## suff(FILE ref file, err_t ref err_out)              \
	{                                                            \
		err_t err = ERR_OK;                                  \
		char c;                                              \
		type res = 0;                                        \
		bool neg = false;                                    \
                                                                     \
		while ((c = peekc(file, &err)) != -1 && c <= ' ') {  \
			TRY_WITH(err, 0);                            \
			fgetc(file, &err);                           \
			TRY_WITH(err, 0);                            \
		}                                                    \
		TRY_WITH(err, 0);                                    \
		if (c == -1) ERR_WITH(ERR_EOF, 0);                   \
                                                                     \
		if (c == '-') {                                      \
			neg = true;                                  \
			fgetc(file, &err);                           \
			c = peekc(file, &err);                       \
		}                                                    \
		if (c < '0' || c > '9') ERR_WITH(ERR_PARSE, 0);      \
		while ((c = fgetc(file, &err)) >= '0' && c <= '9') { \
			const type oldres = res;                     \
			TRY_WITH(err, 0);                            \
                                                                     \
			res *= 10;                                   \
			res += (c - '0') * -(neg*2 - 1);             \
                                                                     \
			if (!neg && res < oldres) {                  \
				ERR_WITH(ERR_OVERFLOW, 0);           \
			} else if (neg && res > oldres) {            \
				ERR_WITH(ERR_OVERFLOW, 0);           \
			}                                            \
		}                                                    \
		TRY_WITH(err, 0);                                    \
		if (c != -1) {                                       \
			ungetc(file, c, &err);                       \
			TRY_WITH(err, 0);                            \
		}                                                    \
                                                                     \
		return res;                                          \
	}
LIST_OF_FMTTYPES_NUMERIC_SIGNED
#undef X
