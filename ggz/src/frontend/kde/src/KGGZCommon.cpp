#include "KGGZCommon.h"

#include <dirent.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// returns a string describing the current state
// taken from ggz-txt
char* KGGZCommon::state(GGZStateID stateid)
{
	char* statestr;

	switch(stateid)
	{
		case GGZ_STATE_OFFLINE:
			statestr = "offline";
			break;
		case GGZ_STATE_CONNECTING:
			statestr = "connecting";
			break;
		case GGZ_STATE_ONLINE:
			statestr = "online";
			break;
		case GGZ_STATE_LOGGING_IN:
			statestr = "logging In";
			break;
		case GGZ_STATE_LOGGED_IN:
			statestr = "logged In";
			break;
		case GGZ_STATE_BETWEEN_ROOMS:
		case GGZ_STATE_ENTERING_ROOM:
			statestr = "in room";
			break;
		case GGZ_STATE_IN_ROOM:
			statestr = "chatting";
			break;
		case GGZ_STATE_LAUNCHING_TABLE:
		case GGZ_STATE_JOINING_TABLE:
			statestr = "on table";
			break;
		case GGZ_STATE_AT_TABLE:
			statestr = "playing";
			break;
		case GGZ_STATE_LEAVING_TABLE:
			statestr = "leaving table";
			break;
		case GGZ_STATE_LOGGING_OUT:
			statestr = "logging out";
			break;
		default:
			statestr = "unknown";
	}

	return statestr;
}


// findprocess:
// find a unix system process by the given name
// returns pid if process already runs, 0 otherwise; and -1 on error
int KGGZCommon::findProcess(char* process)
{
	DIR *d;
	struct dirent *e;
	FILE *f;
	char dir[512];
	int ret;
	char cmd[256];

	d = opendir("/proc/");
	if(d == NULL) return -1;
	ret = 0;
	while((!ret) && (e = readdir(d)))
	{
		strcpy(dir, "/proc/");
		strcat(dir, e->d_name);
		strcat(dir, "/cmdline");
		f = fopen(dir, "r");
		if(f != NULL)
		{
			fscanf(f, "%s", &cmd);
			if(strcmp(cmd, process) == 0) ret = atoi(e->d_name);
			fclose(f);
		}
	}
	closedir(d);
	return ret;
}

// launches the ggzd server
// returns -1 on error, -2 if already running, child pid else
int KGGZCommon::launchProcess(char* process)
{
	int result;
	const char *ggzd = process;
	char *const ggzdarg[] = {process, NULL};

	KGGZDEBUG("Starting process %s...\n", process);
	if(findProcess(process) > 0)
	{
		KGGZDEBUG("Process %s already running!\n", process);
		return -2;
	}
	result = fork();
	if(result == -1) /* external validation */
	{
		KGGZDEBUG("Undefined error!\n");
	}
	else if(result == 0)
	{
		result = execvp(ggzd, ggzdarg);
		KGGZDEBUG("execvp-Result: %s\n", result);
	}
	else
	{
		// parent process; sleep a while to allow server to startup
		while(findProcess(process) == 0)
			sleep(1);
	}
	return result;
}

// tries to kill all running ggzd processes
// sends sigterm first, like the kill(1) command
// returns -1 on failure, 1 on success, and 0 if only a sigkill was successful
int KGGZCommon::killProcess(char* process)
{
	pid_t pid;
	int ret;
	int counter;
	int sigterm;

	pid = 1;
	counter = 0;
	sigterm = 1;
	KGGZDEBUG("Going to kill %s...\n", process);
	while(pid > 0)
	{
		pid = findProcess(process);
		if(pid > 0)
		{
			KGGZDEBUG("Found one (%i) ;)\n", pid);
			ret = kill(pid, SIGTERM);
			KGGZDEBUG("SIGTERM-Ret is: %i\n", ret);
			if(ret != 0)
			{
				KGGZDEBUG("Thou bastard, don't want? I'll show you...\n");
				ret = kill(pid, SIGTERM);
				KGGZDEBUG("SIGKILL-Ret is: %i\n", ret);
				sigterm = 0;
			}
			counter++;
		}
	}
	if(counter) return sigterm;
	return -1;
}

char* KGGZCommon::append(char* string1, char* string2)
{
	char* tmp;

	tmp = (char*)malloc(strlen(string1) + strlen(string2) + 1);
	strcpy(tmp, string1);
	strcat(tmp, string2);

	return tmp;
}

void KGGZCommon::clear()
{
}
