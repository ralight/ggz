/* Include files */
#include "ggzcommgen.h"
#include "minidom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global buffers */
char *filedecl = NULL;
char *fileimpl = NULL;
char *fileimpl2 = NULL;
char *fileimpl3 = NULL;
char *fileimpl4 = NULL;
int counter = 0;

/* Function prototypes */
void ggzcommgen_source(DOM *dom, const char *task, const char *format);
void ggzcommgen_push(char **buffer, const char *arg);
void ggzcommgen_header(const char *protocol, const char *format);
void ggzcommgen_footer(const char *format);
void ggzcommgen_output(const char *protocol, const char *format);

/* File names */
char *filenames[3][3] =
{
	{"c++", "cpp", "h"},
	{"c", "c", "h"},
	{NULL, NULL, NULL}
};

/* Generate the protocol source implementation */
void ggzcommgen_generate(const char *protocol, const char *task, const char *format, const char *library, const char *mode)
{
	DOM *dom;
	char filename[1024];

	/* Load the protocol into memory */
	snprintf(filename, sizeof(filename), "%s.protocol", protocol);
	dom = minidom_load(filename);
	if(!dom)
	{
		fprintf(stderr, "Couldn't load protocol file '%s.protocol'\n", protocol);
		exit(-1);
	}

	/* Do some checks here... */
	/*minidom_dump(dom);*/

	/* Some comments */
	ggzcommgen_push(&filedecl, "/* Generated via GGZCommGen */\n\n");
	ggzcommgen_push(&fileimpl, "/* Generated via GGZCommGen */\n\n");

	/* Build up source buffers */
	if(!strcmp(format, "c"))
	{
		ggzcommgen_header(protocol, format);
		ggzcommgen_source(dom, task, format);
		ggzcommgen_footer(format);
	}
	else if(!strcmp(format, "c++"))
	{
		ggzcommgen_header(protocol, format);
		ggzcommgen_source(dom, task, format);
		ggzcommgen_footer(format);
	}
	else
	{
		fprintf(stderr, "Unsupported format: %s\n", format);
		exit(-1);
	}
	
	/* Produce the sources */
	ggzcommgen_output(protocol, format);

	/* Clean up again */
	minidom_free(dom);
}

/* Helper function: append two strings */
void ggzcommgen_push(char **buffer, const char *arg)
{
	int len, alen;

	len = (*buffer ? strlen(*buffer) : 0);
	alen = (arg ? strlen(arg) : 0);
	*buffer = (char*)realloc(*buffer, len + alen + 1);
	memcpy((*buffer) + len, arg, alen);
	(*buffer)[len + alen] = 0;
}

