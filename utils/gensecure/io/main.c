/* Test non-blocking input while displaying annoying stuff */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char buffer[12];
	int num;
	int i;

	i = 0;
	fcntl(0, F_SETFL, O_NONBLOCK);
	for(;;)
	{
		num = read(0, buffer, sizeof(buffer));
		if(num > 0)
		{
			buffer[num - 1] = 0;
			printf("got: %s\n", buffer);
			if(!strcasecmp(buffer, "quit")) exit(0);
		}
		else if (num < 0)
		{
			if(errno != EAGAIN)
			{
				printf("ERROR!\n");
				exit(-1);
			}
		}
		i++;
		if(!(i % 500000)) printf("--wait\n");
	}
	return 0;
}

