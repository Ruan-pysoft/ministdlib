#include <ministd.h>
#include <ministd_io.h>
#include <ministd_fmt.h>
#include <ministd_term.h>
#include <ministd_time.h>
#include <ministd_types.h>

#define SLEN 41

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

int
atoi(char ref buf)
{
	int res = 0;
	int i;

	for (i = 0; '0' <= buf[i] && buf[i] <= '9'; ++i) {
		res *= 10;
		res += buf[i] - '0';
	}

	return res;
}

int
main(void)
{
	int r;
	int x, y, t;
	float fps;
	float frametime;

	prints("Sphere radius> ", NULL);
	getline(buf, 10, NULL);
	r = atoi(buf);
	if (r > SLEN>>1) r = SLEN>>1;

	fps = 2*r + 1; /* show whole sphere in one second */
	frametime = 1 / (float)fps;

	/* clear screen & move cursor to top-left */
	for (y = 0; y <= SLEN; ++y) printc('\n', NULL);
	term_csi_clearscreen(stdout, NULL);
	term_csi_goto(1, 1, stdout, NULL);
	flush(stdout, NULL);

	t = SLEN/2-r-2;
	if (t < 0) t = 0;
	for (;;) {
		char (ref tmp)[SLEN][SLEN];
		struct update ref update;

		updatequeue_len = 0;

		for (x = SLEN/2-r; x != SLEN/2+r+1; ++x) {
			for (y = SLEN/2-r; y != SLEN/2+r+1; ++y) {
				int dx, dy, dt, d2;
				dx = x - SLEN/2;
				dy = y - SLEN/2;
				dt = t - SLEN/2;
				d2 = dx*dx + dy*dy + dt*dt;
				if (d2 <= r*r - 16) {
					(*front)[x][y] = '#';
				} else if (d2 <= r*r - 8) {
					(*front)[x][y] = '%';
				} else if (d2 <= r*r) {
					(*front)[x][y] = ':';
				} else if (d2 <= r*r + 1) {
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

		for (update = updatequeue; update != updatequeue+updatequeue_len; ++update) {
			term_csi_goto(update->y+1, 2*update->x+1, stdout, NULL);
			printc((*front)[update->x][update->y], NULL);
			printc((*front)[update->x][update->y], NULL);
		}
		term_csi_goto(1, 1, stdout, NULL);
		flush(stdout, NULL);

		{ tmp = front; front = back; back = tmp; }

		++t;
		if (t == SLEN/2+r+2 + 1 || t == SLEN) {
			t = SLEN/2-r-2;
			if (t < 0) t = 0;
		}
		sleep(frametime, NULL);
	}

	return 0;
}
