#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <ggz.h>

#include "fdtrack.h"

typedef struct _fdtrack {
	char in_use;
	char *tag;
	char *file;
	int line;
	char *pathhost;
} _fdtrack;

#define FDTCOUNT 2048

static _fdtrack *fdt;
static FILE *fdtptr;
static pthread_rwlock_t lock;

void fdt_error(const char *error, int fd, const char *hostpath, const char *tag, const char *file, int line);
void _fdt_open_core(int fd, const char *pathhost, const char *tag, const char *file, int line);


void fdt_init(const char *logfile)
{
	char *filename;

	if(!logfile) return;

	pthread_rwlock_init(&lock, NULL);

	fdt = calloc(FDTCOUNT, sizeof(_fdtrack));

	filename = malloc(sizeof(char) * (strlen(logfile) + 10));
	if(filename){
		snprintf(filename, strlen(logfile)+10, "%s.%d", logfile, getpid());
		fdtptr = fopen(filename, "wt");
		free(filename);
	}
}


void fdt_cleanup(void)
{
	int i;

	if(fdt){
		for(i = 0; i < FDTCOUNT; i++){
			fdt_close(i);
		}
		free(fdt);
	}
	if(fdtptr){
		fclose(fdtptr);
	}
	pthread_rwlock_destroy(&lock);
}


void fdt_error(const char *error, int fd, const char *pathhost, const char *tag, const char *file, int line)
{
	if(!fdtptr || fd < 0 || fd >= FDTCOUNT) return;

	fprintf(fdtptr, "%ld ERROR: %s\n", time(NULL), error);
	fprintf(fdtptr, "\tfd %d %s (%s:%d) %s\n", fd, fdt[fd].tag, fdt[fd].file, fdt[fd].line, fdt[fd].pathhost);
}


void fdt_log(void)
{
	int i;
	struct stat buf;

	if(!fdtptr || !fdt) return;

	pthread_rwlock_rdlock(&lock);
	fprintf(fdtptr, "================ Curent State (%ld) ================\n", time(NULL));

	for(i = 0; i < FDTCOUNT; i++){
		if(fdt[i].in_use){
			if(!fstat(i, &buf)){
				if(S_ISSOCK(buf.st_mode)){
					fprintf(fdtptr, "\tfd %d %s (%s:%d) %s\n", i, fdt[i].tag, fdt[i].file, fdt[i].line, fdt[i].pathhost);
				}
			}
		}
	}
	fprintf(fdtptr, "===========================================================\n");
	pthread_rwlock_unlock(&lock);
}


void fdt_message(int fd, const char *fmt, ...)
{
	va_list ap;

	if(fdtptr && fdt){
		if(fd >= 0){
			fprintf(fdtptr, "%ld fd: %d (%s) ", time(NULL), fd, fdt[fd].pathhost);
		}else{
			fprintf(fdtptr, "%ld fd: %d ", time(NULL), fd);
		}
		va_start(ap, fmt);
		vfprintf(fdtptr, fmt, ap);
		va_end(ap);
	}
}


void _fdt_open_core(int fd, const char *pathhost, const char *tag, const char *file, int line)
{
	if(!fdt || fd < 0 || fd >= FDTCOUNT) return;

	pthread_rwlock_wrlock(&lock);

	if(fdt[fd].in_use){
		fdt_error("Duplicate fd usage (new)", fd, pathhost, tag, file, line);
		fdt_error("Duplicate fd usage (existing)", fd, fdt[fd].pathhost, fdt[fd].tag, fdt[fd].file, fdt[fd].line);
	}else{
		fdt[fd].in_use = 1;
		fdt[fd].pathhost = ggz_strdup(pathhost);
		fdt[fd].tag = ggz_strdup(tag);
		fdt[fd].file = ggz_strdup(file);
		fdt[fd].line = line;
	}
	pthread_rwlock_unlock(&lock);
}


/* 
 * ======================================================================
 *  Actual fd related functions below here.
 * ======================================================================
 */
