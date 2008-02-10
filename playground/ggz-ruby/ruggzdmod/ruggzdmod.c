/*
 * Ruby bindings for libggzdmod
 * Copyright (C) 2001, 2002, 2005 Josef Spillner <josef@ggzgamingzone.org>
 * Published under GNU GPL conditions
 */

/* Necessary include files */

#include <ggzdmod.h>
#include <ruby.h>

/* Global Variables */

static VALUE cTEST;
static int ret;
static const char *rets;
static VALUE retv;
static GGZdMod *ggzdmod;

/* Callback handlers */

static void cb_handler ( GGZdMod *mod, GGZdModEvent event, const void *data )
{
	VALUE dataval = Qnil;

	if ( event == GGZDMOD_EVENT_JOIN )
	{
		GGZSeat *oldseat = ( GGZSeat* ) data;
		dataval = INT2FIX ( oldseat->num );
	}

	if ( event == GGZDMOD_EVENT_LEAVE )
	{
		GGZSeat *oldseat = ( GGZSeat* ) data;
		dataval = INT2FIX ( oldseat->num );
	}

	if ( event == GGZDMOD_EVENT_SEAT )
	{
		GGZSeat *oldseat = ( GGZSeat* ) data;
		dataval = INT2FIX ( oldseat->num );
	}

	if ( event == GGZDMOD_EVENT_SPECTATOR_JOIN )
	{
		GGZSeat *oldseat = ( GGZSeat* ) data;
		dataval = INT2FIX ( oldseat->num );
	}

	if ( event == GGZDMOD_EVENT_SPECTATOR_LEAVE )
	{
		GGZSeat *oldseat = ( GGZSeat* ) data;
		dataval = INT2FIX ( oldseat->num );
	}

	if ( event == GGZDMOD_EVENT_SPECTATOR_SEAT )
	{
		GGZSeat *oldseat = ( GGZSeat* ) data;
		dataval = INT2FIX ( oldseat->num );
	}

	if ( event == GGZDMOD_EVENT_PLAYER_DATA )
	{
		int seatnum = * ( int * ) data;
		dataval = INT2FIX ( seatnum );
	}

	if ( event == GGZDMOD_EVENT_SPECTATOR_DATA )
	{
		int seatnum = * ( int * ) data;
		dataval = INT2FIX ( seatnum );
	}

	if ( event == GGZDMOD_EVENT_STATE )
	{
		GGZdModState state = * ( GGZdModState * ) data;
		dataval = INT2FIX ( state );
	}

	if ( event == GGZDMOD_EVENT_ERROR )
	{
		const char *errormsg = ( const char * ) data;
		if ( errormsg )
		{
			dataval = rb_str_new2 ( errormsg );
		}
	}

	rb_funcall( cTEST, rb_intern("ggzdmod_handler"), 2, INT2FIX ( event ), dataval );
}

static void init_ggz ()
{
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_STATE, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_JOIN, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_LEAVE, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_SEAT, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_SPECTATOR_SEAT, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_SPECTATOR_DATA, cb_handler );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_ERROR, cb_handler );
}

/* Methods for the ggzdmod object */

static VALUE t_is_ggz_mode ( VALUE self )
{
	retv = INT2FIX ( ggzdmod_is_ggz_mode () );

	return retv;
}

static VALUE t_init ( VALUE self )
{
	ggzdmod = ggzdmod_new ( GGZDMOD_GAME );

	return self;
}

static VALUE t_connect ( VALUE self )
{
	ret = ggzdmod_connect ( ggzdmod );

	init_ggz ();

	return INT2FIX ( ret );
}

static VALUE t_disconnect ( VALUE self )
{
	ret = ggzdmod_disconnect ( ggzdmod );

	return INT2FIX ( ret );
}

static VALUE t_get_state ( VALUE self )
{
	retv = INT2FIX ( ggzdmod_get_state ( ggzdmod ) );

	return retv;
}

static VALUE t_loop ( VALUE self )
{
	ret = ggzdmod_loop ( ggzdmod );

	return INT2FIX ( ret );
}

static VALUE t_get_num_seats ( VALUE self )
{
	ret = ggzdmod_get_num_seats ( ggzdmod );

	return INT2FIX ( ret );
}

