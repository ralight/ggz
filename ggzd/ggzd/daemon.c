/* 
 *  daemon.c : Handle daemon initialization
 *  Copyright (C) 1996 Thomas Koenig
 *
 *  Modified 6/24/00 by Brent Hendricks for use with the GGZ project.
 *  $Id: daemon.c 10067 2008-06-24 22:01:07Z jdorje $
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "daemon.h"
#include "err_func.h"


static GGZReturn lock_fd(int fd)
{
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	if (fcntl(fd, F_SETLK, &lock) < 0) {
		ggz_error_sys("failed fcntl call");
		return GGZ_ERROR;
	} else {
		return GGZ_OK;
	}
}


void daemon_init(void)
{
	pid_t pid;
	mode_t old_umask;
	int fd;
	FILE *fp;
	const char pid_filename[] = "ggzd.pid";
	char pid_file[strlen(opt.data_dir) + strlen(pid_filename) + 2];

	if ((pid = fork()) < 0)
		ggz_error_sys_exit("fork failed");
	else if (pid != 0)
		exit(0);

	setsid();
	/* Should chdir() to gamedir?*/
        /*chdir("/");*/
	umask(0);
        close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Set up standard daemon environment */
	old_umask = umask(S_IWGRP | S_IWOTH);

	/* Form absolute path of pid file */
	snprintf(pid_file, sizeof(pid_file),
		 "%s/%s", opt.data_dir, pid_filename);

	if ( (fd = open(pid_file, O_RDWR | O_CREAT | O_EXCL, 
			S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH)) == -1) {
		
		if (errno != EEXIST)
			ggz_error_sys_exit("Cannot open %s", pid_file);
		
		if ( (fd = open(pid_file, O_RDWR)) < 0)
			ggz_error_sys_exit("Cannot open %s", pid_file);
		
		if ( (fp = fdopen(fd, "rw")) == NULL)
			ggz_error_sys_exit("Cannot open %s for reading", pid_file);

		pid = -1;
		if ((fscanf(fp, "%d", &pid) != 1) || (pid == getpid())
		    || (lock_fd(fileno(fp)) == GGZ_OK)) {
				
			log_msg(GGZ_LOG_NOTICE, "Removing stale lockfile for pid %d", pid);
			if (unlink(pid_file) == -1)
				ggz_error_sys_exit("Cannot unlink %s", pid_file);

		} else 
			ggz_error_sys_exit("Another ggzd already running with pid %d!", pid);
		
		fclose(fp);
		unlink(pid_file);
		fd = open(pid_file, O_RDWR | O_CREAT | O_EXCL,
			  S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
				
		if (fd == -1)
			ggz_error_sys_exit("Cannot open %s the second time round", pid_file);
	}

	if (lock_fd(fd) == GGZ_ERROR)
		ggz_error_sys_exit("Cannot lock %s", pid_file);
	
	if ( (fp = fdopen(fd, "w")) == NULL)
		ggz_error_sys_exit("Special weirdness: fdopen failed");
	
	fprintf(fp, "%d\n", getpid());
	
	/* 
	 *  We do NOT close fd, since we want to keep the
	 *  lock. However, we don't want to keep the file descriptor
	 *  in case of an exec().  
	 */
	fflush(fp);
	fcntl(fd, F_SETFD, (long)1);

	return;
}


void daemon_cleanup()
{
	const char pid_filename[] = "ggzd.pid";
	char pid_file[strlen(opt.data_dir) + strlen(pid_filename) + 2];

	/* Form absolute path of pid file */
	snprintf(pid_file, sizeof(pid_file), "%s/%s",
		 opt.data_dir, pid_filename);
	
	unlink(pid_file);
}
