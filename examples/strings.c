#include "ministd.h"
#include "ministd_types.h"
#include "ministd_string.h"

int
main(int argc, char **argv)
{
	String own greeting;
	String own recipient;

	greeting = s_copy("Hello,");
	recipient = s_newalloc(1024);

	recipient = s_append(recipient, "World!");
	recipient = (String own)s_restart(s_as_sv(recipient));

	greeting = s_parse(s_as_sv(recipient), greeting);

	puts(s_to_c(greeting));

	s_free(greeting);
	s_free(recipient);

	return 0;
}
