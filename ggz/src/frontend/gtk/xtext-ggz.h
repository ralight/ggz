/*
 * File: xtext-ggz.h
 * Author: Jason Short
 * Project: GGZ GTK Client
 * $Id: xtext-ggz.h 6981 2005-03-11 07:35:03Z jdorje $
 *
 * This file is included from xtext.c to add additional ggz-specific
 * directives.
 *
 * Copyright (C) 2005 GGZ Development Team
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


/*
 * xtext.c and xtext.h was taken from xchat-2.4.1.  I then made the following
 * modifications to xtext.c:
 *
 * - Remove old code:
 *     #ifdef XCHAT
 *     #include "../../config.h"
 *     #else
 *     #define USE_XLIB
 *     #endif
 * - Add #include "xtext-ggz.h" in its place.
 * - Some quick fixes to avoid compiler warnings.
 * - Change the colors (XTEXT_BG, etc.) in xtext.h to match the colors[]
 *   array in chat.c (in future maybe palette.[ch] could be included as
 *   well).
 *
 * This file therefore contains extra code that xtext.c and xtext.h will
 * need.  It's contained here so that large changes to xtext.c aren't
 * necessary.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#undef XCHAT
#ifndef WIN32
#define USE_XLIB
#endif

#include <strings.h> /* For strcasecmp */
