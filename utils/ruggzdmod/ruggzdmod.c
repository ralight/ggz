/*
 * Ruby bindings for libggzdmodmod
 * Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions
 */

/* Necessary include files */

#include <ggzdmod.h>
#include <ruby.h>

/* Global Variables */

VALUE cTEST;
int ret;
const char *rets;
VALUE retv;
GGZdMod *ggzdmod;

/* Methods for the ggzdmodmod object */

static VALUE seat2ruby ( VALUE self, VALUE seat )
{
	GGZSeat s;

	s = ggzdmod_get_seat ( ggzdmod, FIX2INT ( seat ) );
	retv = Data_Wrap_Struct ( self, NULL, free, &s );

	return retv;
}

static VALUE t_init ( VALUE self, VALUE type )
{
	ggzdmod = ggzdmod_new( FIX2INT ( type ) );

	return self;
}

static VALUE t_connect ( VALUE self )
{
	ret = ggzdmod_connect ( ggzdmod );

	return INT2FIX ( ret );
}

static VALUE t_disconnect ( VALUE self )
{
	ret = ggzdmod_disconnect ( ggzdmod );

	return INT2FIX ( ret );
}

static VALUE t_get_seat ( VALUE self, VALUE seat )
{
	retv = seat2ruby ( self, seat );

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

/* Module initialization */

void init_constants ( VALUE self )
{
	rb_define_const ( self, "seatopen", INT2NUM ( -1 ) );
	rb_define_const ( self, "seatbot", INT2NUM ( -2 ) );
	rb_define_const ( self, "seatresv", INT2NUM ( -3 ) );
	rb_define_const ( self, "seatnone", INT2NUM ( -4 ) );
	rb_define_const ( self, "seatplayer", INT2NUM ( -5 ) );

	rb_define_const ( self, "statecreated", INT2FIX ( 0 ) );
	rb_define_const ( self, "statewaiting", INT2FIX ( 1 ) );
	rb_define_const ( self, "stateplaying", INT2FIX ( 2 ) );
	rb_define_const ( self, "statedone", INT2FIX ( 3 ) );

	rb_define_const ( self, "eventstate", INT2FIX ( 0 ) );
	rb_define_const ( self, "eventjoin", INT2FIX ( 1 ) );
	rb_define_const ( self, "eventleave", INT2FIX ( 2 ) );
	rb_define_const ( self, "eventlog", INT2FIX ( 3 ) );
	rb_define_const ( self, "eventplayer", INT2FIX ( 4 ) );
	rb_define_const ( self, "eventerror", INT2FIX ( 5 ) );

	rb_define_const ( self, "typeggz", INT2FIX ( 0 ) );
	rb_define_const ( self, "typegame", INT2FIX ( 1 ) );
}

void Init_GGZDMod ()
{
	cTEST = rb_define_class ( "GGZDMod", rb_cObject );

	rb_define_method ( cTEST, "initialize", t_init, 0 );
	rb_define_method ( cTEST, "connect", t_connect, 0 );
	rb_define_method ( cTEST, "disconnect", t_disconnect, 0 );
	rb_define_method ( cTEST, "get_seat", t_get_seat, 1 );
	rb_define_method ( cTEST, "loop", t_loop, 0 );
	rb_define_method ( cTEST, "get_num_seats", t_get_num_seats, 0 );

	init_constants ( cTEST );
}

