#include <stdio.h>

#include <ggz.h>

#define TESTSTR "\"' &quot; >< &&amp;\""
int main(void) 
{
	int errs = 0;
	char *instr, *outstr;

	instr = TESTSTR;
	outstr = ggz_xml_escape(instr);
	printf("Input string = %s\n", instr);
	printf("Output string = %s\n", outstr);
	instr = ggz_xml_unescape(outstr);
	printf("Back to input string = %s\n", instr);
	if(strcmp(instr, TESTSTR)) {
		printf("Output doesn't match original!\n");
		errs++;
	}
	ggz_free(instr);
	ggz_free(outstr);

	instr = "This test requires no conversion";
	outstr = ggz_xml_escape(instr);
	if(outstr != NULL) {
		printf("Erroneous production from string = %s\n", instr);
		printf("To produce = %s\n", outstr);
		ggz_free(outstr);
		errs++;
	}

	ggz_memory_check();

	if(errs == 0)
		printf("All tests successful\n");
	else
		printf("Failed tests = %d\n", errs);
	return errs;
}
