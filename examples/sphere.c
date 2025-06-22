#include "ministd.h"
#include "ministd_types.h"

/* TODO: get this working */

#define SLEN 81

char buf[10];

char sb0[SLEN][SLEN] = {{32}};
char sb1[SLEN][SLEN] = {{32}};
char (ref front)[SLEN][SLEN] = &sb0;
char (ref back)[SLEN][SLEN] = &sb1;

struct update {
	int x, y;
};

struct update updatequeue[SLEN*SLEN];
usz updatequeue_len = 0;

itoa(n, buf, buflen)
	str buf;
{
	int maxlen, nlen, tmpn, i;

	maxlen = buflen-1;
	tmpn = n/10;
	LOOP FROM nlen = 0 WHILE tmpn ADV tmpn /= 10 DO;
	if (nlen > maxlen) return 1;

	nlen[buf] = 0;
	LOOP FROM i = 0 UNTIL i == nlen ADV ++i, n /= 10 DO {
		buf[nlen-i-1] = '0' + n%10;
	}

	return 0;
}

main()
{
	int r;
	int x, y, t;

	fputs("Sphere radius> ", STDOUT);
	read(STDIN, buf, 10);
	r = atoi(buf);
	if (r > SLEN>>1) r = SLEN>>1;

	/* clear screen & move cursor to top-left */
	fputs("\033[2J\033[1;1H", STDOUT);
	fsync(STDOUT);

	LOOP FROM t = SLEN/2-r FOREVER ADV DO {
		char (ref tmp)[SLEN][SLEN];
		struct update ref update;

		updatequeue_len = 0;

		LOOP FROM x = SLEN/2-r UNTIL x == SLEN/2+r+1 ADV ++x DO {
			LOOP FROM y = SLEN/2-r UNTIL y == SLEN/2+r+1 ADV ++y DO {
				int dx, dy, d2;
				dx = x - SLEN/2;
				dy = y - SLEN/2;
				d2 = dx*dx + dy*dy;
				if (d2 < r) {
					(*front)[x][y] = '#';
				} else if (d2 == r) {
					(*front)[x][y] = '.';
				} else {
					(*front)[x][y] = ' ';
				}
				if ((*front)[x][y] != (*back)[x][y]) {
					updatequeue[updatequeue_len].x = x;
					updatequeue[updatequeue_len].y = y;
					++updatequeue_len;
				}
			}
		}

		LOOP FROM update = updatequeue UNTIL update == updatequeue+updatequeue_len ADV ++update DO {
			itoa(update->x+1, buf, 5);
			itoa(update->y+1, buf+5, 5);
			fputs("\033[", STDOUT);
			fputs(buf, STDOUT);
			fputs(";", STDOUT);
			fputs(buf+5, STDOUT);
			fputs("H", STDOUT);
			write(STDOUT, &(*front)[update->x][update->y], 1);
		}
		fputs("\033[1;1HDone!", STDOUT);
			fputs(buf, STDOUT);
		fsync(STDOUT);

		{ tmp = front; front = back; back = tmp; }

		++t;
		if (t == SLEN/2+r+1) t = SLEN/2-r;
		sleep(1);
	}

	return 0;
}
