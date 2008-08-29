#ifndef FDTRACK_H
#define FDTRACK_H

#include <sys/socket.h>

/* Run before using any other fdt_ functions. logfile will have ".<pid>"
 * appended to it. */
void fdt_init(const char *logfile);

/* Run after finishing with fdt_ functions to free memory/close files. */
void fdt_cleanup(void);

/* Log the current open fds to the log file. */
void fdt_log(void);

/* Add a message to the log file. If fd >= 0 then the path/host will be
 * included in the message. */
void fdt_message(int fd, const char *fmt, ...);

/* The below are the replacement functions to use when fd tracking. Note that
 * there are two separate replacements for "open()" to cover both versions. */
int _fdt_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, const char *tag, const char *file, int line);
#define fdt_accept(sockfd, addr, addrlen) _fdt_accept(sockfd, addr, addrlen, __func__, __FILE__,  __LINE__)
int _fdt_close(int fd, const char *tag, const char *file, int line);
#define fdt_close(fd) _fdt_close(fd, __func__, __FILE__,  __LINE__)
int _fdt_open(const char *pathname, int flags, const char *tag, const char *file, int line);
#define fdt_open(pathname, flags) _fdt_open(pathname, flags, __func__, __FILE__,  __LINE__)
int _fdt_openm(const char *pathname, int flags, mode_t mode, const char *tag, const char *file, int line);
#define fdt_openm(pathname, flags, mode) _fdt_openm(pathname, flags, mode, __func__, __FILE__,  __LINE__)
int _fdt_socket(int domain, int type, int protocol, const char *tag, const char *file, int line);
#define fdt_socket(domain, type, protocol) _fdt_socket(domain, type, protocol, __func__, __FILE__,  __LINE__)
int _fdt_socketpair(int d, int type, int protocol, int sv[2], const char *tag, const char *file, int line);
#define fdt_socketpair(d, type, protocol, sv) _fdt_socketpair(d, type, protocol, sv, __func__, __FILE__,  __LINE__)

#endif

