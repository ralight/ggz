/*
 * File: unicode.h
 * Author: GGZ Development Team
 * Project: GGZ Server
 * Date: 2007-03-01
 * Desc: Unicode handling such as policies on allowed usernames
 * $Id: reconfiguration.h 8467 2006-08-04 01:58:46Z jdorje $
 *
 * Copyright (C) 2007 GGZ Development Team
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ggzd.h"

#ifndef _GGZ_UNICODE_H
#define _GGZ_UNICODE_H

/* Initialize all unicode-dependent settings from a policy file */
/* The default file is 'username.poliy' */
/* Returns true if file could be loaded, false otherwise */
bool init_unicode(const char *policyfile);

/* Check if the given username is allowed according to the policy */
/* True means it's allowed, false means it's prohibited */
bool username_allowed(const char *str);

/* Return a canonicalized version of a username */
/* Implements RFC 3454 ("stringprep") for ambiguity elimination */
/* Does not yet specifically address RFC 4013 ("SASLprep") */
/* This function should be called before username_allowed */
/* NULL is returned if the name could not be canonicalized */
char *username_canonical(const char *username);

#endif
