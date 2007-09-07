/*
 * Ruby bindings for libggzmod
 * Copyright (C) 2007 Josef Spillner <josef@ggzgamingzone.org>
 * Published under GNU GPL conditions
 */

/* Necessary include files */

#include <ggzmod.h>
#include <ruby.h>

/* Global Variables */

static VALUE cTEST;
static int ret;
static const char *rets;
static VALUE retv;
static GGZMod *ggzmod;

/* Callback handlers */

static void cb_handler ( GGZMod *mod, GGZModEvent event, const void *data )
{
	VALUE dataval = Qnil;

	if ( event == GGZMOD_EVENT_SERVER )
	{
		int fd = *( int * ) data;
		dataval = INT2FIX ( fd );
	}

	if ( event == GGZMOD_EVENT_CHAT )
	{
		GGZChat *chat = ( GGZChat * ) data;
		dataval = rb_str_new2 ( chat->player );
		VALUE sep = rb_str_new2 ( ": " );
		VALUE msg = rb_str_new2 ( chat->message );
		rb_str_concat ( dataval, sep );
		rb_str_concat ( dataval, msg );
	}

	rb_funcall( cTEST, rb_intern("ggzmod_handler"), 2, INT2FIX ( event ), dataval );
}

static void init_ggz ()
{
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_STATE, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_SERVER, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_PLAYER, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_SEAT, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_SPECTATOR_SEAT, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_CHAT, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_STATS, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_INFO, cb_handler );
	ggzmod_set_handler ( ggzmod, GGZMOD_EVENT_ERROR, cb_handler );
}

/* Methods for the ggzmod object */

static VALUE t_is_ggz_mode ( VALUE self )
{
	retv = INT2FIX ( ggzmod_is_ggz_mode () );

	return retv;
}

static VALUE t_init ( VALUE self )
{
	ggzmod = ggzmod_new ( GGZMOD_GAME );

	return self;
}

static VALUE t_connect ( VALUE self )
{
	ret = ggzmod_connect ( ggzmod );

	init_ggz ();

	return INT2FIX ( ret );
}

static VALUE t_disconnect ( VALUE self )
{
	ret = ggzmod_disconnect ( ggzmod );

	return INT2FIX ( ret );
}

static VALUE t_dispatch ( VALUE self )
{
	ret = ggzmod_dispatch ( ggzmod );
	if ( ret == -1 )
		return Qnil;
	else
		return INT2FIX ( ret );
}

static VALUE t_get_state ( VALUE self )
{
	retv = INT2FIX ( ggzmod_get_state ( ggzmod ) );

	return retv;
}

static VALUE t_get_control_fd ( VALUE self )
{
	retv = INT2FIX ( ggzmod_get_fd ( ggzmod ) );

	return retv;
}

static VALUE t_get_server_fd ( VALUE self )
{
	retv = INT2FIX ( ggzmod_get_server_fd ( ggzmod ) );

	return retv;
}

static VALUE t_get_num_seats ( VALUE self )
{
	retv = INT2FIX ( ggzmod_get_num_seats ( ggzmod ) );

	return retv;
}

static VALUE t_get_num_spectators ( VALUE self )
{
	retv = INT2FIX ( ggzmod_get_num_spectator_seats ( ggzmod ) );

	return retv;
}

static VALUE t_get_seat_name ( VALUE self, VALUE seat )
{
	rets = ggzmod_get_seat ( ggzmod, FIX2INT ( seat ) ).name;
	if ( ! rets )
	{
		retv = Qnil;
	}
	else
	{
		retv = rb_str_new2 ( rets );
	}

	return retv;
}

static VALUE t_get_spectator_name ( VALUE self, VALUE seat )
{
	rets = ggzmod_get_spectator_seat ( ggzmod, FIX2INT ( seat ) ).name;
	if ( ! rets )
	{
		retv = Qnil;
	}
	else
	{
		retv = rb_str_new2 ( rets );
	}

	return retv;
}

static VALUE t_get_seat_type ( VALUE self, VALUE seat )
{
	ret = ggzmod_get_seat ( ggzmod, FIX2INT ( seat ) ).type;
	retv = INT2FIX ( ret );

	return retv;
}

static VALUE t_get_player_seat_number ( VALUE self )
{
	int is_spectator, seat_num;
	if ( ggzmod_get_player ( ggzmod, &is_spectator, &seat_num ) )
		return INT2FIX ( seat_num );
	else
		return Qnil;
}

