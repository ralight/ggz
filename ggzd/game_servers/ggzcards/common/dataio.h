/********************************************************************** 
 Copyright (C) 1996-2006 the Freeciv Dev Team and the GGZ Dev Team
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   $Id: net_common.c 4046 2002-04-22 00:04:41Z jdorje $
***********************************************************************/
#ifndef __DATAIO_H__
#define __DATAIO_H__

#include <stdbool.h>

struct dataio;

/* Initializers. */
struct dataio *dio_new(int socket);
void dio_free(struct dataio *dio);

void dio_set_writeable_callback(struct dataio *dio,
				void (writeable_callback)(struct dataio *,
							  bool));

/* Network r/w functions. */
int dio_read_data(struct dataio *dio, void (read_callback)(struct dataio *));
int dio_write_data(struct dataio *dio);

bool dio_is_write_pending(const struct dataio *dio);
int dio_get_socket(const struct dataio *dio);

void dio_start_packet(struct dataio *dio);
void dio_end_packet(struct dataio *dio);

/* gets */
void dio_get_char(struct dataio *dio, char *dest);
void dio_get_int(struct dataio *dio, int *dest);

void dio_get_uint8(struct dataio *dio, int *dest);
void dio_get_uint16(struct dataio *dio, int *dest);
void dio_get_uint32(struct dataio *dio, unsigned int *dest);

void dio_get_sint8(struct dataio *dio, int *dest);
void dio_get_sint16(struct dataio *dio, int *dest);
void dio_get_sint32(struct dataio *dio, signed int *dest);

void dio_get_bool8(struct dataio *dio, bool *dest);
void dio_get_bool32(struct dataio *dio, bool *dest);
void dio_get_memory(struct dataio *dio, void *dest, size_t dest_size);
void dio_get_string(struct dataio *dio, char *dest, size_t max_dest_size);
void dio_get_string_alloc(struct dataio *dio, char **dest);

/* puts */
void dio_put_char(struct dataio *dio, char dest);
void dio_put_int(struct dataio *dio, int dest);

void dio_put_uint8(struct dataio *dio, int value);
void dio_put_uint16(struct dataio *dio, int value);
void dio_put_uint32(struct dataio *dio, int value);

void dio_put_sint8(struct dataio *dio, int value);
void dio_put_sint16(struct dataio *dio, int value);
void dio_put_sint32(struct dataio *dio, int value);

void dio_put_bool8(struct dataio *dio, bool value);
void dio_put_bool32(struct dataio *dio, bool value);

void dio_put_memory(struct dataio *dio, const void *value, size_t size);
void dio_put_string(struct dataio *dio, const char *value);

#endif  /* FC__PACKETS_H */
