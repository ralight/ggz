#ifndef _GGZ_RECONFIGURATION_H
#define _GGZ_RECONFIGURATION_H

#ifdef HAVE_INOTIFY

#include <linux/inotify.h>

/*#define IN_CREATE      0x00000100*/
#define IN_DELETE      0x00000200
#define IN_CLOSE_WRITE 0x00000008

#define INOTIFY_EVENTSIZE sizeof(struct inotify_event)

#define __u32 unsigned int

/* now mostly stolen from the Linux::Inotify2 perl module */
/* this should eventually be available from glibc but isn't yet */

#include <sys/syscall.h>

#if defined(__i386__)
# define __NR_inotify_init	291
# define __NR_inotify_add_watch	292
# define __NR_inotify_rm_watch	293
#elif defined(__x86_64__)
# define __NR_inotify_init	253
# define __NR_inotify_add_watch	254
# define __NR_inotify_rm_watch	255
#elif defined(__powerpc__) || defined(__powerpc64__)
# define __NR_inotify_init	275
# define __NR_inotify_add_watch	276
# define __NR_inotify_rm_watch	277
#elif defined (__ia64__)
# define __NR_inotify_init	1277
# define __NR_inotify_add_watch	1278
# define __NR_inotify_rm_watch	1279
#elif defined (__s390__)
# define __NR_inotify_init	284
# define __NR_inotify_add_watch	285
# define __NR_inotify_rm_watch	286
#elif defined (__alpha__)
# define __NR_inotify_init	444
# define __NR_inotify_add_watch	445
# define __NR_inotify_rm_watch	446
#elif defined (__sparc__) || defined (__sparc64__)
# define __NR_inotify_init	151
# define __NR_inotify_add_watch	152
# define __NR_inotify_rm_watch	156
#elif defined (__arm__)
# define __NR_inotify_init	316
# define __NR_inotify_add_watch	317
# define __NR_inotify_rm_watch	318
#elif defined (__sh__)
# define __NR_inotify_init	290
# define __NR_inotify_add_watch	291
# define __NR_inotify_rm_watch	292
#elif defined (__mips__)

#if _MIPS_SIM == _ABIO32
/*
 * Linux o32 style syscalls are in the range from 4000 to 4999.
 */
#define __NR_Linux             4000
#define __NR_inotify_init      (__NR_Linux + 284)
#define __NR_inotify_add_watch (__NR_Linux + 285)
#define __NR_inotify_rm_watch  (__NR_Linux + 286)
#endif

#if _MIPS_SIM == _ABI64
/*
 * Linux 64-bit syscalls are in the range from 5000 to 5999.
 */
#define __NR_Linux 5000
#define __NR_inotify_init      (__NR_Linux + 243)
#define __NR_inotify_add_watch (__NR_Linux + 244)
#define __NR_inotify_rm_watch  (__NR_Linux + 245)
#endif

#if _MIPS_SIM == _ABIN32
/*
 * Linux N32 syscalls are in the range from 6000 to 6999.
 */
#define __NR_Linux 6000
#define __NR_inotify_init      (__NR_Linux + 247)
#define __NR_inotify_add_watch (__NR_Linux + 248)
#define __NR_inotify_rm_watch  (__NR_Linux + 249)
#endif

#else
# error "Unsupported architecture!"
#endif

static inline int inotify_init (void)
{
	return syscall (__NR_inotify_init);
}

static inline int inotify_add_watch (int fd, const char *name, __u32 mask)
{
	return syscall (__NR_inotify_add_watch, fd, name, mask);
}

static inline int inotify_rm_watch (int fd, __u32 wd)
{
	return syscall (__NR_inotify_rm_watch, fd, wd);
}

#endif

#endif
