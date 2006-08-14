#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct safestring
{
	char *s;
	int len;
	char ***shallow;
};
typedef struct safestring safestring;

#define SS_INLINE inline

SS_INLINE safestring ss(const char *str)
{
	safestring s;
	if(str)
	{
		s.s = strdup(str);
		s.len = strlen(str);
		if(!s.s)
		{
			/* OOM error handling here */
		}
	}
	else
	{
		s.s = NULL;
		s.len = 0;
	}
	s.shallow = NULL;
	return s;
}

SS_INLINE void ss_destroy(safestring *s)
{
	if(!s) return;
	free(s->s);
	s->s = NULL;
	s->len = 0;
	if(s->shallow)
	{
		for(int i = 0; s->shallow[i]; i++)
		{
			*(s->shallow[i]) = NULL;
		}
		free(s->shallow);
		s->shallow = NULL;
	}
}

SS_INLINE void ss_append_compat(safestring *s, safestring s2)
{
	char *m;
	m = realloc(s->s, s->len + s2.len + 1);
	if(!m)
	{
		/* OOM error handling here */
		return;
	}
	s->s = m;
	if(!s->len) s->s[0] = '\0';
	strcat(s->s, s2.s);
	s->len += s2.len;
}

SS_INLINE void ss_append(safestring *s, safestring s2)
{
	char *m;
	m = realloc(s->s, s->len + s2.len + 1);
	if(!m)
	{
		/* OOM error handling here */
		return;
	}
	s->s = m;
	strncpy(s->s + s->len, s2.s, s2.len);
	s->len += s2.len;
	s->s[s->len] = '\0';
}

#define ss_copy(x) ss(x.s)
#define ss_empty() ss(NULL)

SS_INLINE safestring ss_shallowcopy(safestring *s)
{
	safestring s2;
	s2.s = s->s;
	s2.len = s->len;
	s2.shallow = malloc(sizeof(char**) * 2);
	s2.shallow[0] = &s->s;
	s2.shallow[1] = NULL;
	int pos = 0;
	if(s->shallow)
	{
		while(s->shallow[pos]) pos++;
	}
	s->shallow = realloc(s->shallow, sizeof(char**) * (pos + 2));
	s->shallow[pos] = &s2.s;
	s->shallow[pos + 1] = NULL;
	return s2;
}

static void benchmark(void)
{
	int i;
	time_t starttime, endtime;
	const int iterations = 20000;

	safestring s = ss_empty();
	safestring s2 = ss("Some string which will be appended repeatedly.");

	starttime = time(NULL);
	printf("== start %li\n", starttime);
	for(i = 0; i < iterations; i++)
	{
		ss_append_compat(&s, s2);
	}
	endtime = time(NULL);
	printf("== end %li\n", endtime);
	printf("Total string length: %i bytes\n", s.len);
	printf("Time needed for %i iterations: %li seconds\n", iterations, endtime - starttime);

	ss_destroy(&s);

	starttime = time(NULL);
	printf("== start %li (fast version)\n", starttime);
	for(i = 0; i < iterations; i++)
	{
		ss_append(&s, s2);
	}
	endtime = time(NULL);
	printf("== end %li\n", endtime);
	printf("Total string length: %i bytes\n", s.len);
	printf("Time needed for %i iterations: %li seconds\n", iterations, endtime - starttime);
}

int main()
{
	safestring s = ss("This is a very safe string.");

	printf("Safe string is: %s\n", s.s);

	ss_destroy(&s);

	printf("Safe string is after destruction: %s\n", s.s);

	safestring s2 = ss("Appendable");

	ss_append(&s, s2);

	printf("New safe string is: %s\n", s.s);

	safestring s3 = ss_copy(s);

	printf("Copy of safe string is: %s\n", s3.s);

	safestring s4 = ss_shallowcopy(&s);

	printf("Shallow copy of safe string is: %s\n", s4.s);

	ss_destroy(&s4);

	printf("Original is after destruction of shallow copy: %s\n", s.s);

	benchmark();

	return 0;
}

