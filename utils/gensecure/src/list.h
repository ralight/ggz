#ifndef TLS_LIST_H
#define TLS_LIST_H

#include <openssl/ssl.h>

/* Structure for mapping on fd's */
struct tls_list_t
{
	SSL *tls;
	int fd;
	struct tls_list_t *last;
	struct tls_list_t *next;
	int active;
};

typedef struct tls_list_t TLSList;

/* Functions which handle a list of mappings */
void tls_list_add(int fd, SSL *tls, int active);
void tls_list_remove(int fd);
SSL *tls_list_get(int fd);
int tls_list_active(int fd);

#endif

