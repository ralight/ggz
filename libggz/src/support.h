/**
 * File   support.h
 * Author GGZ Development Team
 * Project: Libggz
 * Date: 02/03/03
 * $Id: support.h 5838 2004-02-08 04:34:49Z jdorje $
 * 
 * Replacements for non-supported functions.
 *
 * Copyright (C) 2003 GGZ Development Team.
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

#ifndef HAVE_SUN_LEN
#define SUN_LEN(ptr) ((size_t)(((struct sockaddr_un *) 0)->sun_path) + strlen ((ptr)->sun_path))
#endif

#ifdef HAVE_MSGHDR_MSG_CONTROL
#ifndef HAVE_CMSG_ALIGN
#define CMSG_ALIGN(len) (((len) + sizeof (size_t) - 1) & ~(sizeof (size_t) - 1))
#endif
#ifndef HAVE_CMSG_SPACE
#define CMSG_SPACE(len) (CMSG_ALIGN (len) + CMSG_ALIGN (sizeof (struct cmsghdr)))
#endif
#ifndef HAVE_CMSG_LEN
#define CMSG_LEN(len)   (CMSG_ALIGN (sizeof (struct cmsghdr)) + (len))
#endif
#endif

#ifndef HAVE_STRSEP
#ifdef HAVE_STRTOK_R
#define strsep(stringp, delim) (strtok_r(*stringp, delim, stringp))
#else
/* Non-reentrant, non-threadsafe version. */
#define strsep(stringp, delim) (strtok(*stringp, delim))
#endif
#endif
