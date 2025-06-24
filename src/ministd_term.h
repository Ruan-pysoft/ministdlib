#ifndef _RM_STD_TERM_H
#define _RM_STD_TERM_H

#include "ministd_types.h"
#include "ministd_error.h"
#include "ministd_io.h"

void term_csi_cursor_up(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_down(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_forward(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_back(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_nextline(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_prevline(int n, FILE ref file, err_t ref err_out);

void term_csi_goto_col(int n, FILE ref file, err_t ref err_out);
void term_csi_goto(int x, int y, FILE ref file, err_t ref err_out);

void term_csi_clearscreen_to_end(FILE ref file, err_t ref err_out);
void term_csi_clearscreen_to_start(FILE ref file, err_t ref err_out);
void term_csi_clearscreen(FILE ref file, err_t ref err_out);
void term_csi_clearscreen_full(FILE ref file, err_t ref err_out);
void term_csi_clear_line_to_end(FILE ref file, err_t ref err_out);
void term_csi_clear_line_to_start(FILE ref file, err_t ref err_out);
void term_csi_clear_line(FILE ref file, err_t ref err_out);

void term_csi_scroll_up(int n, FILE ref file, err_t ref err_out);
void term_csi_scroll_down(int n, FILE ref file, err_t ref err_out);

enum TERM_GRAPHICS {
	TG_RESET = 0,
	TG_BOLD = 1,
	TG_FAINT = 2,
	TG_ITALIC = 3,
	TG_UNDERLINE = 4,
	TG_SLOW_BLINK = 5,
	TG_FAST_BLINK = 6, /* WARN: Not widely supported */
	TG_INVERT = 7,
	TG_STRIKETHROUGH = 9,
	TG_FONT_DEFAULT = 10,
	TG_FONT1 = 11,
	TG_FONT2 = 12,
	TG_FONT3 = 13,
	TG_FONT4 = 14,
	TG_FONT5 = 15,
	TG_FONT6 = 16,
	TG_FONT7 = 17,
	TG_FONT8 = 18,
	TG_FONT9 = 19,
	TG_GOTHIC = 20, /* WARN: Not widely supported */
	TG_NORMAL_INTENSITY = 22,
	TG_NO_ITALIC_BLACKLETTER = 23,
	TG_NO_UNDERLINE = 24,
	TG_NO_BLINK = 25,
	TG_NO_REVERSE = 27,
	TG_REVEAL = 28,
	TG_NO_STRIKETHROUGH = 29,

	/* colours! */
	FG_DEFAULT = 39,
	FG_BLACK = 30,
	FG_RED = 31,
	FG_GREEN = 32,
	FG_YELLOW = 33,
	FG_BLUE = 34,
	FG_MAGENTA = 35,
	FG_CYAN = 36,
	FG_WHITE = 37,
	FG_GRAY = 90,
	FG_BRIGHT_RED = 91,
	FG_BRIGHT_GREEN = 92,
	FG_BRIGHT_YELLOW = 93,
	FG_BRIGHT_BLUE = 94,
	FG_BRIGHT_MAGENTA = 95,
	FG_BRIGHT_CYAN = 96,
	FG_BRIGHT_WHITE = 97,

	BG_DEFAULT = 49,
	BG_BLACK = 40,
	BG_RED = 41,
	BG_GREEN = 42,
	BG_YELLOW = 43,
	BG_BLUE = 44,
	BG_MAGENTA = 45,
	BG_CYAN = 46,
	BG_WHITE = 47,
	BG_GRAY = 100,
	BG_BRIGHT_RED = 101,
	BG_BRIGHT_GREEN = 102,
	BG_BRIGHT_YELLOW = 103,
	BG_BRIGHT_BLUE = 104,
	BG_BRIGHT_MAGENTA = 105,
	BG_BRIGHT_CYAN = 106,
	BG_BRIGHT_WHITE = 107
};
void term_set_graphics(enum TERM_GRAPHICS code, FILE ref file, err_t ref err_out);

#endif /* _RM_STD_TERM_H */
