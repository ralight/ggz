/* Test program for ggz_read_line file functions */
/* NOTE: If the buffer size tuning defines in misc.c are altered */
/*       this program will no longer test properly.              */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ggz.h>

#include "misc.h" /* We cheat. */

#define TESTFILE "/tmp/libggz_test.read_line"

void build_testfile(void);
void check_testfile(void);
char * build_line(int len);

int failures=0;
int read_lines=0;
int total_lines=0;
int buffer_expansions=0;
int bufsize;

int main(int argc, char *argv[])
{
	char *check, *check_buf;

	build_testfile();
	check_testfile();

	if(read_lines == total_lines)
		check = "ok";
	else {
		failures += (total_lines - read_lines);
		check = "FAIL";
	}
	if(buffer_expansions == 3 && bufsize == 131072)
		check_buf = "ok";
	else {
		failures++;
		check_buf = "FAIL";
	}
	printf("The buffer expanded %d times to %d bytes (%s)\n",
		buffer_expansions, bufsize, check_buf);
	printf("Total test lines = %d (%s)\nNumber of failures = %d\n",
		read_lines, check, failures);
	if(failures == 0)
		printf("Test passed\n");
	else
		printf("Test failed\n");

	unlink(TESTFILE);

	return failures;
}


void build_testfile(void)
{
	FILE *f;
	char *line;
	int i;

	if((f = fopen(TESTFILE, "w")) == NULL) {
		printf("Unable to open test file for writing\n");
		exit(1);
	}

	/* These lines trigger a buffer fill in midline (BS=1024) */
	line = build_line(99);
	for(i=0; i<10; i++)
		fprintf(f, "%s\n", line);
	ggz_free(line);
	total_lines += 10;
	/* BR=24 */
	line = build_line(47);
	fprintf(f, "%s\n", line);	/* Should trigger left shift fill */
	ggz_free(line);
	total_lines++;

	/* Now we want to trigger a buffer fill at start of line */
	/* BS=1024, BR=976 */
	line = build_line(60);
	for(i=0; i<16; i++)	/* 61 * 16 = 976 */
		fprintf(f, "%s\n", line);
	ggz_free(line);
	total_lines += 16;

	/* Now we want to trigger a buffer fill at start of line */
	/* BR=0 */
	line = build_line(1023);	/* Fills buffer perfectly */
	fprintf(f, "%s\n", line);
	ggz_free(line);
	total_lines++;

	/* This will trigger a buffer expansion */
	/* BR=0 */
	line = build_line(23);
	fprintf(f, "%s\n", line);	/* Fill triggered */
	ggz_free(line);
	total_lines++;
	/* BR = 1000 */
	line = build_line(1499);	/* Two buffer fills, first left shift */
	fprintf(f, "%s\n", line);	/* then an expansion */
	ggz_free(line);
	total_lines++;

	/* Now trigger a left shift with new bufsize */
	/* BS=1536, BR=36 */
	line = build_line(71);
	fprintf(f, "%s\n", line);
	ggz_free(line);
	total_lines++;

	/* Fill the buffer perfectly again test */
	/* BR=1500 */
	line = build_line(149);
	for(i=0; i<10; i++)
		fprintf(f, "%s\n", line);
	ggz_free(line);
	total_lines += 10;

	/* Make a really big line on an empty buffer */
	/* BR = 0 */
	line = build_line(2499);
	fprintf(f, "%s\n", line);	/* Cause 2 buffer expansions */
	ggz_free(line);			/* NOTE: But we'll only see one */
	total_lines++;

	/* Finally a ridiculously long line */
	/* BS=2560, BR=60 */
	line = build_line(130999);	/* Cause (a lot) of buffer expansions */
	fprintf(f, "%s\n", line);	/* Again, we only see one */
	ggz_free(line);
	total_lines++;

	/* BS=131072, BR=72 */

	fclose(f);
}


char *build_line(int len)
{
	char *new;
	int i;

	/* The line is a psuedorandom uppercase string based on 'len' */
	srandom(len);
	new = ggz_malloc(len+1);
	for(i=0; i<len; i++)
		new[i] = (random() % 26) + 'A';
	new[i] = '\0';
	return new;
}


void check_testfile(void)
{
	int fdes, len;
	GGZFile *fstruct;
	char *line_in, *line_check;

	if((fdes = open(TESTFILE, O_RDONLY)) == -1) {
		printf("Unable to open test file for reading\n");
		exit(1);
	}

	fstruct = ggz_get_file_struct(fdes);
	bufsize = fstruct->bufsize;

	while((line_in = ggz_read_line(fstruct)) != NULL) {
		len = strlen(line_in);
		line_check = build_line(len);
		printf("Read %d bytes... ", len);
		read_lines++;
		if(strcmp(line_in, line_check)) {
			printf("FAIL");
			failures++;
		} else
			printf("ok");
		if(bufsize != fstruct->bufsize) {
			bufsize = fstruct->bufsize;
			printf(" (buffer expansion, new size = %d)\n", bufsize);
			buffer_expansions++;
		} else
			printf("\n");
		ggz_free(line_in);
		ggz_free(line_check);
	}

	close(fdes);
	ggz_free_file_struct(fstruct);
}