static VALUE t_get_player_is_spectator ( VALUE self )
{
	int is_spectator, seat_num;
	if ( ggzmod_get_player ( ggzmod, &is_spectator, &seat_num ) )
		return INT2FIX ( is_spectator );
	else
		return Qnil;
}

/* Module initialization */

static void init_constants ( VALUE self )
{
	/* This is shared with ruggzdmod (because it's actually from libggz) */
	rb_define_const ( self, "SEATNONE", INT2NUM ( GGZ_SEAT_NONE ) );
	rb_define_const ( self, "SEATOPEN", INT2NUM ( GGZ_SEAT_OPEN ) );
	rb_define_const ( self, "SEATBOT", INT2NUM ( GGZ_SEAT_BOT ) );
	rb_define_const ( self, "SEATPLAYER", INT2NUM ( GGZ_SEAT_PLAYER ) );
	rb_define_const ( self, "SEATRESERVED", INT2NUM ( GGZ_SEAT_RESERVED ) );
	rb_define_const ( self, "SEATABANDONED", INT2NUM ( GGZ_SEAT_ABANDONED ) );

	/* States from ggzmod */
	rb_define_const ( self, "STATECREATED", INT2FIX ( GGZMOD_STATE_CREATED ) );
	rb_define_const ( self, "STATECONNECTED", INT2FIX ( GGZMOD_STATE_CONNECTED ) );
	rb_define_const ( self, "STATEWAITING", INT2FIX ( GGZMOD_STATE_WAITING ) );
	rb_define_const ( self, "STATEPLAYING", INT2FIX ( GGZMOD_STATE_PLAYING ) );
	rb_define_const ( self, "STATEDONE", INT2FIX ( GGZMOD_STATE_DONE ) );

	/* Events in ggzmod */
	rb_define_const ( self, "EVENTSTATE", INT2FIX ( GGZMOD_EVENT_STATE ) );
	rb_define_const ( self, "EVENTSERVER", INT2FIX ( GGZMOD_EVENT_SERVER ) );
	rb_define_const ( self, "EVENTPLAYER", INT2FIX ( GGZMOD_EVENT_PLAYER ) );
	rb_define_const ( self, "EVENTSEAT", INT2FIX ( GGZMOD_EVENT_SEAT ) );
	rb_define_const ( self, "EVENTSPECTATOR", INT2FIX ( GGZMOD_EVENT_SPECTATOR_SEAT ) );
	rb_define_const ( self, "EVENTCHAT", INT2FIX ( GGZMOD_EVENT_CHAT ) );
	rb_define_const ( self, "EVENTSTATS", INT2FIX ( GGZMOD_EVENT_STATS ) );
	rb_define_const ( self, "EVENTINFO", INT2FIX ( GGZMOD_EVENT_INFO ) );
	rb_define_const ( self, "EVENTERROR", INT2FIX ( GGZMOD_EVENT_ERROR ) );
}

static void init_methods ( VALUE self )
{
	rb_define_method ( self, "is_ggz_mode", t_is_ggz_mode, 0 );

	rb_define_method ( self, "initialize", t_init, 0 );
	rb_define_method ( self, "connect", t_connect, 0 );
	rb_define_method ( self, "disconnect", t_disconnect, 0 );
	rb_define_method ( self, "dispatch", t_dispatch, 0 );

	rb_define_method ( self, "get_state", t_get_state, 0 );
	rb_define_method ( self, "get_control_fd", t_get_control_fd, 0 );
	rb_define_method ( self, "get_server_fd", t_get_server_fd, 0 );

	rb_define_method ( self, "get_num_seats", t_get_num_seats, 0 );
	rb_define_method ( self, "get_num_spectators", t_get_num_spectators, 0 );
	rb_define_method ( self, "get_seat_name", t_get_seat_name, 1 );
	rb_define_method ( self, "get_seat_type", t_get_seat_type, 1 );
	rb_define_method ( self, "get_spectator_name", t_get_spectator_name, 1 );
	rb_define_method ( self, "get_player_seat_number", t_get_player_seat_number, 0 );
	rb_define_method ( self, "get_player_is_spectator", t_get_player_is_spectator, 0 );

	/* missing: request_XXX, table-rankings, infos, (top-rankings) */
}

void Init_GGZMod ()
{
	cTEST = rb_define_class ( "GGZMod", rb_cObject );

	init_methods ( cTEST );
	init_constants ( cTEST );
}
