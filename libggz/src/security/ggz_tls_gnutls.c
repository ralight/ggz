/*
 * File: tls.c
 * Author: Rich Gade
 * Project: GGZ Core Client Lib
 * Date: 10/21/02
 *
 * Routines to enable easysock to utilize TLS using gnutls
 *
 * Copyright (C) 2002 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "config.h"
#ifdef GGZ_TLS_GNUTLS

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gnutls.h>

#include "ggz.h"

#ifndef gnutls_session
#define gnutls_session GNUTLS_STATE
#define gnutls_anon_server_credentials GNUTLS_ANON_SERVER_CREDENTIALS
#define gnutls_anon_client_credentials GNUTLS_ANON_CLIENT_CREDENTIALS
#define gnutls_anon_allocate_client_credentials gnutls_anon_allocate_client_sc
#define gnutls_anon_allocate_server_credentials gnutls_anon_allocate_server_sc
#define gnutls_credentials_set gnutls_cred_set
#define GNUTLS_COMP_LZO GNUTLS_COMP_ZLIB
#endif

#ifndef GNUTLS_CIPHER_ARCFOUR
#define GNUTLS_CRD_ANON GNUTLS_ANON
#define GNUTLS_KX_DHE_DSS GNUTLS_KX_X509PKI_DHE_DSS
#define GNUTLS_KX_DHE_RSA GNUTLS_KX_X509PKI_DHE_RSA
#define GNUTLS_CIPHER_ARCFOUR_40 GNUTLS_CIPHER_ARCFOUR
#define GNUTLS_CIPHER_ARCFOUR_128 GNUTLS_CIPHER_ARCFOUR
#endif

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static int state_entries = -1;
static gnutls_session *state = NULL;
static gnutls_anon_server_credentials s_cred;
static gnutls_anon_client_credentials c_cred;
/*static gnutls_dh_params params;
static gnutls_datum prime, generator;*/

const int cipher_priority[] = {GNUTLS_CIPHER_3DES_CBC, GNUTLS_CIPHER_NULL, GNUTLS_CIPHER_ARCFOUR_40, GNUTLS_CIPHER_ARCFOUR_128, 0};
const int mac_priority[] = {GNUTLS_MAC_NULL, GNUTLS_MAC_MD5, GNUTLS_MAC_SHA, 0};
const int kx_priority[] = {GNUTLS_KX_ANON_DH, GNUTLS_KX_DHE_DSS, GNUTLS_KX_DHE_RSA, 0};
const int protocol_priority[] = {GNUTLS_TLS1, GNUTLS_SSL3, 0};
const int compression_priority[] = {GNUTLS_COMP_NULL, GNUTLS_COMP_ZLIB, GNUTLS_COMP_LZO, 0};

void ggz_tls_init(const char *certfile, const char *keyfile, const char *password)
{
	/* no cert support yet */
}


int ggz_tls_support_query(void)
{
	return 1;
}

int ggz_tls_enable_fd(int fdes, GGZTLSType whoami, GGZTLSVerificationType verify)
{
	gnutls_session session;
	int ret;

	/* Check for argument validity */
	if(verify != GGZ_TLS_VERIFY_NONE) {
		ggz_error_msg("verify_peer is not supported yet\n");
		return 0;
	}

	/* Initialize anon Diffie-Hellman if necessary */
	pthread_mutex_lock(&mut);
	if(state_entries == -1) {
		if(gnutls_global_init() < 0) {
			ggz_error_msg("gnutls_global_init() failure\n");
			return 0;
		}
		if(whoami == GGZ_TLS_CLIENT) {
			gnutls_anon_allocate_client_credentials(&c_cred);
		} else {
			gnutls_anon_allocate_server_credentials(&s_cred);
			/*gnutls_dh_params_init(&params);
			gnutls_dh_params_generate(&prime, &generator, 512);
			gnutls_dh_params_set(params, prime, generator, 512);
			gnutls_anon_set_server_dh_params(s_cred, params);*/
		}
		state_entries = 0;
	}
	pthread_mutex_unlock(&mut);

	if(whoami == GGZ_TLS_CLIENT)
		gnutls_init(&session, GNUTLS_CLIENT);
	else
		gnutls_init(&session, GNUTLS_SERVER);

	if(whoami == GGZ_TLS_CLIENT) {
		/*gnutls_set_default_priority(session);*/
		gnutls_mac_set_priority(session, mac_priority);
		gnutls_kx_set_priority(session, kx_priority);
		gnutls_protocol_set_priority(session, protocol_priority);
		gnutls_compression_set_priority(session, compression_priority);
		gnutls_cipher_set_priority(session, cipher_priority);
	}
	else {
		gnutls_protocol_set_priority(session, protocol_priority);
		gnutls_compression_set_priority(session, compression_priority);
		gnutls_cipher_set_priority(session, cipher_priority);
		gnutls_mac_set_priority(session, mac_priority);
		gnutls_kx_set_priority(session, kx_priority);
	}

	/*gnutls_dh_set_prime_bits(session, 512);*/

	if(whoami == GGZ_TLS_CLIENT)
		gnutls_credentials_set(session, GNUTLS_CRD_ANON, c_cred);
	else
		gnutls_credentials_set(session, GNUTLS_CRD_ANON, s_cred);

	gnutls_transport_set_ptr(session, fdes);
	do {
		ret = gnutls_handshake(session);
	} while(ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);

	if(ret < 0) {
		gnutls_deinit(session);
		ggz_error_msg("TLS handshake failed miserably (%s) (%s)\n",
			(whoami == GGZ_TLS_CLIENT ? "client" : "server"),
			gnutls_strerror(ret));
		return 0;
	}

	pthread_mutex_lock(&mut);
	if(state_entries <= fdes) {
		state = ggz_realloc(state, (fdes+1)*sizeof(gnutls_session*));
		state_entries = fdes+1;
	}
	pthread_mutex_unlock(&mut);

	state[fdes] = session;

	return 1;
}


int ggz_tls_disable_fd(int fdes)
{
	pthread_mutex_lock(&mut);
	if(state_entries > fdes)
		if(state[fdes]) {
			gnutls_deinit(state[fdes]);
			ggz_free(state[fdes]);
			state[fdes] = NULL;
		}
	pthread_mutex_unlock(&mut);

	return 1;
}


static int check_fd(int fdes)
{
	pthread_mutex_lock(&mut);
	if(state_entries > fdes)
		if(state[fdes]) {
			pthread_mutex_unlock(&mut);
			return 1;
		}
	pthread_mutex_unlock(&mut);
	return 0;
}


size_t ggz_tls_write(int fd, void *ptr, size_t n)
{
	if(check_fd(fd))
		return gnutls_write(state[fd], ptr, n);
	else
		return write(fd, ptr, n);
}


size_t ggz_tls_read(int fd, void *ptr, size_t n)
{
	if(check_fd(fd))
		return gnutls_read(state[fd], ptr, n);
	else
		return read(fd, ptr, n);
}

#endif

