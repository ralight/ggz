/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/
 
/* tls: transparent Transport Layer Security functions */

#include "tls.h"
#include "list.h"
#include <stdio.h>
#include <string.h>
#include <openssl/err.h>
#include <unistd.h>

/* The control structure for TLS */
SSL_CTX *_tlsctx = NULL;
/* Flag to indicate a correct state flow */
int _state = 0;
/* The registered callback to decrypt the private key (may be NULL) */
pem_password_cb *_callback = NULL;
/* Location of the certificate */
char *_cert = NULL;
/* Location of the private key */
char *_key = NULL;

/* Not everything in OpenSSL is well documented... */
#define OPENSSLCHECK(x)

void tls_error(const char *error, const char *file, int line)
{
	printf("*** ERROR! ***\n");
	printf("(TLS: %s)\n", error);
	printf("(in %s, line %i)\n", file, line);
	printf("**************\n");
	_state = 0;
}

char *tls_exterror(SSL *_tls, int ret)
{
	switch(SSL_get_error(_tls, ret))
	{
		case SSL_ERROR_NONE:
			return "SSL_ERROR_NONE";
			break;
		case SSL_ERROR_ZERO_RETURN:
			return "SSL_ERROR_ZERO_RETURN";
			break;
		case SSL_ERROR_WANT_READ:
			return "SSL_ERROR_WANT_READ";
			break;
		case SSL_ERROR_WANT_WRITE:
			return "SSL_ERROR_WANT_WRITE";
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			return "SSL_ERROR_WANT_X509_LOOKUP";
			break;
		case SSL_ERROR_SYSCALL:
			return "SSL_ERROR_SYSCALL";
			break;
		case SSL_ERROR_SSL:
			return "SSL_ERROR_SSL";
			break;
	}
	return NULL;
}

void tls_prepare(const char *cert, const char *key, pem_password_cb *callback)
{
	_callback = callback;
	_cert = (char*)cert;
	_key = (char*)key;
}

int tls_verify(int preverify_ok, X509_STORE_CTX *ctx)
{
	printf("### VERIFY CALLBACK: %i ###\n", preverify_ok);
	preverify_ok = 1;
	return preverify_ok;
}

void tls_init(int verify)
{
	SSL_load_error_strings();
	SSL_library_init();

	_tlsctx = SSL_CTX_new(TLSv1_method());
	if(!_tlsctx) TLSERROR("Couldn't create TLS object.\n");
	else
	{
		OPENSSLCHECK(SSL_CTX_set_quiet_shutdown(_tlsctx, 1));
		OPENSSLCHECK(SSL_CTX_set_info_callback(_tlsctx, NULL));
		OPENSSLCHECK(SSL_CTX_load_verify_locations(ctx, CAfile, CApath));
		OPENSSLCHECK(SSL_CTX_set_default_verify_paths());
		if(verify == TLS_VERIFY) SSL_CTX_set_verify(_tlsctx, SSL_VERIFY_PEER, tls_verify);
		else SSL_CTX_set_verify(_tlsctx, SSL_VERIFY_NONE, NULL);
	}
}

void tls_start(int fd, int mode, int verify)
{
	int ret, ret2;
	STACK_OF(SSL_CIPHER) *stack;
	SSL_CIPHER *cipher;
	int bits;
	char *cipherlist;
	SSL *_tls;
	int _tls_active;

	_state = 1;
	_tls_active = 0;
	if((mode != TLS_CLIENT) && (mode != TLS_SERVER))
	{
		TLSERROR("Wrong mode.");
		return;
	}

	if(!_tlsctx) tls_init(verify);
		
	_tls = SSL_new(_tlsctx);
	if(_tls)
	{
		/*cipherlist = strdup(SSL_get_cipher_list(_tls, 0));*/
		cipherlist = NULL;
		stack = SSL_get_ciphers(_tls);
		while((cipher = (SSL_CIPHER*)sk_pop(stack)) != NULL)
		{
			printf("* Cipher: %s\n", SSL_CIPHER_get_name(cipher));
			printf("  Bits: %i\n", SSL_CIPHER_get_bits(cipher, &bits));
			printf("  Used bits: %i\n", bits);
			printf("  Version: %s\n", SSL_CIPHER_get_version(cipher));
			printf("  Description: %s\n", SSL_CIPHER_description(cipher, NULL, 0));
			if(cipherlist)
			{
				cipherlist = (char*)realloc(cipherlist, (strlen(cipherlist) + 1) + strlen(SSL_CIPHER_get_name(cipher)) + 1);
				strcat(cipherlist, ":");
				strcat(cipherlist, SSL_CIPHER_get_name(cipher));
			}
			else
			{
				cipherlist = (char*)malloc(strlen(SSL_CIPHER_get_name(cipher)) + 1);
				strcpy(cipherlist, SSL_CIPHER_get_name(cipher));
			}
		}
		printf("Available ciphers: %s\n", cipherlist);
		/*ret = SSL_set_cipher_list(_tls, "EDH-RSA-DES-CBC3-SHA");*/
		ret = SSL_set_cipher_list(_tls, cipherlist);
		if(!ret) TLSERROR("Cipher selection failed.");
		ret = SSL_set_fd(_tls, fd);
		if(!ret) TLSERROR("Assignment to connection failed.");
		else
		{
			SSL_set_read_ahead(_tls, 1);
			if(mode == TLS_SERVER)
			{
				tls_certkey(_tls);
				if(_state)
				{
					SSL_set_accept_state(_tls);
					ret = SSL_accept(_tls);
				}
			}
			else
			{
				SSL_set_connect_state(_tls);
				ret = SSL_connect(_tls);
			}
			if((ret != 1) || (!_state))
			{
				printf("Ret: %i, State: %i\n", ret, _state);
				TLSERROR("Handshake failed.");
				ret2 = ERR_get_error();
				printf("EXT: %s\n%s\n%s\n%s\n%s\n", tls_exterror(_tls, ret), ERR_error_string(ret2, NULL),
					ERR_lib_error_string(ret2), ERR_func_error_string(ret2), ERR_reason_error_string(ret2));
			}
			else
			{
				printf(">>>>> Handshake successful.\n");
				if((mode == TLS_SERVER) || (verify == TLS_NOVERIFY)) _tls_active = 1;
				else
				{
					printf(">>>>> Client side, thus checking Certificate.\n");
					printf("Negotiated cipher: %s\n", SSL_get_cipher(_tls));
					printf("Shared ciphers: %s\n", SSL_get_shared_ciphers(_tls, NULL, 0));
					if(SSL_get_peer_certificate(_tls))
					{
						if(SSL_get_verify_result(_tls) == X509_V_OK)
						{
							_tls_active = 1;
						}
						else
						{
							printf("Error code: %li\n", SSL_get_verify_result(_tls));
							TLSERROR("Invalid certificate, or certificate is not self-signed.");
						}
					}
					else TLSERROR("Couldn't get certificate.");
				}
			}
			tls_list_add(fd, _tls, _tls_active);
		}
	}
}

