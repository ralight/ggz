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

void tls_prepare(const char *cert, const char *key, pem_password_cb *callback)
{
	_callback = callback;
	_cert = (char*)cert;
	_key = (char*)key;
}

void tls_start(int fd, int mode)
{
	int ret;
	
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
		SSL_CTX_set_verify(_tlsctx, SSL_VERIFY_NONE, NULL);
		_tls = SSL_new(_tlsctx);
		if(_tls)
		{
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
				if((ret != 1) || (!_state)) TLSERROR("Handshake failed.");
				else _tls_active = 1;
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

