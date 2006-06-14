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
***********************************************************************/

/*
 * The DataIO module provides a system independent (endianess and
 * sizeof(int) independent) way to write and read data. It supports
 * multiple datas which are collected in a buffer. It provides
 * recognition of error cases like "not enough space" or "not enough
 * data".
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_WINSOCK
#include <winsock.h>
#endif

#include <ggz.h>

#include "dataio.h"


#ifndef MIN
#  define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

struct dataio {
  int fd;

  bool handling;

  struct {
    char *buf; /* The buffer. */
    size_t bufsz; /* Size of the buffer. */

    size_t start; /* The start of the current packet. */
    size_t current; /* The current position (end of the unfinished packet) */

    size_t writeloc; /* The last position written to the network. */
  } output;
  struct {
    char *buf; /* The buffer. */
    size_t bufsz; /* Size of the buffer. */

    size_t start; /* Start of the currently packet. */
    size_t current; /* Current position in the current packet. */
    size_t final; /* End position of this packet. */

    size_t readloc; /* The last position read from the network. */
  } input;
};

/***************************************************************
  Set socket to non-blocking.
***************************************************************/
static void my_nonblock(int sockfd)
{
#ifdef HAVE_FCNTL
  int f_set;

  if ((f_set=fcntl(sockfd, F_GETFL)) == -1) {
    freelog(LOG_ERROR, "fcntl F_GETFL failed: %s", mystrerror());
  }

  f_set |= O_NONBLOCK;

  if (fcntl(sockfd, F_SETFL, f_set) == -1) {
    freelog(LOG_ERROR, "fcntl F_SETFL failed: %s", mystrerror());
  }
#else
#ifdef HAVE_IOCTL
  long value=1;

  if (ioctl(sockfd, FIONBIO, (char*)&value) == -1) {
    freelog(LOG_ERROR, "ioctl failed: %s", mystrerror());
  }
#else
  ggz_error_msg("Nonblocking sockets are not available.");
#endif
#endif
}

struct dataio *dio_new(int socket)
{
  struct dataio *dio = ggz_malloc(sizeof(*dio));
  const size_t bufsz;

  dio->fd = socket;
  my_nonblock(socket);

  dio->handling = false;

  dio->output.buf = ggz_malloc(bufsz);
  dio->output.bufsz = bufsz;
  dio->output.start = dio->output.current = dio->output.writeloc = 0;

  dio->input.buf = ggz_malloc(bufsz);
  dio->input.bufsz = bufsz;
  dio->input.start = dio->input.current = dio->input.final = 0;

  return dio;
}

void dio_free(struct dataio *dio)
{
  ggz_free(dio->output.buf);
  ggz_free(dio->input.buf);
  ggz_free(dio);
}

/* Takes packets from the input buffer and parses them. */
static void consume_packets(struct dataio *dio,
			    void (read_callback)(struct dataio *))
{
  assert(dio->input.final == dio->input.start);
  assert(dio->input.start == dio->input.current);

  while (dio->input.readloc - dio->input.start > 2) {
    uint16_t pack_size;

    assert(sizeof(pack_size) == 2);
    memcpy(&pack_size, dio->input.buf + dio->input.start, sizeof(pack_size));
    pack_size = ntohs(pack_size);

    if (dio->input.start + pack_size > dio->input.readloc) {
      /* Not enough data. */
      break;
    }

    /* Advance to next packet and read it. */
    dio->input.final = dio->input.start + pack_size;
    dio->input.current = sizeof(pack_size);
    (read_callback)(dio);
    dio->input.start = dio->input.final;
    dio->input.current = dio->input.final;
  }

  /* Rewind the buffer. */
  if (dio->input.readloc == dio->input.start) {
    dio->input.start = 0;
    dio->input.final = 0;
    dio->input.current = 0;
    dio->input.readloc = 0;
  } else if (dio->input.start >= dio->input.bufsz / 2) {
    size_t diff = dio->input.start;

    dio->input.start -= diff;
    dio->input.final -= diff;
    dio->input.current -= diff;
    dio->input.readloc -= diff;

    memcpy(dio->input.buf, dio->input.buf + diff, dio->input.readloc);
  }
}

