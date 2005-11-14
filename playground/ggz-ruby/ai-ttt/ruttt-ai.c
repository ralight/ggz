/*
 * Ruby bindings for the Tic-Tac-Toe AI
 * Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
 * Published under GNU GPL conditions
 */

#include "ttt-ai.h"
#include <ruby.h>

VALUE cTTTAI;

static VALUE t_ai_findmove(VALUE self, VALUE player, VALUE difficulty, VALUE board)
{
	int c_player, c_difficulty;
	char c_board[9];
	int ret;
	int i;
	VALUE field, retv;
	VALUE *ptr;

	Check_Type(player, T_FIXNUM);
	Check_Type(difficulty, T_FIXNUM);
	Check_Type(board, T_ARRAY);

	c_player = FIX2INT(player);
	c_difficulty = FIX2INT(board);
	
	ptr = RARRAY(board)->ptr;
	if(RARRAY(board)->len != 9)
		return -1;

	for(i = 0; i < 9; i++)
	{
		field = ptr[i];
		if((TYPE(field) != T_FIXNUM) && (TYPE(field) != T_NIL))
			Check_Type(field, T_FIXNUM);

		if(TYPE(field) == T_NIL)
			c_board[i] = -1;
		else
			c_board[i] = FIX2INT(field);
	}

	ret = ai_findmove(c_player, c_difficulty, c_board);

	retv = INT2FIX(ret);
	return retv;
}

void Init_TTTAI()
{
	cTTTAI = rb_define_class("TTTAI", rb_cObject);

	rb_define_method(cTTTAI, "ai_findmove", t_ai_findmove, 3);
}

