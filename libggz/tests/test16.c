/*
 * File: test16.c
 * Author: GGZ Dev Team
 * Project: libggz
 * Date: 
 * $Id: easysock.c 8165 2006-06-11 23:01:42Z oojah $
 *
 * Copyright (C) 2006 GGZ Development Team
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ggz.h>

int main(int argc, char *argv[])
{
#ifdef USE_GCRYPT
	char *enc;
	char plain[7];
	char expected[10];

	strncpy(plain, "hello", 7);
	strncpy(expected, "68656c6c6f", 12);

	enc = ggz_base16_encode(plain, strlen(plain));
	printf("Original: %s\n", plain);
	printf("Encoded:  %s\n", enc);
	printf("Expected: %s\n", expected);

	if(strcmp(enc, expected)) return 1;

	ggz_free(enc);
#endif

	return 0;
}
