/*
 * GGZ Starterpack for C - Sample Server
 * Copyright (C) 2008 GGZ Development Team
 *
 * This code is made available as public domain; you can use it as a base
 * for your own game, as long as its licence is compatible with the libraries
 * you use.
 */

#include <stdio.h>
#include <ggz.h>

#include "game.h"
#include "net.h"

/* Data structure for sample server; more members can be added easily */
struct server_t {
	GGZdMod *ggz;
};

/* Global game variables */
static struct server_t sample_server;

/* GGZdMod callbacks - usually, there are a lot more */
static void game_handle_ggz_seat(GGZdMod *ggz, GGZdModEvent event, const void *data);

/* Setup game state */
void game_init(GGZdMod *ggzdmod)
{
	sample_server.ggz = ggzdmod;

	/* Setup GGZ game module callbacks - usually, there are a lot more */
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN, &game_handle_ggz_seat);

	/* Logging is very useful */
	ggzdmod_log(sample_server.ggz, "Game server initialized");
}

/* Callback for ggzdmod JOIN, LEAVE, and SEAT events */
static void game_handle_ggz_seat(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	const GGZSeat *old_seat = data;
	GGZSeat new_seat = ggzdmod_get_seat(ggz, old_seat->num);

	GGZCommIO *io = ggzcomm_io_allocate(new_seat.fd);
	ggzcomm_hello(io);
}