int _fdt_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, const char *tag, const char *file, int line)
{
	int fd;
	const char *tmp;

	fd = accept(sockfd, addr, addrlen);

	if(fdt && fd >= 0 && fd < FDTCOUNT){
		tmp = ggz_getpeername(fd, 0);
		if(tmp){
			pthread_rwlock_wrlock(&lock);
			if(fdt[fd].in_use == 1){
				fdt_error("Duplicate fd usage (new) %d %s (%s:%d)", fd, tmp, tag, file, line);
				fdt_error("Duplicate fd usage (existing) %d %s (%s:%d)", fd, fdt[fd].pathhost, fdt[fd].tag, fdt[fd].file, fdt[fd].line);
			}else{
				fprintf(fdtptr, "%ld accepting socket from %d on %d %s (%s:%d) for host %s\n", time(NULL), sockfd, fd, tag, file, line, tmp);
				fdt[fd].in_use = 1;
				fdt[fd].pathhost = ggz_strdup(tmp);
				fdt[fd].tag = ggz_strdup(tag);
				fdt[fd].file = ggz_strdup(file);
				fdt[fd].line = line;
			}
			pthread_rwlock_unlock(&lock);
			ggz_free(tmp);
		}
	}

	return fd;
}


int _fdt_close(int fd, const char *tag, const char *file, int line)
{
	if(fdt && fd >= 0 && fd < FDTCOUNT){
		if(fdtptr){
			fprintf(fdtptr, "%ld closing fd %d %s (%s:%d) (from %s (%s:%d) %s)\n", time(NULL), fd, tag, file, line, fdt[fd].tag, fdt[fd].file, fdt[fd].line, fdt[fd].pathhost);
		}
		pthread_rwlock_wrlock(&lock);

		fdt[fd].in_use = 0;
		if(fdt[fd].pathhost){
			ggz_free(fdt[fd].pathhost);
			fdt[fd].pathhost = NULL;
		}
		if(fdt[fd].tag){
				ggz_free(fdt[fd].tag);
				fdt[fd].tag = NULL;
		}
		if(fdt[fd].file){
				ggz_free(fdt[fd].file);
				fdt[fd].file = NULL;
		}
		fdt[fd].line = -1;

		pthread_rwlock_unlock(&lock);
	}
	return close(fd);
}


int _fdt_open(const char *pathname, int flags, const char *tag, const char *file, int line)
{
	int fd;

	fd = open(pathname, flags);

	_fdt_open_core(fd, pathname, tag, file, line);
	if(fdtptr){
		fprintf(fdtptr, "%ld opening fd %d %s (%s:%d)\n", time(NULL), fd, tag, file, line);
	}

	return fd;
}


int _fdt_openm(const char *pathname, int flags, mode_t mode, const char *tag, const char *file, int line)
{
	int fd;

	fd = open(pathname, flags, mode);

	_fdt_open_core(fd, pathname, tag, file, line);
	if(fdtptr){
		fprintf(fdtptr, "%ld opening fd %d %s (%s:%d)\n", time(NULL), fd, tag, file, line);
	}

	return fd;
}


int _fdt_socket(int domain, int type, int protocol, const char *tag, const char *file, int line)
{
	int fd;

	fd = socket(domain, type, protocol);

	_fdt_open_core(fd, NULL, tag, file, line);
	if(fdtptr){
		fprintf(fdtptr, "creating socket %d %s (%s:%d)\n", fd, tag, file, line);
	}

	return fd;
}


int _fdt_socketpair(int d, int type, int protocol, int sv[2], const char *tag, const char *file, int line)
{
	int rc;

	rc = socketpair(d, type, protocol, sv);

	_fdt_open_core(sv[0], NULL, tag, file, line);
	_fdt_open_core(sv[1], NULL, tag, file, line);
	if(fdtptr){
		fprintf(fdtptr, "%ld creating socketpair %d %d %s (%s:%d)\n", time(NULL), sv[0], sv[1], tag, file, line);
	}

	return rc;
}


