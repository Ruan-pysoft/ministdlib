#include <ministd.h>
#include <ministd_fmt.h>

usz state;

/* xorshift generator, from en.wikipedia.org/wiki/Xorshift */
usz
rand(void)
{
	usz x = state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return state = x;
}

/* get a ~roughly uniform distribution in the interval [min, max].
 * slightly biased towards lower numbers
 */
usz
randint(usz min, usz max)
{
	return rand()%(max-min+1) + min;
}

/* perform fixed-point division.
 * I'll explain the algorithm later
 */
usz fdiv(usz num, usz den, usz facc, usz ref fpart) {
	usz ipart = num / den;

	*fpart = num * facc / den;
	*fpart -= ipart * facc;

	if (2 * num * facc / den - 2 * ipart * facc - 2 * *fpart) ++*fpart;
	if (*fpart == facc) {
		++ipart;
		*fpart = 0;
	}

	return ipart;
}

/* correctly print the result of fixed-point division */
/* NOTE: facc should be a power of ten */
void
fprintfixed(usz ipart, usz fpart, usz facc, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz div;

	fprintuz(ipart, file, &err);
	TRY_VOID(err);
	fprintc('.', file, &err);
	TRY_VOID(err);
	for (div = facc / 10; div > 0; div /= 10) {
		fprintc('0' + (fpart/div)%10, file, &err);
		TRY_VOID(err);
	}
}
/* print a number with , as a decimal seperator */
void
fprintgrouped(usz n, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	usz div = 1;
	bool first = true;

	while (n / div / 1000 != 0) div *= 1000;

	for (; div > 0; div /= 1000) {
		if (first) {
			fprintuz(n / div, file, &err);
			TRY_VOID(err);

			first = false;
		} else {
			usz i;
			fprintc(',', file, &err);
			TRY_VOID(err);
			for (i = 100; i > 0; i /= 10) {
				fprintc('0' + (n/div/i)%10, file, &err);
				TRY_VOID(err);
			}
		}
	}
}

int
main(void)
{
	/* simulate a million rounds of the game
	 * described in youtu.be/LUCvSsx6-EU "A Finite Game of Infinite Rounds"
	 */
	usz loops = 1000 * 1000, i;
	usz total_rounds = 0, max_rounds = 0;

	state = 42;

	for (i = 0; i < loops; ++i) {
		usz round = 0, balls = 2, selected = 0;

		while (selected != 1) {
			++round;
			selected = randint(1, balls);
			++balls;
		}

		/* printuz(round, NULL);
		putc('\n', NULL); */

		total_rounds += round;
		if (round > max_rounds) max_rounds = round;

		if ((i+1) % 10000 == 0) {
			usz expected_ipart, expected_fpart;
			prints("Total rounds: ", NULL);
			fprintgrouped(total_rounds, stdout, NULL);
			prints(" (over ", NULL);
			fprintgrouped(i+1, stdout, NULL);
			prints(" games)\n", NULL);
			prints("Max. no. of rounds so far: ", NULL);
			printuz(max_rounds, NULL);
			putc('\n', NULL);
			prints("Expected rounds: ~", NULL);
			expected_ipart = fdiv(
				total_rounds, i+1, 1000 * 1000, &expected_fpart
			);
			fprintfixed(
				expected_ipart, expected_fpart, 1000 * 1000,
				stdout, NULL
			);
			putc('\n', NULL);
		}
	}

	return 0;
}
