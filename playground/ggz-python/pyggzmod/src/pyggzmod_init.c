/**********************************************/
/*                                            */
/* PyGGZMod - Python wrapper for libggzmod    */
/* Copyright (C) 2003 Josef Spillner          */
/* dr_maux@users.sourceforge.net              */
/* Published under GNU GPL conditions         */
/*                                            */
/**********************************************/

/**********************************************/
/* Include files                              */
/**********************************************/

#include <Python.h>
#include <ggzmod.h>

/**********************************************/
/* Function prototypes                        */
/**********************************************/

static void pyggzmod_cb_server_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data);
static void pyggzmod_cb_player_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data);
static void pyggzmod_cb_seat_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data);
static void pyggzmod_cb_spectatorseat_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data);
static void pyggzmod_cb_error_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data);
static void pyggzmod_cb_state_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data);

/**********************************************/
/* Global variables                           */
/**********************************************/

static GGZMod *ggzmod;

static PyObject *pyggzmod_cb_server = NULL;
static PyObject *pyggzmod_cb_player = NULL;
static PyObject *pyggzmod_cb_seat = NULL;
static PyObject *pyggzmod_cb_spectatorseat = NULL;
static PyObject *pyggzmod_cb_error = NULL;
static PyObject *pyggzmod_cb_state = NULL;

/**********************************************/
/* GGZDMod object methods just like in C       */
/**********************************************/

static PyObject *pyggzmod_get_fd(PyObject *self, PyObject *args)
{
	int fd;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	fd = ggzmod_get_fd(ggzmod);
	return Py_BuildValue("i", fd);
}

static PyObject *pyggzmod_set_handler(PyObject *self, PyObject *args)
{
	int id;
	PyObject *result = NULL;
	PyObject *temp;

	if(!PyArg_ParseTuple(args, "iO", &id, &temp)) return NULL;
	Py_XINCREF(temp);
	switch(id)
	{
		case GGZMOD_EVENT_SERVER:
			Py_XDECREF(pyggzmod_cb_server);
			pyggzmod_cb_server = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_SERVER, pyggzmod_cb_server_hook);
			break;
		case GGZMOD_EVENT_PLAYER:
			Py_XDECREF(pyggzmod_cb_player);
			pyggzmod_cb_player = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_PLAYER, pyggzmod_cb_player_hook);
			break;
		case GGZMOD_EVENT_SEAT:
			Py_XDECREF(pyggzmod_cb_seat);
			pyggzmod_cb_seat = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_SEAT, pyggzmod_cb_seat_hook);
			break;
		case GGZMOD_EVENT_SPECTATOR_SEAT:
			Py_XDECREF(pyggzmod_cb_spectatorseat);
			pyggzmod_cb_spectatorseat = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_SPECTATOR_SEAT, pyggzmod_cb_spectatorseat_hook);
			break;
		case GGZMOD_EVENT_ERROR:
			Py_XDECREF(pyggzmod_cb_error);
			pyggzmod_cb_error = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_ERROR, pyggzmod_cb_error_hook);
			break;
		case GGZMOD_EVENT_STATE:
			Py_XDECREF(pyggzmod_cb_state);
			pyggzmod_cb_state = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_STATE, pyggzmod_cb_state_hook);
			break;
	}
	Py_INCREF(Py_None);
	result = Py_None;
	return result;
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyggzmod_methods[] =
{
	{"getFd", pyggzmod_get_fd, METH_VARARGS},
	{"setHandler", pyggzmod_set_handler, METH_VARARGS},
	{NULL, NULL}
};

/**********************************************/
/* Internal callbacks                         */
/**********************************************/

void pyggzmod_cb_server_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzmod_cb_server, arg);
	if(res == NULL)
	{
		printf("---------------------------\n");
		printf("ERROR in pyggzmod callback!\n");
		PyErr_Print();
		printf("---------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_player_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzmod_cb_player, arg);
	if(res == NULL)
	{
		printf("---------------------------\n");
		printf("ERROR in pyggzmod callback!\n");
		PyErr_Print();
		printf("---------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_seat_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzmod_cb_seat, arg);
	if(res == NULL)
	{
		printf("---------------------------\n");
		printf("ERROR in pyggzmod callback!\n");
		PyErr_Print();
		printf("---------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_spectatorseat_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzmod_cb_spectatorseat, arg);
	if(res == NULL)
	{
		printf("---------------------------\n");
		printf("ERROR in pyggzmod callback!\n");
		PyErr_Print();
		printf("---------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_error_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzmod_cb_error, arg);
	if(res == NULL)
	{
		printf("---------------------------\n");
		printf("ERROR in pyggzmod callback!\n");
		PyErr_Print();
		printf("---------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_state_hook(GGZMod *ggzmod, GGZModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("()");
	res = PyEval_CallObject(pyggzmod_cb_state, arg);
	if(res == NULL)
	{
		printf("---------------------------\n");
		printf("ERROR in pyggzmod callback!\n");
		PyErr_Print();
		printf("---------------------------\n");
	}
	Py_DECREF(arg);
}

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initggzmod(void)
{
	PyObject *mod;

	mod = Py_InitModule("ggzmod", pyggzmod_methods);

	PyModule_AddIntConstant(mod, "GGZ", GGZMOD_GGZ);
	PyModule_AddIntConstant(mod, "GAME", GGZMOD_GAME);

	PyModule_AddIntConstant(mod, "STATE_CREATED", GGZMOD_STATE_CREATED);
	PyModule_AddIntConstant(mod, "STATE_WAITING", GGZMOD_STATE_WAITING);
	PyModule_AddIntConstant(mod, "STATE_PLAYING", GGZMOD_STATE_PLAYING);
	PyModule_AddIntConstant(mod, "STATE_DONE", GGZMOD_STATE_DONE);

	PyModule_AddIntConstant(mod, "EVENT_STATE", GGZMOD_EVENT_STATE);
	PyModule_AddIntConstant(mod, "EVENT_SERVER", GGZMOD_EVENT_SERVER);
	PyModule_AddIntConstant(mod, "EVENT_PLAYER", GGZMOD_EVENT_PLAYER);
	PyModule_AddIntConstant(mod, "EVENT_SEAT", GGZMOD_EVENT_SEAT);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATOR_SEAT", GGZMOD_EVENT_SPECTATOR_SEAT);
	PyModule_AddIntConstant(mod, "EVENT_ERROR", GGZMOD_EVENT_ERROR);

	ggzmod = ggzmod_new(GGZMOD_GAME);
}

