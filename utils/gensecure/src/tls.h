/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/

/* tls: transparent Transport Layer Security functions */

#ifndef TLS_H
#define TLS_H

#include "gensecure.h"

/* Internal: verify a received certificate */
int tls_verify(int preverify_ok, X509_STORE_CTX *ctx);

/* Internal: display an error */
void tls_error(const char *error, const char *file, int line);
/* Internal: initialize the environment */
void tls_certkey();
/* Internal: give out TLS errors */
char *tls_exterror(SSL *_tls, int ret);
/* Internal: initialize context */
void tls_init(int verify);

#define TLSERROR(x) tls_error(x, __FILE__, __LINE__)

#endif

