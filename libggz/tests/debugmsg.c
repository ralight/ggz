#include <stdio.h>
#include <errno.h>

#include <ggz.h>

int main(void) 
{
	char *foolist[] = {"foo", "bar", NULL};

	printf("-- Pre-init testing --\n");
	ggz_debug(NULL, "Testing ggz_debug with NULL type");
	ggz_debug("foo", "Testing ggz_debug with level foo");

	ggz_error_msg("Testing ggz_error_msg");
	ggz_error_sys("Testing ggz_error_sys (no errno)");
	errno = EIO;
	ggz_error_sys("Testing ggz_error_sys (errno EIO)");

	printf("\n-- NULL init/cleanup --\n");
	ggz_debug_init(NULL, NULL);
	ggz_debug_debug();
	ggz_debug_cleanup();
	ggz_debug_debug();

	printf("\n-- NULL init/test foo/cleanup\n");
	ggz_debug_init(NULL, NULL);
	ggz_debug("foo", "Testing foo before enabling (shouldn't see this)");
	ggz_debug_enable_type("foo");
	ggz_debug_debug();
	ggz_debug("foo", "Testing foo after enabling (should see this)");
	ggz_debug_disable_type("foo");
	ggz_debug_debug();
	ggz_debug("foo", "Testing foo after disabling (shouldn't see this)");
	ggz_debug_cleanup();
	
	printf("\n-- foo init/test foo, bar/cleanup\n");
	ggz_debug_init((const char**)foolist, NULL);
	ggz_debug_debug();
	ggz_debug("foo", "Testing foo (should see this)");
	ggz_debug("bar", "Testing bar (should see this)");
	ggz_debug("baz", "Testing baz (should not this)");
	ggz_debug_cleanup();
	
	printf("\n-- foo init to testfile\n");
	ggz_debug_init((const char**)foolist, "testfile");
	ggz_debug("foo", "Testing foo (should see this)");
	ggz_debug("bar", "Testing bar (should see this)");
	ggz_debug("baz", "Testing baz (should not this)");
	ggz_debug_cleanup();
	
	ggz_memory_check();
	return 0;
}
