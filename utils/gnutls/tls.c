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

#include <config.h>

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
#include "msg.h"


#ifdef HAVE_GNUTLS

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static int state_entries = -1;
static GNUTLS_STATE **state = NULL;
static GNUTLS_ANON_SERVER_CREDENTIALS s_cred;
static GNUTLS_ANON_CLIENT_CREDENTIALS c_cred;

const int protocol_priority[] = {GNUTLS_TLS1, GNUTLS_SSL3, 0};
const int kx_priority[] = {GNUTLS_KX_ANON_DH, 0};
const int cipher_priority[] = {GNUTLS_CIPHER_3DES_CBC, GNUTLS_CIPHER_ARCFOUR,0};
const int comp_priority[] = {GNUTLS_COMP_ZLIB, GNUTLS_COMP_NULL, 0};
const int mac_priority[] = {GNUTLS_MAC_SHA, GNUTLS_MAC_MD5, 0};


int ggz_tls_support_query(void)
{
	return 1;
}


void ggz_tls_enable_fd(int fdes, GGZTLSType whoami)
{
	GNUTLS_STATE *new;
	int ret;

	/* Initialize anon Diffie-Hellman if necessary */
	pthread_mutex_lock(&mut);
	if(state_entries == -1) {
		if(gnutls_global_init() < 0)
			ggz_error_msg_exit("gnutls_global_init() failure\n");
		if(whoami == GGZ_TLS_CLIENT) {
			gnutls_anon_allocate_client_sc(&c_cred);
		} else {
			gnutls_anon_allocate_server_sc(&s_cred);
			gnutls_anon_set_server_cred(s_cred, 1024);/*1024 bits */
				  /* actually results in a prime of 1035 bits */
		}
		state_entries = 0;
	}
	pthread_mutex_unlock(&mut);

	new = ggz_malloc(sizeof(GNUTLS_STATE));

	if(whoami == GGZ_TLS_CLIENT)
		gnutls_init(new, GNUTLS_CLIENT);
	else
		gnutls_init(new, GNUTLS_SERVER);

	gnutls_cipher_set_priority(*new, cipher_priority);
	gnutls_compression_set_priority(*new, comp_priority);
	gnutls_kx_set_priority(*new, kx_priority);
	gnutls_protocol_set_priority(*new, protocol_priority);
	gnutls_mac_set_priority(*new, mac_priority);

	if(whoami == GGZ_TLS_CLIENT)
		gnutls_cred_set(*new, GNUTLS_ANON, c_cred);
	else
		gnutls_cred_set(*new, GNUTLS_ANON, s_cred);

	gnutls_transport_set_ptr(*new, fdes);
	do {
		ret = gnutls_handshake(*new);
	} while(ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);

	if(ret < 0) {
		gnutls_deinit(*new);
		ggz_free(new);
		ggz_error_msg("TLS handshake failed (%s)\n",
			      gnutls_strerror(ret));
		return;
	}

	/* Expand state table so we can store state for fdes */
	/* FIXME: This table should probably contract too one day? */
	/* FIXME: Is there a better way to store this stuff ULTRA fast? */
	/*        realizing that we have to lookup the state for EVERY  */
	/*        network read and write that gets performed?           */
	pthread_mutex_lock(&mut);
	if(state_entries <= fdes) {
		state = ggz_realloc(state, (fdes+1)*sizeof(GNUTLS_STATE));
		state_entries = fdes+1;
	}
	pthread_mutex_unlock(&mut);

	state[fdes] = new;

	return;
}


void ggz_tls_disable_fd(int fdes)
{
	pthread_mutex_lock(&mut);
	if(state_entries > fdes)
		if(state[fdes]) {
			gnutls_deinit(*state[fdes]);
			ggz_free(state[fdes]);
			state[fdes] = NULL;
		}
	pthread_mutex_unlock(&mut);

	return;
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
		return gnutls_write(*state[fd], ptr, n);
	else
		return write(fd, ptr, n);
}


size_t ggz_tls_read(int fd, void *ptr, size_t n)
{
	if(check_fd(fd))
		return gnutls_read(*state[fd], ptr, n);
	else
		return read(fd, ptr, n);
}


#else /* HAVE_GNUTLS */
/*****************************
 * Empty versions of functions
 * to prevent linkage errors
 *****************************/
int ggz_tls_support_query(void)
{
	return 0;
}

void ggz_tls_enable_fd(int fdes, GGZTLSType whoami)
{
	return;
}

void ggz_tls_disable_fd(int fdes)
{
	return;
}

size_t ggz_tls_write(int fd, void *ptr, size_t n)
{
	return write(fd, ptr, n);
}

size_t ggz_tls_read(int fd, void *ptr, size_t n)
{
	return read(fd, ptr, n);
}
#endif
