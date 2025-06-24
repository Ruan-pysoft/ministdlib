#include "ministd_fmt.h"

#include "ministd_error.h"
#include "ministd_io.h"

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
		return fprintul((unsigned long)l, file, err_out);
	}
	if (l < 0) {
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintl(-l, file, err_out);
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
	err_t err = ERR_OK;
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
		return fprintulb((unsigned long)l, file, err_out);
	}
	if (l < 0) {
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintlb(-l, file, err_out);
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
	err_t err = ERR_OK;
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
		return fprintulx((unsigned long)l, file, err_out);
	}
	if (l < 0) {
		fprintc('-', file, &err);
		TRY_WITH(err, 0);
		return fprintlx(-l, file, err_out);
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
	err_t err = ERR_OK;
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

#include "ministd_memory.h"

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
	isz len = 0;
	isz bytes_read;

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
#define INTSCAN(type) do {                                           \
		err_t err = ERR_OK;                                  \
		char c;                                              \
		type res = 0;                                        \
                                                                     \
		while ((c = peekc(file, &err) != -1) && c <= ' ') {  \
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
	} while (0)
long
fscanl(FILE ref file, err_t ref err_out)
{
	INTSCAN(signed long);
}
unsigned long
fscanul(FILE ref file, err_t ref err_out)
{
	INTSCAN(unsigned long);
}
isz
fscanz(FILE ref file, err_t ref err_out)
{
	INTSCAN(isz);
}
usz
fscanuz(FILE ref file, err_t ref err_out)
{
	INTSCAN(usz);
}
int
fscani(FILE ref file, err_t ref err_out)
{
	INTSCAN(signed int);
}
unsigned int
fscanu(FILE ref file, err_t ref err_out)
{
	INTSCAN(unsigned int);
}
short
fscanh(FILE ref file, err_t ref err_out)
{
	INTSCAN(signed short);
}
unsigned short
fscanuh(FILE ref file, err_t ref err_out)
{
	INTSCAN(unsigned short);
}