/* Network r/w functions. */
int dio_read_data(struct dataio *dio, void (read_callback)(struct dataio *))
{
  int nleft;
  char *ptr = dio->input.buf + dio->input.readloc;
  size_t nread;

  assert(!dio->handling);
  dio->handling = true;

  if (dio->input.bufsz < dio->input.readloc + 20) {
    dio->input.bufsz *= 2;
    dio->input.buf = ggz_realloc(dio->input.buf, dio->input.bufsz);
  }
  nleft = dio->input.bufsz - dio->input.readloc;

#ifdef HAVE_WINSOCK2_H
  nread = recv(dio->fd, ptr, nleft, 0);
#else
  nread = read(dio->fd, ptr, nleft);
#endif

  if (nread < 0) {
    dio->handling = false;
    return -1;
  }

  dio->input.readloc += nread;

  consume_packets(dio, read_callback);

  dio->handling = false;

  return 0;
}

int dio_write_data(struct dataio *dio)
{
  int nleft = dio->output.start - dio->output.writeloc;
  size_t nwritten;
  char *ptr = dio->output.buf + dio->output.writeloc;

  assert(!dio->handling);
  assert(dio->output.start == dio->output.current);
  dio->handling = true;

  assert(nleft >= 0);
  if (nleft <= 0) {
    dio->handling = false;
    return 0;
  }

#ifdef HAVE_WINSOCK2_H
  nwritten = send(dio->fd, ptr, nleft, 0);
#else
  nwritten = write(dio->fd, ptr, nleft);
#endif
  if (nwritten < 0) {
    dio->handling = false;
    return -1;
  }
  assert(nwritten <= nleft);
  dio->output.writeloc += nwritten;

  /* Rewind the buffer. */
  if (dio->output.writeloc == dio->output.current) {
    dio->output.writeloc = 0;
    dio->output.start = 0;
    dio->output.current = 0;
  } else if (dio->output.writeloc > dio->output.bufsz / 2) {
    size_t diff = dio->output.writeloc;

    dio->output.writeloc -= diff;
    dio->output.start -= diff;
    dio->output.current -= diff;

    memmove(dio->output.buf, dio->output.buf + diff, dio->output.current);
  }

  dio->handling = false;

  return 0;
}

bool dio_is_write_pending(const struct dataio *dio)
{
  return (dio->output.start > dio->output.writeloc);
}

int dio_get_socket(const struct dataio *dio)
{
  return dio->fd;
}

void dio_start_packet(struct dataio *dio)
{

  assert(!dio->handling);
  dio->handling = true;

  dio->output.current = dio->output.start + 2;
}

void dio_end_packet(struct dataio *dio)
{
  uint16_t pack_size = dio->output.current - dio->output.start;

  assert(sizeof(pack_size) == 2);
  pack_size = htons(pack_size);
  memcpy(dio->output.buf + dio->output.start, &pack_size, sizeof(pack_size));

  dio->output.start = dio->output.current;

  dio->handling = false;
}

/**************************************************************************
  Returns true iff the output has size bytes available.
**************************************************************************/
static void ensure_output_data(struct dataio *dio, size_t size)
{
  int diff = size - (dio->output.bufsz - dio->output.current);

  assert(dio->output.start <= dio->output.current);
  assert(dio->output.current <= dio->output.bufsz);
  if (diff > 0) {
    dio->output.buf = ggz_realloc(dio->output.buf, dio->output.bufsz + diff);
  }
}

/**************************************************************************
  Returns true iff the input contains size unread bytes.
**************************************************************************/
static bool enough_input_data(struct dataio *dio, size_t size)
{
  assert(dio->input.start <= dio->input.current);
  assert(dio->input.current <= dio->input.final);
  assert(dio->input.final <= dio->input.bufsz);

  return (dio->input.current + size <= dio->input.final);
}

