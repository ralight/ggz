/* Test program for configuration file functions */
/* Bonus:  This also tests list handling pretty intensively */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ggz.h>

void build_testfile(void);
void check_testfile(void);
void check_raw_testfile(void);
void cleanup(void);
void fail(char *msg);
void pass(void);
void crit_fail(char *msg);

int failures=0;
int successes=0;

char *lines1[] = {
	"[Section1]",
	"Item1 = A string",
	"Item2 = 1",
	"Section2 = Not really a section ID",
	"[Section2]",
	"Item1 = Another string",
	"Item2 = 2",
	"[Section3]",
	"Item1 = We can delete this item"
};

char *lines2[] = {
	"[IntTest]",
	"Negative = -84",
	"Positive = 15",
	"[ListTest]",
	"List = String\\ 1 String\\ 2 String\\ 3",
	"[StringTest]",
	"String = A string"
};

#define NUM_LINES(X)	(sizeof(X) / sizeof(char *))

char *list_src[] = { "String 1", "String 2", "String 3" };

int main(int argc, char *argv[])
{
	printf("* This program intentionally causes some errors.\n");
	printf("* Ignore any debug output you may see.\n\n");

	build_testfile();
	check_testfile();
	check_raw_testfile();

	if(failures == 0)
		printf("\n*** All tests succeeded ***\n");
	else
		printf("%d tests succeeded\n%d tests failed\n",
			successes, failures);
	return failures;
}


void check_raw_testfile(void)
{
	int i=0;
	int ok=1;
	FILE *testfile;
	char line[1024];

	if((testfile = fopen("cfg_testfile", "r")) == NULL)
		crit_fail("'cfg_testfile' raw file has disappeared!?");
	else
		pass();

	while(fgets(line, 1024, testfile) && ok) {
		if(line[0] == '\n')
			continue;
		line[strlen(line)-1] = '\0';
		if(i == NUM_LINES(lines2)) {
			ok = 0;
			break;
		}
		if(strcmp(line, lines2[i]))
			ok = 0;
		i++;
	}
	fclose(testfile);

	if(ok) {
		pass();
		unlink("cfg_testfile");
	} else
		fail("Raw 'cfg_testfile' is incorrect format, retaining file");
}


