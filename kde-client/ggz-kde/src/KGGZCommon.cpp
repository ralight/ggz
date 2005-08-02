/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZCommon: Contains macros, definitions, constants and versioning information. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZCommon.h"

// System includes
#include <dirent.h>
#include <stdlib.h>
#ifdef __STRICT_ANSI__
#define __USE_POSIX
#endif
#include <signal.h>
#include <string.h>
#ifdef __STRICT_ANSI__
#define __USE_XOPEN
#endif
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

// returns a string describing the current state
// taken from ggz-txt
const char* KGGZCommon::state(GGZStateID stateid)
{
	const char* statestr;

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
// new - taken over from Paralog project (old function is now obsolete)
int KGGZCommon::findProcess(const char *process)
{
    DIR *d;
    struct dirent *e;
    FILE *f;
    char dir[512];
    int ret;
    char stat[128];
    char bprocess[256];
    char pids[32];
    int pid;

	KGGZDEBUG("findProcess: %s\n", process); 
    d = opendir("/proc/");
    if(d == NULL) return -1;
    ret = 0;
    sprintf(bprocess, "(%s)", process);
    while((!ret) && (e = readdir(d)))
    {
        strcpy(dir, "/proc/");
        strcat(dir, e->d_name);
        strcat(dir, "/stat");
        f = fopen(dir, "r");
        if(f != NULL)
        {
            fscanf(f, "%s %s", pids, stat);
            pid = atoi(pids);
            if((strcmp(stat, bprocess) == 0) && (pid != getpid())) ret = atoi(e->d_name);
            fclose(f);
        }
    }
    closedir(d);
    return ret;
}

// launches the ggzd server
// returns -1 on error, -2 if already running, child pid else
int KGGZCommon::launchProcess(const char* process, char* processpath)
{
	int result;
	char *const ggzdarg[] = {(char*)process, NULL};
	int counter;

	KGGZDEBUG("Starting process %s...\n", process);
	if(findProcess(process) > 0) return -2;
	result = fork();
	if(result < 0) /* external validation */
	{
		KGGZDEBUG("Undefined error!\n");
		return -1;
	}
	else if(result == 0)
	{
		result = execv(processpath, ggzdarg);
		KGGZDEBUG("execv result: %i\n", result);
		exit(-1);
	}
	else
	{
		// parent process; sleep a while to allow server to startup
		KGGZDEBUG("We did it! %s (%s) has been launched!!\n", process, processpath);
		counter = 0;
		while(findProcess(process) == 0)
		{
			if(counter == 2)
			{
				KGGZDEBUG("Unable to start server!\n");
				return -1;
			}
			counter++;
			KGGZDEBUG("Server not running yet - must sleep...\n");
			sleep(1);
		}
		KGGZDEBUG("Ok.\n");
	}
	return result;
}

// tries to kill all running ggzd processes
// sends sigterm first, like the kill(1) command
// returns < 0 on failure, 1 on success, and 0 if only a sigkill was successful
int KGGZCommon::killProcess(const char* process)
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
				if(ret == -1)
				{
					if(errno == EPERM) return -2;
					if(errno == ESRCH) return -3;
					return -1;
				}
			}
			counter++;
		}
	}
	if(counter) return sigterm;
	return -3;
}

int KGGZCommon::kggzdebugdummy(const char *x, ...)
{
	return 0;
}

