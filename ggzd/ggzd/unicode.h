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

bool username_allowed(const char *str);
bool init_unicode(const char *policyfile);

#endif
