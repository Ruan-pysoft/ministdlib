#include "ministd.h"
#include "ministd_types.h"
#include "ministd_string.h"

int
main(void)
{
	String own greeting;
	String own recipient;

	greeting = s_copy("Hello,", NULL);
	recipient = s_newalloc(1024, NULL);

	recipient = s_append(recipient, "World!", NULL);
	recipient = (String own)s_restart(s_as_sv(recipient));

	greeting = s_parse(s_as_sv(recipient), greeting, NULL);

	puts(s_to_c(greeting), NULL);

	s_free(greeting);
	s_free(recipient);

	return 0;
}