/* Generate body of source file */
void ggzcommgen_source(DOM *dom, const char *task, const char *format)
{
	ELE *ele, *etask, *eevent, *edata;
	int i, j, k;
	char *eventname;
	char *dataname, *datatype;
	char *bufdecl, *bufimpl, *bufimpl2;
	int my;
	char scounter[64];

	ele = dom->el;
	if(!ele) return;

	for(i = 0; ele->el && ele->el[i]; i++)
	{
		if(!strcmp(ele->el[i]->name, task))
		{
			my = 1;
		}
		else
		{
			my = 0;
		}

		etask = ele->el[i];
		for(j = 0; etask->el && etask->el[j]; j++)
		{
			if(!strcmp(etask->el[j]->name, "event"))
			{
				eevent = etask->el[j];
				eventname = MD_att(eevent, "name");
				if(!eventname)
				{
					fprintf(stderr, "Event without name detected\n");
					exit(-1);
				}

				snprintf(scounter, sizeof(scounter), "%i", ++counter);
				ggzcommgen_push(&fileimpl, "#define ");
				ggzcommgen_push(&fileimpl, eventname);
				ggzcommgen_push(&fileimpl, " ");
				ggzcommgen_push(&fileimpl, scounter);
				ggzcommgen_push(&fileimpl, "\n");

				bufdecl = NULL;
				bufimpl = NULL;
				bufimpl2 = NULL;
				if(my)
				{
					ggzcommgen_push(&bufdecl, eventname);
					ggzcommgen_push(&bufdecl, "(");
					ggzcommgen_push(&bufimpl, "\tif((ggz_write_char(fd, ");
					ggzcommgen_push(&bufimpl, eventname);
					ggzcommgen_push(&bufimpl, ") < 0)");
				}
				else
				{
					ggzcommgen_push(&fileimpl2, "\t\tcase ");
					ggzcommgen_push(&fileimpl2, eventname);
					ggzcommgen_push(&fileimpl2, ":\n");
					ggzcommgen_push(&bufimpl2, "\t\t\tif(");
				}

				for(k = 0; eevent->el && eevent->el[k]; k++)
				{
					if(!strcmp(eevent->el[k]->name, "data"))
					{
						edata = eevent->el[k];
						dataname = MD_att(edata, "name");
						datatype = MD_att(edata, "type");
						if((!dataname) || (!datatype))
						{
							fprintf(stderr, "Data without name or type detected\n");
							exit(-1);
						}

						if(k) ggzcommgen_push(&bufdecl, ", ");
						ggzcommgen_push(&bufimpl, " || ");
						if(k) ggzcommgen_push(&bufimpl2, " || ");

						if(!strcmp(datatype, "int"))
						{
							if(my)
							{
								ggzcommgen_push(&bufdecl, "int ");
								ggzcommgen_push(&bufimpl, "(ggz_write_int(fd, ");
							}
							else
							{
								ggzcommgen_push(&bufimpl2, "(ggz_read_int(fd, &");
								ggzcommgen_push(&fileimpl3, "\tint ");
							}
						}
						else if(!strcmp(datatype, "byte"))
						{
							if(my)
							{
								ggzcommgen_push(&bufdecl, "char ");
								ggzcommgen_push(&bufimpl, "(ggz_write_char(fd, ");
							}
							else
							{
								ggzcommgen_push(&bufimpl2, "(ggz_read_char(fd, &");
								ggzcommgen_push(&fileimpl3, "\tchar ");
							}
						}
						else if(!strcmp(datatype, "string"))
						{
							if(my)
							{
								ggzcommgen_push(&bufdecl, "char* ");
								ggzcommgen_push(&bufimpl, "(ggz_write_string(fd, ");
							}
							else
							{
								ggzcommgen_push(&bufimpl2, "(ggz_read_string(fd, &");
								ggzcommgen_push(&fileimpl3, "\tchar* ");
							}
						}
						else
						{
							fprintf(stderr, "Unknown data type: %s\n", datatype);
							exit(-1);
						}

						if(my)
						{
							ggzcommgen_push(&bufdecl, dataname);
							ggzcommgen_push(&bufimpl, dataname);
							ggzcommgen_push(&bufimpl, ") < 0)");
						}
						else
						{
							ggzcommgen_push(&bufimpl2, dataname);
							ggzcommgen_push(&bufimpl2, ")");
							ggzcommgen_push(&fileimpl3, dataname);
							ggzcommgen_push(&fileimpl3, ";\n");
						}
					}
				}

				if(my)
				{
					ggzcommgen_push(&bufdecl, ")");
					ggzcommgen_push(&bufimpl, ")\n\t{\n\t\t");
					if(!strcmp(format, "c"))
					{
						ggzcommgen_push(&bufimpl, "ggzcomm_");
					}
					ggzcommgen_push(&bufimpl, "error();\n\t}");
					if(!strcmp(format, "c++"))
					{
						ggzcommgen_push(&filedecl, "\t\tvoid ");
					}
					else
					{
						ggzcommgen_push(&filedecl, "void ggzcomm_");
					}
					ggzcommgen_push(&filedecl, bufdecl);
					ggzcommgen_push(&filedecl, ";\n");

					ggzcommgen_push(&fileimpl4, "void ");
					if(!strcmp(format, "c++"))
					{
						ggzcommgen_push(&fileimpl4, "GGZComm::");
					}
					else
					{
						ggzcommgen_push(&fileimpl4, "ggzcomm_");
					}
					ggzcommgen_push(&fileimpl4, bufdecl);
					ggzcommgen_push(&fileimpl4, "\n{\n");
					ggzcommgen_push(&fileimpl4, bufimpl);
					ggzcommgen_push(&fileimpl4, "\n}\n\n");
					free(bufdecl);
					free(bufimpl);
					bufdecl = NULL;
					bufimpl = NULL;
				}
				else
				{
					if(k)
					{
						ggzcommgen_push(&fileimpl2, bufimpl2);
						ggzcommgen_push(&fileimpl2, ") < 0))\n\t\t\t{\n\t\t\t\t");
						if(!strcmp(format, "c"))
						{
							ggzcommgen_push(&fileimpl2, "ggzcomm_");
						}
						ggzcommgen_push(&fileimpl2, "error();\n\t\t\t}\n");
					}
					else
					{
					}
					ggzcommgen_push(&fileimpl2, "\t\t\tbreak;\n");
					free(bufimpl2);
					bufimpl2 = NULL;
				}
			}
		}
	}
}

