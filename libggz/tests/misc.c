#include <stdio.h>

#include <ggz.h>

#define TESTSTR1 "\"' &quot; >< &&amp;\""
#define TESTSTR2 "This test requires no conversion" 

int main(void)
{
	int errs = 0;
	char *instr, *outstr;

	instr = TESTSTR1;
	outstr = ggz_xml_escape(instr);
	printf("Input string = %s\n", instr);
	printf("Output string = %s\n", outstr);
	instr = ggz_xml_unescape(outstr);
	printf("Back to input string = %s\n", instr);
	if(strcmp(instr, TESTSTR1)) {
		printf("Output doesn't match original!\n");
		errs++;
	}
	ggz_free(instr);
	ggz_free(outstr);

	instr = TESTSTR2;
	outstr = ggz_xml_escape(instr);
	printf("Input string = %s\n", instr);
	printf("Output string = %s\n", outstr);
	if(strcmp(outstr, instr)) {
		printf("Conversion doesn't match original!\n");
		errs++;
	}
	instr = ggz_xml_unescape(outstr);
	printf("Back to input string = %s\n", instr);
	if(strcmp(instr, TESTSTR2)) {
		printf("Output doesn't match original!\n");
		errs++;
	}
	ggz_free(instr);
	ggz_free(outstr);

	ggz_memory_check();

	if(errs == 0)
		printf("All tests successful\n");
	else
		printf("Failed tests = %d\n", errs);
	return errs;
}