#if 0
/**************************************************************************
  Initializes the output to the given output buffer and the given
  buffer size.
**************************************************************************/
void dio_output_init(struct dataio *dio, void *destination,
		     size_t dest_size)
{
  dio->dest = destination;
  dio->dest_size = dest_size;
  dio->current = 0;
  dio->used = 0;
  dio->too_short = FALSE;
}

/**************************************************************************
  Return the maximum number of bytes used.
**************************************************************************/
size_t dio_output_used(struct dataio *dio)
{
  return dio->used;
}

/**************************************************************************
  Rewinds the stream so that the put-functions start from the
  beginning.
**************************************************************************/
void dio_output_rewind(struct dataio *dio)
{
  dio->current = 0;
}

/**************************************************************************
  Initializes the input to the given input buffer and the given
  number of valid input bytes.
**************************************************************************/
void dio_input_init(struct dataio *dio, const void *src, size_t src_size)
{
  dio->src = src;
  dio->src_size = src_size;
  dio->current = 0;
  dio->too_short = FALSE;
  dio->bad_string = FALSE;
#if 0
  dio->bad_bit_string = FALSE;
#endif
}

/**************************************************************************
  Rewinds the stream so that the get-functions start from the
  beginning.
**************************************************************************/
void dio_input_rewind(struct dataio *dio)
{
  dio->current = 0;
}

/**************************************************************************
  Return the number of unread bytes.
**************************************************************************/
size_t dio_input_remaining(struct dataio *dio)
{
  return dio->src_size - dio->current;
}
#endif

/* Convenience function corresponding to easysock. */
void dio_put_char(struct dataio *dio, char dest)
{
  assert(sizeof(dest) == 1);
  dio_put_memory(dio, &dest, sizeof(dest));
}

/* Convenience function corresponding to easysock. */
void dio_put_int(struct dataio *dio, int dest)
{
  dio_put_sint32(dio, dest);
}

/**************************************************************************
...
**************************************************************************/
void dio_put_uint8(struct dataio *dio, int value)
{
  uint8_t x = value;

  assert(sizeof(x) == 1);
  dio_put_memory(dio, &x, 1);
}

/**************************************************************************
...
**************************************************************************/
void dio_put_uint16(struct dataio *dio, int value)
{
  uint16_t x = htons(value);

  assert(sizeof(x) == 2);
  dio_put_memory(dio, &x, 2);
}

/**************************************************************************
...
**************************************************************************/
void dio_put_uint32(struct dataio *dio, int value)
{
  uint32_t x = htonl(value);

  assert(sizeof(x) == 4);
  dio_put_memory(dio, &x, 4);
}

void dio_put_sint8(struct dataio *dio, int value)
{
  dio_put_uint8(dio, value);
}

void dio_put_sint16(struct dataio *dio, int value)
{
  dio_put_uint16(dio, value);
}

void dio_put_sint32(struct dataio *dio, int value)
{
  dio_put_uint32(dio, value);
}

/**************************************************************************
...
**************************************************************************/
void dio_put_bool8(struct dataio *dio, bool value)
{
  if (value != true && value != false) {
    ggz_error_msg("Trying to put a non-boolean: %d", (int) value);
    value = false;
  }

  dio_put_uint8(dio, value ? 1 : 0);
}

/**************************************************************************
...
**************************************************************************/
void dio_put_bool32(struct dataio *dio, bool value)
{
  if (value != true && value != false) {
    ggz_error_msg("Trying to put a non-boolean: %d", (int) value);
    value = false;
  }

  dio_put_uint32(dio, value ? 1 : 0);
}

#if 0
/**************************************************************************
...
**************************************************************************/
void dio_put_uint8_vec8(struct dataio *dio, int *values, int stop_value)
{
  size_t count;

  for (count = 0; values[count] != stop_value; count++) {
    /* nothing */
  }

  if (enough_space(dio, 1 + count)) {
    size_t i;

    dio_put_uint8(dio, count);

    for (i = 0; i < count; i++) {
      dio_put_uint8(dio, values[i]);
    }
  }
}

