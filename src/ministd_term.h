#ifndef _RM_STD_TERM_H
#define _RM_STD_TERM_H

#include "ministd_types.h"
#include "ministd_error.h"
#include "ministd_io.h"

void term_csi_cursor_up(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_down(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_forward(int n, FILE ref file, err_t ref err_out);
void term_csi_cursor_back(int n, FILE ref file, err_t ref err_out);

void term_csi_clear_to_end(FILE ref file, err_t ref err_out);
void term_csi_clear_to_start(FILE ref file, err_t ref err_out);
void term_csi_clear_all(FILE ref file, err_t ref err_out);
void term_csi_clear_full(FILE ref file, err_t ref err_out);

void term_csi_goto(int x, int y, FILE ref file, err_t ref err_out);

#endif /* _RM_STD_TERM_H */