/* Generate the file headers */
void ggzcommgen_header(const char *protocol, const char *format)
{
	ggzcommgen_push(&filedecl, "#ifndef GGZCOMM_H\n");
	ggzcommgen_push(&filedecl, "#define GGZCOMM_H\n\n");
	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&filedecl, "class GGZComm\n{\n");
		ggzcommgen_push(&filedecl, "\tpublic:\n");
		ggzcommgen_push(&filedecl, "\t\tGGZComm();\n");
		ggzcommgen_push(&filedecl, "\tprotected:\n");
	}

	ggzcommgen_push(&fileimpl, "#include \"");
	ggzcommgen_push(&fileimpl, protocol);
	ggzcommgen_push(&fileimpl, "_generated.h\"\n");
	ggzcommgen_push(&fileimpl, "#include <stdlib.h>\n");
	ggzcommgen_push(&fileimpl, "#include <ggz.h>\n\n");
	ggzcommgen_push(&fileimpl, "int fd = 3;\n\n");

	ggzcommgen_push(&fileimpl3, "void ");
	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&fileimpl3, "GGZComm::");
	}
	else
	{
		ggzcommgen_push(&fileimpl3, "ggzcomm_");
	}
	ggzcommgen_push(&fileimpl3, "input()\n{\n");
	ggzcommgen_push(&fileimpl3, "\tchar opcode;\n");

	ggzcommgen_push(&fileimpl2, "\tif(ggz_read_char(fd, &opcode) < 0)\n\t{\n\t\t");
	if(!strcmp(format, "c"))
	{
		ggzcommgen_push(&fileimpl2, "ggzcomm_");
	}
	ggzcommgen_push(&fileimpl2, "error();\n\t}\n");
	ggzcommgen_push(&fileimpl2, "\tggzcomm_chain(opcode);\n\n");
	ggzcommgen_push(&fileimpl2, "\tswitch(opcode)\n\t{\n");
}

/* Generate the file footers */
void ggzcommgen_footer(const char *format)
{
	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&filedecl, "\t\tvoid ");
	}
	else
	{
		ggzcommgen_push(&filedecl, "void ggzcomm_");
	}
	ggzcommgen_push(&filedecl, "chain(int opcode);\n");

	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&filedecl, "\t\tvoid ");
	}
	else
	{
		ggzcommgen_push(&filedecl, "void ggzcomm_");
	}
	ggzcommgen_push(&filedecl, "error();\n");

	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&filedecl, "};\n");
	}
	ggzcommgen_push(&filedecl, "\n#endif\n");

	ggzcommgen_push(&fileimpl2, "\n\t}\n}\n\n");
	ggzcommgen_push(&fileimpl2, "void ");
	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&fileimpl2, "GGZComm::");
	}
	else
	{
		ggzcommgen_push(&fileimpl2, "ggzcomm_");
	}
	ggzcommgen_push(&fileimpl2, "chain(int opcode)\n{\n");
	ggzcommgen_push(&fileimpl2, "}\n\n");

	ggzcommgen_push(&fileimpl2, "void ");
	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&fileimpl2, "GGZComm::");
	}
	else
	{
		ggzcommgen_push(&fileimpl2, "ggzcomm_");
	}
	ggzcommgen_push(&fileimpl2, "error()\n{\n");
	ggzcommgen_push(&fileimpl2, "\texit(-1);\n");
	ggzcommgen_push(&fileimpl2, "}\n");

	if(!strcmp(format, "c++"))
	{
		ggzcommgen_push(&fileimpl2, "\nGGZComm::GGZComm()\n{\n}\n");
	}
}

/* Output all generated data */
void ggzcommgen_output(const char *protocol, const char *format)
{
	FILE *f;
	char filename[1024];
	char *impl, *decl;
	int i;

	impl = NULL;
	decl = NULL;
	for(i = 0; filenames[i][0]; i++)
	{
		if(!strcmp(format, filenames[i][0]))
		{
			impl = filenames[i][1];
			decl = filenames[i][2];
		}
	}
	if((!impl) || (!decl))
	{
		fprintf(stderr, "No file name extensions found for '%s'\n", format);
		exit(-1);
	}

	snprintf(filename, sizeof(filename), "%s_generated.%s", protocol, decl);
	f = fopen(filename, "w");
	if(!f)
	{
		fprintf(stderr, "Cannot write to file '%s'\n", filename);
		exit(-1);
	}

	/*printf("--- Declaration: ---\n");*/
	fprintf(f, "%s\n", filedecl);
	fclose(f);

	snprintf(filename, sizeof(filename), "%s_generated.%s", protocol, impl);
	f = fopen(filename, "w");
	if(!f)
	{
		fprintf(stderr, "Cannot write to file '%s'\n", filename);
		exit(-1);
	}

	/*printf("--- Implementation: ---\n");*/
	fprintf(f, "%s\n", fileimpl);
	fprintf(f, "%s\n", fileimpl4);
	fprintf(f, "%s\n", fileimpl3);
	fprintf(f, "%s\n", fileimpl2);
	fclose(f);

	fflush(NULL);

	free(filedecl);
	free(fileimpl);
	free(fileimpl2);
	free(fileimpl3);
	filedecl = NULL;
	fileimpl = NULL;
	fileimpl2 = NULL;
	fileimpl3 = NULL;
}

