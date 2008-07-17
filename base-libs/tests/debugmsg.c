#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>

#include <ggz.h>

static int debug_suite(char *filename)
{
	char *foolist[] = {"foo", "bar", NULL};

	printf("-- Testing before init --\n");
	ggz_debug(NULL, "Error: this should not appear");
	ggz_debug("foo", "Error: this should not appear");
	ggz_error_msg("Success: ggz_error_msg");
	ggz_error_sys("Success: ggz_error_sys (no errno)");
	errno = EIO;
	ggz_error_sys("Success: Testing ggz_error_sys (errno EIO)");

	printf("\n-- Testing with file %s and no initial types specified\n", filename);
	ggz_debug_init(NULL, filename);
	ggz_debug("foo", "Error: Debugging with type foo before enabling (this should not appear)");
	ggz_debug_enable("foo");
	ggz_debug("foo", "Success: Debugging with type foo after enabling");
	ggz_debug_disable("foo");
	ggz_debug("foo", "Error: Debugging with type foo after disabling (should not appear)");
	ggz_debug_cleanup(GGZ_CHECK_NONE);

	printf("-- Testing post cleanup --\n");
	ggz_debug(NULL, "Error: this should not appear");
	ggz_debug("foo", "Error: this should not appear");
	ggz_error_msg("Success: ggz_error_msg");
	ggz_error_sys("Success: ggz_error_sys (no errno)");
	errno = EIO;
	ggz_error_sys("Success: Testing ggz_error_sys (errno EIO)");
	
	printf("\n-- Testing with file %s and foo,bar initial types\n", filename);
	ggz_debug_init((const char**)foolist, filename);
	ggz_debug("foo", "Success: Debugging with type foo");
	ggz_debug("bar", "Success: Debugging with type bar");
	ggz_debug("baz", "Error: Debugging with type baz (should not appear )");
	ggz_debug_disable("foo");
	ggz_debug("foo", "Error: Debugging with type foo after disabling (should not appear)");
	ggz_debug_enable("qux");
	ggz_debug("qux", "Success: Debugging with type qux after enabling");
	
	ggz_debug_cleanup(GGZ_CHECK_MEM);

	return 0;
}


int main(void) 
{
	debug_suite(NULL);
	debug_suite("debug.log");

	return 0;
}