void check_testfile(void)
{
	int i;
	int handle;
	int num;
	char *str=NULL;
	char **list_dst;
	int list_items;

	/* Check that we can open the config file */
	if((handle = ggz_conf_parse("cfg_testfile", GGZ_CONF_RDONLY)) < 0)
		crit_fail("ggz_conf_parse() failed");
	else
		pass();

	/* Check that default (non-existent items) work */
	str = ggz_conf_read_string(handle, "Section1", "Item3", "OK");
	if(strcmp(str, "OK"))
		fail("ggz_conf_read_string() failed to return default");
	else
		pass();
	if(str) {
		ggz_free(str);
		str = NULL;
	}

	/* Check that a string can be read properly */
	str = ggz_conf_read_string(handle, "Section2", "Item1", NULL);
	if(!str || strcmp(str, "Another string"))
		fail("ggz_conf_read_string() returned the wrong value");
	else
		pass();
	if(str) {
		ggz_free(str);
		str = NULL;
	}

	/* Check that an integer can be read properly */
	num = ggz_conf_read_int(handle, "Section1", "Item2", -1);
	if(num != 1)
		fail("ggz_conf_read_int() returned the wrong value");
	else
		pass();

	/* Check that we get an error if we try to alter a value */
	num = ggz_conf_write_string(handle, "Section3", "Item1", "New value");
	if(num == 0)
		fail("ggz_conf_write_string() didn't not return a RO error");
	else
		pass();

	ggz_conf_cleanup();

	/* Test that we cannot open a RO file as RW */
	chmod("cfg_testfile", 0400);
	if((handle = ggz_conf_parse("cfg_testfile", GGZ_CONF_RDWR)) < 0)
		pass();
	else {
		/* I'm root, thou shalt not question my power */
		if(!getuid()) {
			pass();
			ggz_conf_cleanup();
		}
		else {
			fail("ggz_conf_parse() thinks it can write to a RO file");
			ggz_conf_cleanup();
		}
	}
	unlink("cfg_testfile");

	/* Test that we cannot create a file in RO mode */
	handle = ggz_conf_parse("cfg_testfile", GGZ_CONF_CREATE | GGZ_CONF_RDONLY);
	if(handle < 0)
		pass();
	else {
		fail("ggz_conf_parse() allowed creating a read only file");
		ggz_conf_cleanup();
		unlink("cfg_testfile");
	}

	/* Test that we cannot open a non-existent file w/o GGZ_CONF_CREATE */
	if((handle = ggz_conf_parse("cfg_testfile", GGZ_CONF_RDWR)) < 0)
		pass();
	else {
		fail("ggz_conf_parse() created a file w/o GGZ_CONF_CREATE");
		ggz_conf_cleanup();
		unlink("cfg_testfile");
	}

	/* Test that we can create a new file */
	handle = ggz_conf_parse("cfg_testfile", GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	if(handle < 0)
		crit_fail("ggz_conf_parse() failed to create 'cfg_testfile'");
	pass();

	/* Write a few values into the config file */
	if(ggz_conf_write_list(handle, "ListTest", "List", 3, list_src))
		fail("ggz_conf_write_list() returned an error");
	else
		pass();
	if(ggz_conf_write_string(handle, "StringTest", "String", "A string"))
		fail("ggz_conf_write_string() returned an error");
	else
		pass();
	if(ggz_conf_write_int(handle, "IntTest", "Positive", 15))
		fail("ggz_conf_write_int() returned an error");
	else
		pass();
	if(ggz_conf_write_int(handle, "IntTest", "Negative", -84))
		fail("ggz_conf_write_int() returned an error");
	else
		pass();
	/* Write a few values for deletion */
	if(ggz_conf_write_string(handle, "StringTest", "DelString", "A string"))
		fail("ggz_conf_write_string() returned an error");
	else
		pass();
	if(ggz_conf_write_int(handle, "DeleteMe", "Int", 42))
		fail("ggz_conf_write_int() returned an error");
	else
		pass();

	/* Commit and reopen */
	if(ggz_conf_commit(handle))
		crit_fail("ggz_conf_commit() returned an error");
	else
		pass();
	ggz_conf_cleanup();
	if((handle = ggz_conf_parse("cfg_testfile", GGZ_CONF_RDWR)) < 0)
		crit_fail("ggz_conf_parse() after commit failed");
	else
		pass();

	/* Read back the items we just created */
	num = ggz_conf_read_list(handle, "ListTest", "List",
				 &list_items, &list_dst);
	if(list_items == 3) {
		for(i=0; i<3; i++)
			if(strcmp(list_dst[i], list_src[i]))
				i = 5;
		if(i > 4)
			fail("ggz_conf_write_list/read_list() failed");
		else
			pass();
	} else
		fail("ggz_conf_write_list/read_list() failed itm != 3");
	str = ggz_conf_read_string(handle, "StringTest", "String", NULL);
	if(!str || strcmp(str, "A string"))
		fail("ggz_conf_read_string() returned the wrong commit value");
	else
		pass();
	if(str) {
		ggz_free(str);
		str = NULL;
	}
	num = ggz_conf_read_int(handle, "IntTest", "Negative", 0);
	if(num != -84)
		fail("ggz_conf_read_int() returned the wrong commit value");
	else
		pass();

	/* Delete some entries */
	if(ggz_conf_remove_key(handle, "StringTest", "DelString"))
		fail("ggz_conf_remove_key() returned an error");
	else
		pass();
	if(ggz_conf_remove_section(handle, "DeleteMe"))
		fail("ggz_conf_remove_section() returned an error");
	else
		pass();

	ggz_conf_commit(handle);

	ggz_conf_cleanup();
}


void build_testfile(void)
{
	FILE *testfile;
	int i;

	if((testfile = fopen("cfg_testfile", "w")) == NULL)
		crit_fail("Couldn't create 'cfg_testfile'");

	for(i=0; i<NUM_LINES(lines1); i++) {
		fputs(lines1[i], testfile);
		fputc('\n', testfile);
	}

	fclose(testfile);
}


void fail(char *msg)
{
	failures++;
	printf("FAILURE = %s\n", msg);
}


void pass(void)
{
	successes++;
}


void crit_fail(char *msg)
{
	printf("TERMINAL FAILURE = %s\n", msg);
	printf("%d tests succeeded\n%d tests failed\n", successes, failures);
	printf("Retaining 'cfg_testfile' for observation if available.\n");
	printf("Note: Not all tests were executed.\n");
	exit(1);
}
