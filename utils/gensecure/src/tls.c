/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/
 
/* tls: transparent Transport Layer Security functions */

#include "tls.h"
#include <stdio.h>

/* The control structure for TLS */
SSL_CTX *_tlsctx;
/* The connection object */
SSL *_tls;
/* Flag to indicate an active TLS connection */
int _tls_active;
/* Flag to indicate a correct state flow */
int _state;
/* The registered callback to decrypt the private key (may be NULL) */
pem_password_cb *_callback;
/* Location of the certificate */
char *_cert;
/* Location of the private key */
char *_key;

void tls_error(const char *error, const char *file, int line)
{
	printf("*** ERROR! ***\n");
	printf("(TLS: %s)\n", error);
	printf("(in %s, line %i)\n", file, line);
	printf("**************\n");
	_state = 0;
}

char *tls_exterror(int ret)
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
}

void tls_prepare(const char *cert, const char *key, pem_password_cb *callback)
{
	_callback = callback;
	_cert = (char*)cert;
	_key = (char*)key;
}

int tls_verify(int preverify_ok, X509_STORE_CTX *ctx)
{
	return preverify_ok;
}

void tls_start(int fd, int mode, int verify)
{
	int ret;
	STACK_OF(SSL_CIPHER) *stack;
	SSL_CIPHER *cipher;
	int bits;
	
	_state = 1;
	SSL_load_error_strings();
	SSL_library_init();

	_tls_active = 0;
	if((mode != TLS_CLIENT) && (mode != TLS_SERVER))
	{
		TLSERROR("Wrong mode.");
		return;
	}

	_tlsctx = SSL_CTX_new(TLSv1_method());
	if(!_tlsctx) TLSERROR("Couldn't create TLS object.\n");
	else
	{
		if(verify == TLS_VERIFY) SSL_CTX_set_verify(_tlsctx, SSL_VERIFY_PEER, tls_verify);
		else SSL_CTX_set_verify(_tlsctx, SSL_VERIFY_NONE, NULL);
		_tls = SSL_new(_tlsctx);
		if(_tls)
		{
			printf("Available ciphers: %s\n", SSL_get_cipher_list(_tls, 0));
			stack = SSL_get_ciphers(_tls);
			while(cipher = (SSL_CIPHER*)sk_pop(stack))
			{
				printf("* Cipher: %s\n", SSL_CIPHER_get_name(cipher));
				printf("  Bits: %i\n", SSL_CIPHER_get_bits(cipher, &bits));
				printf("  Used bits: %i\n", bits);
				printf("  Version: %s\n", SSL_CIPHER_get_version(cipher));
				printf("  Description: %s\n", SSL_CIPHER_description(cipher, NULL, 0));
			}
			ret = SSL_set_fd(_tls, fd);
			if(!ret) TLSERROR("Assignment to connection failed.");
			else
			{
				SSL_set_read_ahead(_tls, 1);
				if(mode == TLS_SERVER)
				{
					tls_certkey();
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
					TLSERROR("Handshake failed.");
					printf("Ret: %i, State: %i\n", ret, _state);
					printf("EXT: %s\n%s\n%s\n%s\n%s\n", tls_exterror(ret), ERR_error_string(ret, NULL),
						ERR_lib_error_string(ret), ERR_func_error_string(ret), ERR_reason_error_string(ret));
				}
				else
				{
					if((mode == TLS_SERVER) || (verify == TLS_NOVERIFY)) _tls_active = 1;
					else
					{
						if(SSL_get_peer_certificate(_tls))
						{
							if(SSL_get_verify_result(_tls) == X509_V_OK)
							{
								_tls_active = 1;
							}
							else TLSERROR("Invalid vertificate.");
						}
						else TLSERROR("Couldn't get certificate.");
					}
				}
			}
		}
	}
}

void tls_certkey()
{
	int ret;

	if(!_tls)
	{
		TLSERROR("Certificate cannot be loaded.");
		return;
	}

	/*PEM_set_getkey_callback(tls_keycallback);*/
	SSL_CTX_set_default_passwd_cb(_tlsctx, _callback);

	ret = SSL_use_RSAPrivateKey_file(_tls, _key, SSL_FILETYPE_PEM);
	if(ret != 1) TLSERROR("Error loading TLS PEM private key.");
	ret = SSL_use_certificate_file(_tls, _cert, SSL_FILETYPE_PEM);
	if(ret != 1) TLSERROR("Error loading TLS PEM certificate.");
}

void tls_finish()
{
	if(_tls) SSL_shutdown(_tls);
	if(_tls) SSL_free(_tls);
	if(_tlsctx) SSL_CTX_free(_tlsctx);
	_tls_active = 0;
	_state = 1;
}

int tls_input(char *buffer, int size)
{
	return SSL_read(_tls, buffer, size);
}

int tls_output(const char *s)
{
	return SSL_write(_tls, s, sizeof(s));
}

int tls_active()
{
	return _tls_active;
}

