/* Generated by GGZComm/ruby version 0.2 */
/* Protocol 'tictactoe', version '4' */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <ggz_dio.h>
#include <ggz.h>
#include <ggz_common.h>

#include "net.h"

static notifier_func_type notifier_func = NULL;
static error_func_type error_func = NULL;
static int requirelink = 0;
static int nextlink;

static void ggzcomm_error(void);

static void ggzcomm_sndmove(GGZDataIO *dio)
{
	ggz_dio_get_int(dio, &variables.move_c);
	if(notifier_func) (notifier_func)(sndmove);
}

static void ggzcomm_reqsync(GGZDataIO *dio)
{
	if(notifier_func) (notifier_func)(reqsync);
}

void ggzcomm_msgseat(GGZDataIO *dio)
{
	ggz_dio_packet_start(dio);
	ggz_dio_put_int(dio, msgseat);
	ggz_dio_put_int(dio, variables.num);
	ggz_dio_packet_end(dio);
}

void ggzcomm_msgplayers(GGZDataIO *dio)
{
	int i;

	ggz_dio_packet_start(dio);
	ggz_dio_put_int(dio, msgplayers);
	for (i = 0; i < 2; i++) {
		ggz_dio_put_int(dio, variables.seat[i]);
		if (variables.seat[i] != GGZ_SEAT_OPEN) {
			ggz_dio_put_string(dio, variables.name[i]);
		}
	}
	ggz_dio_packet_end(dio);
}

void ggzcomm_reqmove(GGZDataIO *dio)
{
	ggz_dio_packet_start(dio);
	ggz_dio_put_int(dio, reqmove);
	ggz_dio_packet_end(dio);

	requirelink = 1;
	nextlink = sndmove;
}

void ggzcomm_rspmove(GGZDataIO *dio)
{
	ggz_dio_packet_start(dio);
	ggz_dio_put_int(dio, rspmove);
	ggz_dio_put_char(dio, variables.status);
	ggz_dio_packet_end(dio);

	requirelink = 1;
	nextlink = sndmove;
}

void ggzcomm_msgmove(GGZDataIO *dio)
{
	ggz_dio_packet_start(dio);
	ggz_dio_put_int(dio, msgmove);
	ggz_dio_put_int(dio, variables.player);
	ggz_dio_put_int(dio, variables.move);
	ggz_dio_packet_end(dio);
}

void ggzcomm_sndsync(GGZDataIO *dio)
{
	int i;

	ggz_dio_packet_start(dio);
	ggz_dio_put_int(dio, sndsync);
	ggz_dio_put_char(dio, variables.turn);
	for (i = 0; i < 9; i++) {
		ggz_dio_put_char(dio, variables.space[i]);
	}
	ggz_dio_packet_end(dio);

	requirelink = 1;
	nextlink = reqsync;
}

void ggzcomm_msggameover(GGZDataIO *dio)
{
	ggz_dio_packet_start(dio);
	ggz_dio_put_int(dio, msggameover);
	ggz_dio_put_char(dio, variables.winner);
	ggz_dio_packet_end(dio);
}

static void ggzcomm_network_main_cb(GGZDataIO *dio, void *userdata)
{
	int opcode;
	enum client_to_server op;

	ggz_dio_get_int(dio, &opcode);
	op = opcode;

	if(requirelink) {
		if(opcode != nextlink) ggzcomm_error();
		requirelink = 0;
	}

	switch (op) {
		case sndmove:
			ggzcomm_sndmove(dio);
			break;
		case reqsync:
			ggzcomm_reqsync(dio);
			break;
	}
}

void ggzcomm_network_main(GGZDataIO *dio)
{
	ggz_dio_set_read_callback(dio, ggzcomm_network_main_cb, NULL);
	if (ggz_dio_read_data(dio) < 0) {
		ggzcomm_error();
	}
}

void ggzcomm_set_notifier_callback(notifier_func_type f)
{
	notifier_func = f;
}

void ggzcomm_set_error_callback(error_func_type f)
{
	error_func = f;
}

#if 0
void ggzcomm_set_dio(GGZDataIO *usedio)
{
	dio = usedio;
}

int ggzcomm_get_fd(void)
{
	return ggz_dio_get_socket(dio);
}
#endif

static void ggzcomm_error(void)
{
	if(error_func) (error_func)();
}

