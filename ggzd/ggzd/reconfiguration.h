#ifndef _GGZ_RECONFIGURATION_H
#define _GGZ_RECONFIGURATION_H

#ifdef HAVE_INOTIFY

/* mostly stolen from KIO's kdirwatch.cpp */

#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <linux/inotify.h>

static inline int inotify_init(void)
{
	return syscall(__NR_inotify_init);
}

static inline int inotify_add_watch(int fd, const char *name, __u32 mask)
{
	return syscall(__NR_inotify_add_watch, fd, name, mask);
}

static inline int inotify_rm_watch(int fd, __u32 wd)
{
	return syscall(__NR_inotify_rm_watch, fd, wd);
}

/*
#ifndef IN_ONLYDIR
#define IN_ONLYDIR 0x01000000
#endif

#ifndef IN_DONT_FOLLOW
#define IN_DONT_FOLLOW 0x02000000
#endif

#ifndef IN_MOVE_SELF
#define IN_MOVE_SELF 0x00000800
#endif
*/

#endif

#endif