/**************************************************************************
...
**************************************************************************/
void dio_put_uint16_vec8(struct dataio *dio, int *values, int stop_value)
{
  size_t count;

  for (count = 0; values[count] != stop_value; count++) {
    /* nothing */
  }

  if (enough_space(dio, 1 + 2 * count)) {
    size_t i;

    dio_put_uint8(dio, count);

    for (i = 0; i < count; i++) {
      dio_put_uint16(dio, values[i]);
    }
  }
}
#endif

/**************************************************************************
...
**************************************************************************/
void dio_put_memory(struct dataio *dio, const void *value, size_t size)
{
  ensure_output_data(dio, size);

  memcpy(dio->output.buf + dio->output.current, value, size);
  dio->output.current += size;
}

/**************************************************************************
...
**************************************************************************/
void dio_put_string(struct dataio *dio, const char *value)
{
  /* Corresponds exactly to ggz_write_string. */
  unsigned int size = strlen(value) + 1;

  assert(sizeof(char) == 1);
  dio_put_sint32(dio, size);
  dio_put_memory(dio, value, size);
}

#if 0
/**************************************************************************
...
**************************************************************************/
void dio_put_bit_string(struct dataio *dio, const char *value)
{
  /* Note that size_t is often an unsigned type, so we must be careful
   * with the math when calculating 'bytes'. */
  size_t bits = strlen(value), bytes;
  size_t max = (unsigned short)(-1);

  if (bits > max) {
    ggz_error_msg("Bit string too long: %lu bits.", (unsigned long)bits);
    assert(false);
    bits = max;
  }
  bytes = (bits + 7) / 8;

  if (enough_space(dio, bytes + 1)) {
    size_t i;

    dio_put_uint16(dio, bits);

    for (i = 0; i < bits;) {
      int bit, data = 0;

      for (bit = 0; bit < 8 && i < bits; bit++, i++) {
	if (value[i] == '1') {
	  data |= (1 << bit);
	}
      }
      dio_put_uint8(dio, data);
    }
  }
}
#endif

/* Convenience function corresponding to easysock. */
void dio_get_char(struct dataio *dio, char *dest)
{
  assert(sizeof(*dest) == 1);
  dio_get_memory(dio, dest, sizeof(*dest));
}

/* Convenience function corresponding to easysock. */
void dio_get_int(struct dataio *dio, int *dest)
{
  dio_get_sint32(dio, dest);
}

/**************************************************************************
...
**************************************************************************/
void dio_get_uint8(struct dataio *dio, int *dest)
{
  uint8_t x;

  assert(sizeof(x) == 1);
  dio_get_memory(dio, &x, 1);
}

/**************************************************************************
...
**************************************************************************/
void dio_get_uint16(struct dataio *dio, int *dest)
{
  uint16_t x;

  assert(sizeof(x) == 2);
  dio_get_memory(dio, &x, 2);
  *dest = ntohs(x);
}

/**************************************************************************
...
**************************************************************************/
void dio_get_uint32(struct dataio *dio, unsigned int *dest)
{
  uint32_t x;

  assert(sizeof(x) == 4);
  dio_get_memory(dio, &x, 4);
  *dest = ntohl(x);
}

/**************************************************************************
...
**************************************************************************/
void dio_get_bool8(struct dataio *dio, bool * dest)
{
  int ival;

  dio_get_uint8(dio, &ival);

  if (ival != 0 && ival != 1) {
    ggz_error_msg("Received value isn't boolean: %d", ival);
    ival = 1;
  }

  *dest = (ival != 0);
}

/**************************************************************************
...
**************************************************************************/
void dio_get_bool32(struct dataio *dio, bool * dest)
{
  unsigned int ival = 0;

  dio_get_uint32(dio, &ival);

  if (ival != 0 && ival != 1) {
    ggz_error_msg("Received value isn't boolean: %d", ival);
    ival = 1;
  }

  *dest = (ival != 0);
}

/**************************************************************************
...
**************************************************************************/
void dio_get_sint8(struct dataio *dio, int *dest)
{
  int tmp;

  dio_get_uint8(dio, &tmp);
  if (tmp > 0x7f) {
    tmp -= 0x100;
  }
  *dest = tmp;
}

