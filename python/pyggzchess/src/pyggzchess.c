/**********************************************/
/*                                            */
/* PyGGZChess - Python wrapper for chess game */
/* Copyright (C) 2004 Josef Spillner          */
/* josef@ggzgamingzone.org                    */
/* Published under GNU GPL conditions         */
/*                                            */
/**********************************************/

/**********************************************/
/* Include files                              */
/**********************************************/

#include <Python.h>

/**********************************************/
/* Prototypes                                 */
/**********************************************/

extern void chess_ai_init(int color, int depth);
extern int chess_ai_move(int from, int to, int force);
extern int chess_ai_find(int color, int *from, int *to);
extern void chess_ai_output(void);
extern int chess_ai_rochade(int color, int which);
extern int chess_ai_exchange(int pos, int *figure);

/**********************************************/
/* Definitions                                */
/**********************************************/

#define C_NONE 0
#define C_WHITE 1
#define C_BLACK 2

/**********************************************/
/* Global variables                           */
/**********************************************/

static PyObject *chess = NULL;

/*****************************************************/
/* GGZChess object methods just like in C            */
/*****************************************************/

static PyObject *pyggzchess_init(PyObject *self, PyObject *args)
{
	int color, depth;

	if(!PyArg_ParseTuple(args, "ii", &color, &depth)) return NULL;
	chess_ai_init(color, depth);
	return Py_BuildValue("");
}

static PyObject *pyggzchess_move(PyObject *self, PyObject *args)
{
	int from, to, force;
	int ret;

	if(!PyArg_ParseTuple(args, "iii", &from, &to, &force)) return NULL;
	ret = chess_ai_move(from, to, force);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzchess_find(PyObject *self, PyObject *args)
{
	int color, from, to;
	int ret;

	if(!PyArg_ParseTuple(args, "i", &color)) return NULL;
	ret = chess_ai_find(color, &from, &to);
	return Py_BuildValue("i(ii)", ret, from, to);
}

static PyObject *pyggzchess_output(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	chess_ai_output();
	return Py_BuildValue("");
}

static PyObject *pyggzchess_rochade(PyObject *self, PyObject *args)
{
	int color, which;
	int ret;

	if(!PyArg_ParseTuple(args, "ii", &color, &which)) return NULL;
	ret = chess_ai_rochade(color, which);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzchess_exchange(PyObject *self, PyObject *args)
{
	int pos;
	int ret, figure;

	if(!PyArg_ParseTuple(args, "i", &pos)) return NULL;
	ret = chess_ai_exchange(pos, &figure);
	return Py_BuildValue("ii", ret, figure);
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyggzchess_methods[] =
{
	{"init", pyggzchess_init, METH_VARARGS},
	{"move", pyggzchess_move, METH_VARARGS},
	{"find", pyggzchess_find, METH_VARARGS},
	{"output", pyggzchess_output, METH_VARARGS},
	{"rochade", pyggzchess_rochade, METH_VARARGS},
	{"exchange", pyggzchess_exchange, METH_VARARGS},
	{NULL, NULL, 0}
};

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initggzchess(void)
{
	chess = Py_InitModule("ggzchess", pyggzchess_methods);

	PyModule_AddIntConstant(chess, "NONE", C_NONE);
	PyModule_AddIntConstant(chess, "WHITE", C_WHITE);
	PyModule_AddIntConstant(chess, "BLACK", C_BLACK);
}

