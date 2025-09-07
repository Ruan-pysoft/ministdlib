#include <ministd_fmt.h>
#include <ministd_io.h>
#include <ministd_memory.h>
#include <ministd_string.h>

void
main(err_t ref err_out)
{
	String own greeting;
	String own recipient;
	FILE own file;

	(void) err_out;

	greeting = s_from_cstring("Hello, ", NULL);
	recipient = s_with_capacity(1024, NULL);

	s_append(recipient, "World!", 6, NULL);

	file = (FILE ref)sf_open_readonly(recipient, NULL);
	s_fscan_into(greeting, file, NULL);
	close(file, NULL);
	free(file);

	s_print(greeting, NULL);
	printc('\n', NULL);

	s_free(greeting);
	s_free(recipient);
}
