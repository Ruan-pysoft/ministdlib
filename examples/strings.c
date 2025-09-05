#include <ministd_fmt.h>
#include <ministd_io.h>
#include <ministd_memory.h>
#include <ministd_string.h>

int
main(void)
{
	String own greeting;
	String own recipient;
	String own temp;
	FILE own file;

	greeting = s_from_cstring("Hello, ", NULL);
	recipient = s_with_capacity(1024, NULL);

	s_append(recipient, "World!", 6, NULL);

	file = (FILE ref)sf_open_readonly(recipient, NULL);
	temp = s_fscan(file, NULL);
	s_sappend(greeting, temp, NULL);
	s_free(temp);
	close(file, NULL);
	free(file);

	s_print(greeting, NULL);
	printc('\n', NULL);

	s_free(greeting);
	s_free(recipient);

	return 0;
}
