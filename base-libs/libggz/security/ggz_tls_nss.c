/*
 * Routines to enable easysock to utilize TLS using NSS
 *
 * Copyright (C) 2007 Josef Spillner <josef@ggzgamingzone.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#ifdef GGZ_TLS_NSS

#include <unistd.h>

#include <prinit.h>
#include <nss/nss.h> // FIXME: evil path override!
#include <nss/ssl.h>

#include "ggz.h"

/* List entries */
struct list_entry
{
	PRFileDesc *fdtls;
	int fd;
	int active;
};
static GGZList *nsslist;

/* Utility comparison function for list entries */
static int list_entry_compare(const void *a, const void *b)
{
	struct list_entry x, y;
	x = *(struct list_entry*)a;
	y = *(struct list_entry*)b;
	return (x.fd == y.fd ? 0 : 1);
}

/* API function declarations missing from NSPR headers */
/* FIXME: blindly taken from nss_compat_ossl sources */
PRInt32 PR_FileDesc2NativeHandle(PRFileDesc *fd);
void PR_ChangeFileDescNativeHandle(PRFileDesc *fd, PRInt32 handle);

void ggz_tls_init(const char *certfile, const char *keyfile, const char *password)
{
	ggz_debug(GGZ_SOCKET_DEBUG, "TLS/NSS: ggz_tls_init");

	/*PR_Init();*/
	/*PK11_SetPasswordFunc();*/
	NSS_Init("/tmp");
	NSS_SetDomesticPolicy();

	nsslist = ggz_list_create(NULL, NULL, NULL, GGZ_LIST_ALLOW_DUPS);
}

#if 0
void ggz_tls_finish(void)
{
	NSS_Shutdown();
	PR_Cleanup();
}
#endif

int ggz_tls_support_query(void)
{
	return 1;
}

const char *ggz_tls_support_name(void)
{
	return "NSS";
}

int ggz_tls_enable_fd(int fdes, GGZTLSType whoami, GGZTLSVerificationType verify)
{
	PRFileDesc *fdpr, *fdtls;
	struct list_entry *entry;

	ggz_debug(GGZ_SOCKET_DEBUG, "TLS/NSS: ggz_tls_enable_fd(%i)", fdes);

	fdpr = PR_NewTCPSocket();

	// FIXME: the following lines are taken from nss_compat_ossl
	close(PR_FileDesc2NativeHandle(fdpr));
	fdpr = PR_GetIdentitiesLayer(fdpr, PR_NSPR_IO_LAYER);
	PR_ChangeFileDescNativeHandle(fdpr, fdes);

	fdtls = SSL_ImportFD(NULL, fdpr);
	if(!fdtls) return 0;

	if(whoami == GGZ_TLS_CLIENT)
	{
		/* FIXME: use real domain */
		SSL_SetURL(fdtls, "localhost");
	}
	else
	{
		/* FIXME: use values from ggz_tls_init instead of NULL */
		SSL_ConfigSecureServer(fdtls, NULL, NULL, kt_dh);
	}

	entry = (struct list_entry*)ggz_malloc(sizeof(struct list_entry));
	entry->fdtls = fdtls;
	entry->fd = fdes;
	entry->active = 1;
	ggz_list_insert(nsslist, entry);

	ggz_debug(GGZ_SOCKET_DEBUG, "TLS/NSS: enabled for role %i, tls socket %p[%p]", whoami, entry, fdtls);

	return 1;
}

int ggz_tls_disable_fd(int fdes)
{
	struct list_entry *entry;
	struct list_entry entrycmp;
	PRFileDesc *fdtls;
	GGZListEntry *listentry;

	ggz_debug(GGZ_SOCKET_DEBUG, "TLS/NSS: ggz_tls_disable_fd(%i)", fdes);

	entrycmp.fd = fdes;
	listentry = ggz_list_search_alt(nsslist, &entrycmp, list_entry_compare);

	if(listentry)
	{
		entry = ggz_list_get_data(listentry);
		fdtls = entry->fdtls;
		PR_Shutdown(fdtls, PR_SHUTDOWN_BOTH);
		PR_Close(fdtls);
		ggz_list_delete_entry(nsslist, listentry);
		return 1;
	}

	ggz_debug(GGZ_SOCKET_DEBUG, "TLS/NSS: disabled");

	return 0;
}

size_t ggz_tls_write(int fd, void *ptr, size_t n)
{
	struct list_entry *entry;
	struct list_entry entrycmp;
	PRFileDesc *fdtls;
	GGZListEntry *listentry;

	entrycmp.fd = fd;
	listentry = ggz_list_search_alt(nsslist, &entrycmp, list_entry_compare);

	if(listentry)
	{
		entry = ggz_list_get_data(listentry);
		fdtls = entry->fdtls;

		ggz_debug(GGZ_SOCKET_DEBUG, "TLS/NSS: write on TLS socket %p for fd %i", entry, fd);

		return PR_Write(fdtls, ptr, n);
	}
	else
	{
		return write(fd, ptr, n);
	}
}

size_t ggz_tls_read(int fd, void *ptr, size_t n)
{
	struct list_entry *entry;
	struct list_entry entrycmp;
	PRFileDesc *fdtls;
	GGZListEntry *listentry;

	entrycmp.fd = fd;
	listentry = ggz_list_search_alt(nsslist, &entrycmp, list_entry_compare);

	if(listentry)
	{
		entry = ggz_list_get_data(listentry);
		fdtls = entry->fdtls;

		ggz_debug(GGZ_SOCKET_DEBUG, "TLS/NSS: read on TLS socket %p for fd %i", entry, fd);
	
		return PR_Read(fdtls, ptr, n);
	}
	else
	{
		return read(fd, ptr, n);
	}
}

#endif

