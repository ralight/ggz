/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/

/* tls: transparent Transport Layer Security functions */

#ifndef TLS_H
#define TLS_H

#include <openssl/ssl.h>

/* Mode of connection (for tls_start) */
#define TLS_SERVER 1
#define TLS_CLIENT 2

/* Verification modes */
#define TLS_NOVERIFY 1
#define TLS_VERIFY 2

/* Server only: prepare the environment */
void tls_prepare(const char *cert, const char *key, pem_password_cb *callback);
/* Secure an already existing connection */
void tls_start(int fd, int mode, int verify);
/* Bring connection in an insecure state again */
void tls_finish();
/* Return whether TLS is active or not */
int tls_active();
/* Read some bytes (secure) */
int tls_input(char *buffer, int size);
/* Write some bytes (secure) */
int tls_output(const char *s);
/* Verify a received certificate */
int tls_verify(int preverify_ok, X509_STORE_CTX *ctx);

/* Internal: display an error */
void tls_error(const char *error, const char *file, int line);
/* Internal: initialize the environment */
void tls_certkey();
/* Internal: give out TLS errors */
char *tls_exterror(int ret);

#define TLSERROR(x) tls_error(x, __FILE__, __LINE__)

#endif