/**************************************************************************
...
**************************************************************************/
void dio_get_sint16(struct dataio *dio, int *dest)
{
  int tmp;

  dio_get_uint16(dio, &tmp);
  if (tmp > 0x7fff) {
    tmp -= 0x10000;
  }
  *dest = tmp;
}

void dio_get_sint32(struct dataio *dio, signed int *dest)
{
  unsigned int tmp;

  dio_get_uint32(dio, &tmp);
  *dest = tmp;
}

/**************************************************************************
...
**************************************************************************/
void dio_get_memory(struct dataio *dio, void *dest, size_t dest_size)
{
  if (enough_input_data(dio, dest_size)) {
    memcpy(dest, dio->input.buf + dio->input.current, dest_size);
  } else {
    memset(dest, 0, dest_size);
  }

  dio->input.current = MIN(dio->input.current + dest_size, dio->input.final);
}

/**************************************************************************
...
**************************************************************************/
void dio_get_string(struct dataio *dio, char *dest, size_t max_dest_size)
{
  /* Corresponds exactly to ggz_read_string */
  unsigned int size;

  dio_get_uint32(dio, &size);
  if (max_dest_size >= size) {
    dio_get_memory(dio, dest, size);
    dest[size - 1] = '\0';
  } else {
    unsigned int rest = size - max_dest_size;

    dio_get_memory(dio, dest, max_dest_size);
    dest[max_dest_size - 1] = '\0';
    dio->input.current = MIN(dio->input.current + rest, dio->input.final);
  }
}

/**************************************************************************
  VERY UNSAFE TO USE THIS FUNCTION.  Only use for a trusted connection.
**************************************************************************/
void dio_get_string_alloc(struct dataio *dio, char **dest)
{
  /* Corresponds exactly to ggz_read_string_alloc. */
  unsigned int size;

  dio_get_uint32(dio, &size);
  *dest = ggz_malloc(size);
  dio_get_memory(dio, *dest, size);
  dest[size - 1] = '\0';
}

#if 0
/**************************************************************************
...
**************************************************************************/
void dio_get_bit_string(struct dataio *dio, char *dest,
			size_t max_dest_size)
{
  int npack = 0;		/* number claimed in packet */
  int i;			/* iterate the bytes */

  assert(dest != NULL && max_dest_size > 0);

  if (!enough_data(dio, 1)) {
    dest[0] = '\0';
    return;
  }

  dio_get_uint16(dio, &npack);
  if (npack >= max_dest_size) {
      ggz_error_msg("Have size for %lu, got %d",
		    (unsigned long)max_dest_size, npack);
    dio->bad_bit_string = true;
    dest[0] = '\0';
    return;
  }

  for (i = 0; i < npack;) {
    int bit, byte_value;

    dio_get_uint8(dio, &byte_value);
    for (bit = 0; bit < 8 && i < npack; bit++, i++) {
      if (TEST_BIT(byte_value, bit)) {
	dest[i] = '1';
      } else {
	dest[i] = '0';
      }
    }
  }

  dest[npack] = '\0';

  if (dio->too_short) {
    dio->bad_bit_string = true;
  }
}

/**************************************************************************
...
**************************************************************************/
void dio_get_uint8_vec8(struct dataio *dio, int **values, int stop_value)
{
  int count, inx;

  dio_get_uint8(dio, &count);
  if (values) {
    *values = ggz_malloc((count + 1) * sizeof(**values));
  }
  for (inx = 0; inx < count; inx++) {
    dio_get_uint8(dio, values ? &((*values)[inx]) : NULL);
  }
  if (values) {
    (*values)[inx] = stop_value;
  }
}

/**************************************************************************
...
**************************************************************************/
void dio_get_uint16_vec8(struct dataio *dio, int **values, int stop_value)
{
  int count, inx;

  dio_get_uint8(dio, &count);
  if (values) {
    *values = ggz_malloc((count + 1) * sizeof(**values));
  }
  for (inx = 0; inx < count; inx++) {
    dio_get_uint16(dio, values ? &((*values)[inx]) : NULL);
  }
  if (values) {
    (*values)[inx] = stop_value;
  }
}
#endif
