/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/

#ifndef GENSECURE_H
#define GENSECURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <openssl/ssl.h>

/* Mode of connection (for tls_start) */
#define TLS_SERVER 1
#define TLS_CLIENT 2

/* Verification modes */
#define TLS_NOVERIFY 1
#define TLS_VERIFY 2

/* Make sure callback exists */
#ifndef pem_password_cb
/*typedef int pem_password_cb(char *buf, int size, int rwflag, void *userdata);*/
#endif

/* Server only: prepare the environment */
void tls_prepare(const char *cert, const char *key, pem_password_cb *callback);
/* Secure an already existing connection */
void tls_start(int fd, int mode, int verify);
/* Bring connection in an insecure state again */
void tls_finish(int fd);
/* Return whether TLS is active or not */
int tls_active(int fd);
/* Read some bytes (secure) */
int tls_read(int fd, char *buffer, int size);
/* Write some bytes (secure) */
int tls_write(int fd, const char *s, int size);

#ifdef __cplusplus
}
#endif

#endif