static VALUE t_get_seat_name ( VALUE self, VALUE seat )
{
	rets = ggzdmod_get_seat ( ggzdmod, FIX2INT ( seat ) ).name;
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
	ret = ggzdmod_get_seat ( ggzdmod, FIX2INT ( seat ) ).type;
	retv = INT2FIX ( ret );

	return retv;
}

static VALUE t_get_seat_fd ( VALUE self, VALUE seat )
{
	ret = ggzdmod_get_seat ( ggzdmod, FIX2INT ( seat ) ).fd;
	retv = INT2FIX ( ret );

	return retv;
}

static VALUE t_log ( VALUE self, VALUE str )
{
	ret = ggzdmod_log ( ggzdmod, STR2CSTR ( str ) );
	if ( ret )
		return Qtrue;
	else
		return Qfalse;

	return retv;
}

/* Module initialization */

static void init_constants ( VALUE self )
{
	rb_define_const ( self, "SEATNONE", INT2NUM ( GGZ_SEAT_NONE ) );
	rb_define_const ( self, "SEATOPEN", INT2NUM ( GGZ_SEAT_OPEN ) );
	rb_define_const ( self, "SEATBOT", INT2NUM ( GGZ_SEAT_BOT ) );
	rb_define_const ( self, "SEATPLAYER", INT2NUM ( GGZ_SEAT_PLAYER ) );
	rb_define_const ( self, "SEATRESERVED", INT2NUM ( GGZ_SEAT_RESERVED ) );
	rb_define_const ( self, "SEATABANDONED", INT2NUM ( GGZ_SEAT_ABANDONED ) );

	rb_define_const ( self, "STATECREATED", INT2FIX ( GGZDMOD_STATE_CREATED ) );
	rb_define_const ( self, "STATEWAITING", INT2FIX ( GGZDMOD_STATE_WAITING ) );
	rb_define_const ( self, "STATEPLAYING", INT2FIX ( GGZDMOD_STATE_PLAYING ) );
	rb_define_const ( self, "STATEDONE", INT2FIX ( GGZDMOD_STATE_DONE ) );

	rb_define_const ( self, "EVENTSTATE", INT2FIX ( GGZDMOD_EVENT_STATE ) );
	rb_define_const ( self, "EVENTJOIN", INT2FIX ( GGZDMOD_EVENT_JOIN ) );
	rb_define_const ( self, "EVENTLEAVE", INT2FIX ( GGZDMOD_EVENT_LEAVE ) );
	rb_define_const ( self, "EVENTSEAT", INT2FIX ( GGZDMOD_EVENT_SEAT ) );
	rb_define_const ( self, "EVENTDATA", INT2FIX ( GGZDMOD_EVENT_PLAYER_DATA ) );
	rb_define_const ( self, "EVENTSPECTATORJOIN", INT2FIX ( GGZDMOD_EVENT_SPECTATOR_JOIN ) );
	rb_define_const ( self, "EVENTSPECTATORLEAVE", INT2FIX ( GGZDMOD_EVENT_SPECTATOR_LEAVE ) );
	rb_define_const ( self, "EVENTSPECTATORSEAT", INT2FIX ( GGZDMOD_EVENT_SPECTATOR_SEAT ) );
	rb_define_const ( self, "EVENTSPECTATORDATA", INT2FIX ( GGZDMOD_EVENT_SPECTATOR_DATA ) );
	rb_define_const ( self, "EVENTERROR", INT2FIX ( GGZDMOD_EVENT_ERROR ) );
}

static void init_methods ( VALUE self )
{
	rb_define_method ( self, "is_ggz_mode", t_is_ggz_mode, 0 );

	rb_define_method ( self, "initialize", t_init, 0 );
	rb_define_method ( self, "connect", t_connect, 0 );
	rb_define_method ( self, "disconnect", t_disconnect, 0 );

	rb_define_method ( self, "get_state", t_get_state, 0 );
	rb_define_method ( self, "get_num_seats", t_get_num_seats, 0 );

	rb_define_method ( self, "get_seat_name", t_get_seat_name, 1 );
	rb_define_method ( self, "get_seat_type", t_get_seat_type, 1 );
	rb_define_method ( self, "get_seat_fd", t_get_seat_fd, 1 );

	rb_define_method ( self, "log", t_log, 1 );

	rb_define_method ( self, "loop", t_loop, 0 );
}

void Init_GGZDMod ()
{
	cTEST = rb_define_class ( "GGZDMod", rb_cObject );

	init_methods ( cTEST );
	init_constants ( cTEST );
}

