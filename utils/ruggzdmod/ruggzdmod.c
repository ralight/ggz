/*
 * Ruby bindings for libggzdmod
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions
 */

/* Necessary include files */

#include <ggz_server.h>
#include <ruby.h>

/* Global Variables */

VALUE cTEST;
int ret;
const char *rets;

/* Methods for the ggzdmod object */

static VALUE t_init ( VALUE self )
{
	return self;
}

static VALUE t_connect ( VALUE self )
{
	ret = ggzd_connect ();

	return INT2FIX ( ret );
}

static VALUE t_disconnect ( VALUE self )
{
	ret = ggzd_disconnect ();

	return INT2FIX ( ret );
}

static VALUE t_gameover ( VALUE self, VALUE status )
{
	ggzd_gameover ( FIX2INT ( status ) );

	return Qnil;
}

static VALUE t_get_gameover ( VALUE self )
{
	ret = ggzd_get_gameover ();

	return INT2FIX ( ret );
}

static VALUE t_get_player_name ( VALUE self, VALUE seat )
{
	rets = ggzd_get_player_name ( FIX2INT ( seat ) );

	return rb_str_new2 ( rets );
}

static VALUE t_get_player_socket ( VALUE self, VALUE seat )
{
	ret = ggzd_get_player_socket ( FIX2INT ( seat ) );

	return INT2FIX ( ret );
}

static VALUE t_get_player_udp_socket ( VALUE self, VALUE seat )
{
	ret = ggzd_get_player_udp_socket ( FIX2INT ( seat ) );

	return INT2FIX ( ret );
}

static VALUE t_get_seat_status ( VALUE self, VALUE seat )
{
	ret = ggzd_get_seat_status ( FIX2INT ( seat ) );

	return INT2FIX ( ret );
}

static VALUE t_main_loop ( VALUE self )
{
	ret = ggzd_main_loop ();

	return INT2FIX ( ret );
}

static VALUE t_seats_bot ( VALUE self )
{
	ret = ggzd_seats_bot ();

	return INT2FIX ( ret );
}

static VALUE t_seats_human ( VALUE self )
{
	ret = ggzd_seats_human ();

	return INT2FIX ( ret );
}

static VALUE t_seats_num ( VALUE self )
{
	ret = ggzd_seats_num ();

	return INT2FIX ( ret );
}

static VALUE t_seats_open ( VALUE self )
{
	ret = ggzd_seats_open ();

	return INT2FIX ( ret );
}

static VALUE t_seats_reserved ( VALUE self )
{
	ret = ggzd_seats_reserved ();

	return INT2FIX ( ret );
}

static VALUE t_set_player_name ( VALUE self, VALUE seat, VALUE name )
{
	ret = ggzd_set_player_name ( FIX2INT (seat), STR2CSTR ( name ) );

	return INT2FIX ( ret );
}

static VALUE t_set_seat_status ( VALUE self, VALUE seat, VALUE status )
{
	ret = ggzd_set_seat_status ( FIX2INT ( seat ), FIX2INT ( status ) );

	return INT2FIX ( ret );
}

/* Module initialization */

void init_constants ( VALUE self )
{
	rb_define_const ( self, "seatopen", INT2FIX ( -1 ) );
	rb_define_const ( self, "seatbot", INT2FIX ( -2 ) );
	rb_define_const ( self, "seatresv", INT2FIX ( -3 ) );
	rb_define_const ( self, "seatnone", INT2FIX ( -4 ) );
	rb_define_const ( self, "seatplayer", INT2FIX ( -5 ) );

	rb_define_const ( self, "eventlaunch", INT2FIX ( 0 ) );
	rb_define_const ( self, "eventjoin", INT2FIX ( 1 ) );
	rb_define_const ( self, "eventleave", INT2FIX ( 2 ) );
	rb_define_const ( self, "eventquit", INT2FIX ( 3 ) );
	rb_define_const ( self, "eventplayer", INT2FIX ( 4 ) );
}

void Init_GGZDMod ()
{
	cTEST = rb_define_class ( "GGZDMod", rb_cObject );

	rb_define_method ( cTEST, "initialize", t_init, 0 );
	rb_define_method ( cTEST, "connect", t_connect, 0 );
	rb_define_method ( cTEST, "disconnect", t_disconnect, 0 );
	rb_define_method ( cTEST, "gameover", t_gameover, 1 );
	rb_define_method ( cTEST, "get_gameover", t_get_gameover, 0 );
	rb_define_method ( cTEST, "get_player_name", t_get_player_name, 1 );
	rb_define_method ( cTEST, "get_player_socket", t_get_player_socket, 1 );
	rb_define_method ( cTEST, "get_seat_status", t_get_seat_status, 1 );
	rb_define_method ( cTEST, "get_player_udp_socket", t_get_player_udp_socket, 1 );
	rb_define_method ( cTEST, "main_loop", t_main_loop, 0 );
	rb_define_method ( cTEST, "seats_bot", t_seats_bot, 0 );
	rb_define_method ( cTEST, "seats_human", t_seats_human, 0 );
	rb_define_method ( cTEST, "seats_num", t_seats_num, 0 );
	rb_define_method ( cTEST, "seats_open", t_seats_open, 0 );
	rb_define_method ( cTEST, "seats_reserved", t_seats_reserved, 0 );
	rb_define_method ( cTEST, "set_player_name", t_set_player_name, 2 );
	rb_define_method ( cTEST, "set_seat_status", t_set_seat_status, 2 );

	init_constants ( cTEST );
}