void tls_certkey(SSL *_tls)
{
	int ret;

	if(!_tls)
	{
		TLSERROR("Certificate cannot be loaded.");
		return;
	}

	if((!_key) || (!_cert) || (!_callback))
	{
		printf("WARNING: certificates are disabled!\n");
		return;
	}

	/*PEM_set_getkey_callback(tls_keycallback);*/
	SSL_CTX_set_default_passwd_cb(_tlsctx, _callback);

	ret = SSL_use_RSAPrivateKey_file(_tls, _key, SSL_FILETYPE_PEM);
	if(ret != 1) TLSERROR("Error loading TLS PEM private key.");
	ret = SSL_use_certificate_file(_tls, _cert, SSL_FILETYPE_PEM);
	if(ret != 1) TLSERROR("Error loading TLS PEM certificate.");
	ret = SSL_check_private_key(_tls);
	if(!ret) TLSERROR("Private key doesn't match certificate public key.");
	printf("*** certificate loaded ***\n");
}

void tls_finish(int fd)
{
	SSL *handler;

	handler = tls_list_get(fd);
	if(handler)
	{
		SSL_shutdown(handler);
		SSL_free(handler);
		tls_list_remove(fd);
	}
	/*if(_tlsctx) SSL_CTX_free(_tlsctx);*/ /*only if refcount == 0*/
	/*ERR_free_strings();*/
	/*_state = 1;*/
}

int tls_read(int fd, char *buffer, int size)
{
	SSL *handler;
	int ret;

	//printf("<< %s\n", buffer);
	handler = tls_list_get(fd);
	if(!handler)
	{
		/*TLSERROR("Given fd is not secure.");*/
		return read(fd, buffer, size);
	}
	ret = SSL_read(handler, buffer, size);
	if(ret <= 0)
	{
		switch(SSL_get_error(handler, ret))
		{
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				break;
			case SSL_ERROR_SYSCALL:
				ret = ERR_get_error();
				if(!ret)
				{
					printf("Protocol violation: EOF\n");
				}
				else
				{
					printf("Unix IO error: %i\n", errno);
				}
				break;
			default:
				printf("SSL read error (%i) on fd %i!\n", ret, fd);
				printf("SSL: %s\n", tls_exterror(handler, ret));
		}
	}
	//else buffer[ret] = 0;
	return ret;
}

int tls_write(int fd, const char *s, int size)
{
	SSL *handler;
	int ret;

	printf(">> %s\n", s);
	handler = tls_list_get(fd);
	if(!handler)
	{
		/*TLSERROR("Given fd is not secure.");*/
		return write(fd, s, size);
	}
	ret = SSL_write(handler, s, size);
	if(ret <= 0)
	{
		switch(SSL_get_error(handler, ret))
		{
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
				break;
			case SSL_ERROR_SYSCALL:
				ret = ERR_get_error();
				if(!ret)
				{
					printf("Protocol violation: EOF\n");
				}
				else
				{
					printf("Unix IO error: %i\n", errno);
				}
				break;
			default:
				printf("SSL write error (%i) on fd %i!\n", ret, fd);
				printf("SSL: %s\n", tls_exterror(handler, ret));
		}
	}
	return ret;
}

int tls_active(int fd)
{
	return (tls_list_active(fd) > 0);
}

