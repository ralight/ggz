/* Test program for dataio functionality. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ggz.h>
#include <ggz_dio.h>

static int numpackets = 0;

static void read_callback(struct dataio *dio, void *userdata)
{
  int i;
  bool b;
  char c;
  char *s;

  numpackets++;
  printf("Got a packet.\n");

  dio_get_string_alloc(dio, &s);
  if (!s || strcmp(s, "testing ggz testing") != 0) {
    printf("dio_get_string error (%s)\n", s);
    exit(-1);
  }
  ggz_free(s);

  dio_get_int(dio, &i);
  if (i != 5) {
    printf("dio_get_int error.\n");
    exit(-1);
  }

  dio_get_char(dio, &c);
  if (c != 'c') {
    printf("dio_get_char error.\n");
    exit(-1);
  }

  dio_get_bool8(dio, &b);
  if (!b) {
    printf("dio_get_bool8 error.\n");
    exit(-1);
  }
}


int main(int argc, char **argv)
{
  int sockets[2];
  struct dataio *dio[2];
  int err;

  socketpair(PF_LOCAL, SOCK_STREAM, 0, sockets);

  dio[0] = dio_new(sockets[0]);
  dio[1] = dio_new(sockets[1]);

  dio_packet_start(dio[0]);
  dio_put_string(dio[0], "testing ggz testing");
  dio_put_int(dio[0], 5);
  dio_put_char(dio[0], 'c');
  dio_put_bool8(dio[0], true);
  dio_packet_end(dio[0]);

  dio_flush(dio[0]);
  dio_free(dio[0]);
  close(sockets[0]);

  do {
    err = dio_read_data(dio[1], read_callback, NULL);
  } while (err > 0);
  if (err < 0) {
    printf("Socket error: %s.\n", strerror(errno));
  }

  if (numpackets != 1) {
    printf("error: %d packets read (expecting 1).\n", numpackets);
    return -1;
  }

  return 0;
}
