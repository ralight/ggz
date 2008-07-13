#ifndef KGGZMOD_MISC_PRIVATE_H
#define KGGZMOD_MISC_PRIVATE_H

#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

namespace KGGZMod
{

// The following function is a variant of libggz's ggz_read_fd()
// All the historic Unix compatibility has been removed
// The return code has been C++-ified
// All GGZ debug/error handling code has been removed

bool readfiledescriptor(int sock, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t	n;
        char dummy;

	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

        /* We're just sending a fd, so it's a dummy byte */
	iov[0].iov_base = &dummy;
	iov[0].iov_len = 1;

	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ( (n = recvmsg(sock, &msg, 0)) < 0) {
		//ggz_debug(GGZ_SOCKET_DEBUG, "Error reading fd msg.");
		return false;
	}

        if (n == 0) {
		//ggz_debug(GGZ_SOCKET_DEBUG, "Warning: fd is closed.");
	        return false;
	}

        if ( (cmptr = CMSG_FIRSTHDR(&msg)) == NULL
	     || cmptr->cmsg_len != CMSG_LEN(sizeof(int))) {
		//ggz_debug(GGZ_SOCKET_DEBUG, "Bad cmsg.");
		return false;
	}

	if (cmptr->cmsg_level != SOL_SOCKET) {
		//ggz_debug(GGZ_SOCKET_DEBUG, "Bad cmsg.");
		return false;
	}

	if (cmptr->cmsg_type != SCM_RIGHTS) {
		//ggz_debug(GGZ_SOCKET_DEBUG, "Bad cmsg.");
		return false;
	}

	/* Everything is good */
	*recvfd = *((int *) CMSG_DATA(cmptr));
	
        return true;
}

}

#endif

