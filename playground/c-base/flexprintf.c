#define _GNU_SOURCE /* for asprintf */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define DEBUG(x)

typedef char* (*flexprintf_format)(const char *pattern, ...);

struct flexformat
{
	char **pattern;
	flexprintf_format *func;
	int num;
};
typedef struct flexformat flexformat;

static flexformat format;

void flexprintf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int i, j, k;
	int start, end;
	char *tmp;
	int length;
	char *result;
	int rlength;

	length = strlen(fmt);
	result = NULL;
	rlength = 0;

	for(i = 0; i < length; i++)
	{
		if((fmt[i] == '%') && (fmt[i + 1] != 'Q') && (i < length - 1))
		{
			char c;
			char *res;

			c = fmt[i + 1];
			i += 1;
			res = NULL;

			if(c == 's')
			{
				char *arg = va_arg(ap, char*);
				DEBUG(printf("string: %s!\n", arg);)
				res = strdup(arg);
			}
			else if(c == 'i')
			{
				int arg = va_arg(ap, int);
				DEBUG(printf("integer: %i!\n", arg);)
				asprintf(&res, "%i", arg);
			}
			else
			{
				printf("Ooops! Unknown conversion '%c'.\n", c);
			}

			if(res)
			{
				result = realloc(result, rlength + 1 + strlen(res));
				strncpy(result + rlength, res, strlen(res));
				result[rlength + strlen(res)] = '\0';
				rlength += strlen(res);

				free(res);
			}
		}
		else if((fmt[i] == '%') && (fmt[i + 1] == 'Q') && (i < length - 3))
		{
			start = i + 2;
			if(fmt[start] == '[')
			{
				for(j = start + 1; fmt[j]; j++)
				{
					if(fmt[j] == ']')
					{
						end = j;
						DEBUG(printf("* range: %i-%i\n", start, end);)
						tmp = malloc(end - start);
						strncpy(tmp, fmt + start + 1, end - start - 1);
						tmp[end - start - 1] = '\0';
						DEBUG(printf("* range argument: %s\n", tmp);)
						for(k = 0; k < format.num; k++)
						{
							if(!strcmp(format.pattern[k], tmp))
							{
								DEBUG(printf("* found pattern!\n");)
								int xarg = va_arg(ap, int);
								char *xres = format.func[k](format.pattern[k], xarg);
								DEBUG(printf("* pattern yielded: %s\n", xres);)

								result = realloc(result, rlength + 1 + strlen(xres));
								strncpy(result + rlength, xres, strlen(xres));
								result[rlength + strlen(xres)] = '\0';
								rlength += strlen(xres);

								free(xres);
								break;
							}
						}
						i = j;
						break;
					}
				}
			}
		}
		else
		{
			result = realloc(result, rlength + 2);
			result[rlength] = fmt[i];
			result[rlength + 1] = '\0';
			rlength += 1;
		}
	}

	va_end(ap);

	printf("%s", result);
}

void flexprintf_init()
{
	format.pattern = NULL;
	format.func = NULL;
	format.num = 0;
}

void flexprintf_pattern(const char *pattern, flexprintf_format func)
{
	format.num += 1;
	format.pattern = realloc(format.pattern, format.num * sizeof(char*));
	format.func = realloc(format.func, format.num * sizeof(flexprintf_format));
	format.pattern[format.num - 1] = strdup(pattern);
	format.func[format.num - 1] = func;
}

/* === End of flexprintf library code === */

struct seat
{
	int num;
	char *name;
};

char *format_seat(const char *pattern, ...)
{
	va_list list;
	struct seat s;

	va_start(list, pattern);
	s = va_arg(list, struct seat);
	va_end(list);

	char *x = malloc(50);
	snprintf(x, 50, "|%i:%s|", s.num, s.name);
	return x;
}

int main()
{
	flexprintf_init();
	flexprintf_pattern("seat", format_seat);

	struct seat seat;
	seat.num = 12;
	seat.name = "player";

	printf("Seat: %s\n", format_seat("seat", seat));

	flexprintf("Hello %s. This is a seat: %Q[seat].\n", "World", seat);

	return 0;
}

