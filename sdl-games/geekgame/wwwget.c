/* wget http://mindx.dyndns.org/homepage/photos/josef.png -q -O josef.png */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void wwwget_internal(const char *source, const char *dest)
{
	const char *program = "wget";
	char *const args[] =
	{
		strdup(program),
		strdup(source),
		"-q",
		"-O",
		strdup(dest),
		NULL
	};

	execvp(program, args);
}

void wwwcopy_internal(const char *source, const char *dest)
{
	const char *program = "cp";
	char *const args[] =
	{
		strdup(program),
		strdup(source),
		strdup(dest),
		NULL
	};

	execvp(program, args);
}

void wwwget(const char *source, const char *dest)
{
	pid_t pid;
	int status;

	pid = fork();
	switch(pid)
	{
		case -1:
			return;
		case 0:
			if(source[0] != '/')
			{
				wwwget_internal(source, dest);
			}
			else
			{
				wwwcopy_internal(source, dest);
			}
			_exit(0);
	}

	waitpid(pid, &status, 0);
}

/*
int main(int argc, char *argv[])
{
	wwwget("http://mindx.dyndns.org/homepage/photos/josef.png", "joseflocal.png");
	return 0;
}
*/
