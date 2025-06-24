#include "ministd_term.h"

#include "ministd_fmt.h"
#include "ministd_string.h"

static void
csi_start(FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;

	fprintc(0x1B, file, &err); /* \e */
	TRY_VOID(err);
	fprintc('[', file, &err);
	TRY_VOID(err);
}
static void
csi_helper_n(int n, char end, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	String own str;
	FILE own sf;

	str = s_new(&err);
	TRY_VOID(err);
	sf = (FILE*)sf_open(str, &err);
	TRY_VOID(err);

	csi_start(sf, &err);
	TRY_VOID(err);
	fprinti(n, sf, &err);
	TRY_VOID(err);
	fprintc(end, sf, &err);
	TRY_VOID(err);

	close(sf, &err);
	TRY_VOID(err);
	fprints(s_to_c(str), file, &err);
	TRY_VOID(err);
	s_free(str);
}
static void
csi_helper_nm(int n, int m, char end, FILE ref file, err_t ref err_out)
{
	err_t err = ERR_OK;
	String own str;
	FILE own sf;

	str = s_new(&err);
	TRY_VOID(err);
	sf = (FILE*)sf_open(str, &err);
	TRY_VOID(err);

	csi_start(sf, &err);
	TRY_VOID(err);
	fprinti(n, sf, &err);
	TRY_VOID(err);
	fprintc(';', sf, &err);
	TRY_VOID(err);
	fprinti(m, sf, &err);
	TRY_VOID(err);
	fprintc(end, sf, &err);
	TRY_VOID(err);

	close(sf, &err);
	TRY_VOID(err);
	fprints(s_to_c(str), file, &err);
	TRY_VOID(err);
	s_free(str);
}

void
term_csi_cursor_up(int n, FILE ref file, err_t ref err_out)
{
	csi_helper_n(n, 'A', file, err_out);
}
void
term_csi_cursor_down(int n, FILE ref file, err_t ref err_out)
{
	csi_helper_n(n, 'B', file, err_out);
}
void
term_csi_cursor_forward(int n, FILE ref file, err_t ref err_out)
{
	csi_helper_n(n, 'C', file, err_out);
}
void
term_csi_cursor_back(int n, FILE ref file, err_t ref err_out)
{
	csi_helper_n(n, 'D', file, err_out);
}

void
term_csi_clear_to_end(FILE ref file, err_t ref err_out)
{
	csi_helper_n(0, 'J', file, err_out);
}
void
term_csi_clear_to_start(FILE ref file, err_t ref err_out)
{
	csi_helper_n(1, 'J', file, err_out);
}
void
term_csi_clear_all(FILE ref file, err_t ref err_out)
{
	csi_helper_n(2, 'J', file, err_out);
}
void
term_csi_clear_full(FILE ref file, err_t ref err_out)
{
	csi_helper_n(3, 'J', file, err_out);
}

void
term_csi_goto(int x, int y, FILE ref file, err_t ref err_out)
{
	csi_helper_nm(x, y, 'H', file, err_out);
}
