/* Generated by GGZComm/ruby version 0.4 */
/* Protocol 'tictactoe', version '5' */
/* Do NOT edit this file! */

#include "net.h"
#include <stdlib.h>
#include <ggz.h>
#include <ggz_dio.h>

struct variables_t variables;

static notifier_func_type notifier_func = NULL;
static error_func_type error_func = NULL;
static int requirelink = 0;
static int nextlink;

static void ggzcomm_error(void);

static void ggzcomm_sndmove(GGZCommIO *io)
{
	ggz_dio_get_int(io->dio, &variables.move_c);
	if(notifier_func)
		(notifier_func)(sndmove);
}

static void ggzcomm_reqsync(GGZCommIO *io)
{
	if(notifier_func)
		(notifier_func)(reqsync);
}

void ggzcomm_reqmove(GGZCommIO *io)
{
	ggz_dio_packet_start(io->dio);
	ggz_dio_put_int(io->dio, reqmove);
	ggz_dio_packet_end(io->dio);
	ggz_dio_flush(io->dio);
}

void ggzcomm_rspmove(GGZCommIO *io)
{
	ggz_dio_packet_start(io->dio);
	ggz_dio_put_int(io->dio, rspmove);
	ggz_dio_put_char(io->dio, variables.status);
	ggz_dio_packet_end(io->dio);
	ggz_dio_flush(io->dio);
}

void ggzcomm_msgmove(GGZCommIO *io)
{
	ggz_dio_packet_start(io->dio);
	ggz_dio_put_int(io->dio, msgmove);
	ggz_dio_put_int(io->dio, variables.player);
	ggz_dio_put_int(io->dio, variables.move);
	ggz_dio_packet_end(io->dio);
	ggz_dio_flush(io->dio);
}

void ggzcomm_sndsync(GGZCommIO *io)
{
	int i1;

	ggz_dio_packet_start(io->dio);
	ggz_dio_put_int(io->dio, sndsync);
	ggz_dio_put_char(io->dio, variables.turn);
	for(i1 = 0; i1 < 9; i1++)
	{
		ggz_dio_put_char(io->dio, variables.space[i1]);
	}
	ggz_dio_packet_end(io->dio);
	ggz_dio_flush(io->dio);
}

void ggzcomm_msggameover(GGZCommIO *io)
{
	ggz_dio_packet_start(io->dio);
	ggz_dio_put_int(io->dio, msggameover);
	ggz_dio_put_char(io->dio, variables.winner);
	ggz_dio_packet_end(io->dio);
	ggz_dio_flush(io->dio);
}

static void ggzcomm_network_main_cb(GGZDataIO *dio, void *userdata)
{
	int opcode;

	GGZCommIO *io = userdata;
	ggz_dio_get_int(io->dio, &opcode);

	if(requirelink)
	{
		if(opcode != nextlink)
			ggzcomm_error();
		requirelink = 0;
	}

	switch(opcode)
	{
		case sndmove:
			ggzcomm_sndmove(io);
			break;
		case reqsync:
			ggzcomm_reqsync(io);
			break;
	}
}

void ggzcomm_network_main(GGZCommIO *io)
{
	ggz_dio_set_read_callback(io->dio, ggzcomm_network_main_cb, io);
	if(ggz_dio_read_data(io->dio) < 0)
		ggzcomm_error();
}

void ggzcomm_set_notifier_callback(notifier_func_type f)
{
	notifier_func = f;
}

void ggzcomm_set_error_callback(error_func_type f)
{
	error_func = f;
}

GGZCommIO *ggzcomm_io_allocate(int fd)
{
	GGZCommIO *io = ggz_malloc(sizeof(GGZCommIO));
	io->dio = ggz_dio_new(fd);
	return io;
}

void ggzcomm_io_free(GGZCommIO *io)
{
	ggz_dio_free(io->dio);
	ggz_free(io);
}

static void ggzcomm_error(void)
{
	if(error_func) (error_func)();
}

