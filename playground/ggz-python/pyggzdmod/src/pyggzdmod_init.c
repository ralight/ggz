/**********************************************/
/*                                            */
/* PyGGZDMod - Python wrapper for libggzdmod  */
/* Copyright (C) 2001, 2002 Josef Spillner    */
/* dr_maux@users.sourceforge.net              */
/* Published under GNU GPL conditions         */
/*                                            */
/**********************************************/

/**********************************************/
/* Include files                              */
/**********************************************/

#include <Python.h>
#include <ggzdmod.h>

/**********************************************/
/* Function prototypes                        */
/**********************************************/

static void pyggzdmod_cb_join_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_leave_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_data_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_spectatorjoin_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_spectatorleave_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_spectatordata_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_log_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_error_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);
static void pyggzdmod_cb_state_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data);

static PyObject *pyggzdmod_test(PyObject *self, PyObject *args);

/**********************************************/
/* Global variables                           */
/**********************************************/

static GGZdMod *ggzdmod;

static PyObject *pyggzdmod_cb_join = NULL;
static PyObject *pyggzdmod_cb_leave = NULL;
static PyObject *pyggzdmod_cb_data = NULL;
static PyObject *pyggzdmod_cb_spectatorjoin = NULL;
static PyObject *pyggzdmod_cb_spectatorleave = NULL;
static PyObject *pyggzdmod_cb_spectatordata = NULL;
static PyObject *pyggzdmod_cb_log = NULL;
static PyObject *pyggzdmod_cb_error = NULL;
static PyObject *pyggzdmod_cb_state = NULL;

/**********************************************/
/* GGZDMod object methods just like in C       */
/**********************************************/

static PyObject *pyggzdmod_get_num_seats(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzdmod_get_num_seats(ggzdmod);
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzdmod_get_max_spectators(PyObject *self, PyObject *args)
{
	int spectators;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	spectators = ggzdmod_get_max_num_spectators(ggzdmod);
	return Py_BuildValue("i", spectators);
}

static PyObject *pyggzdmod_count_spectators(PyObject *self, PyObject *args)
{
	int spectators;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	spectators = ggzdmod_count_spectators(ggzdmod);
	return Py_BuildValue("i", spectators);
}

/*static PyObject *pyggzdmod_seats_open(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzdmod_seats_open(ggzdmod);
	return Py_BuildValue("i", seats);
}*/

static PyObject *pyggzdmod_connect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzdmod_connect(ggzdmod);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzdmod_disconnect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzdmod_disconnect(ggzdmod);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzdmod_dispatch(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	ggzdmod_dispatch(ggzdmod);
	return Py_BuildValue("");
}

static PyObject *pyggzdmod_loop(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzdmod_loop(ggzdmod);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzdmod_set_handler(PyObject *self, PyObject *args)
{
	int id;
	PyObject *result = NULL;
	PyObject *temp;

	if(!PyArg_ParseTuple(args, "iO", &id, &temp)) return NULL;
	Py_XINCREF(temp);
	switch(id)
	{
		case GGZDMOD_EVENT_JOIN:
			Py_XDECREF(pyggzdmod_cb_join);
			pyggzdmod_cb_join = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN, pyggzdmod_cb_join_hook);
			break;
		case GGZDMOD_EVENT_LEAVE:
			Py_XDECREF(pyggzdmod_cb_leave);
			pyggzdmod_cb_leave = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, pyggzdmod_cb_leave_hook);
			break;
		case GGZDMOD_EVENT_PLAYER_DATA:
			Py_XDECREF(pyggzdmod_cb_data);
			pyggzdmod_cb_data = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, pyggzdmod_cb_data_hook);
			break;
		case GGZDMOD_EVENT_SPECTATOR_JOIN:
			Py_XDECREF(pyggzdmod_cb_spectatorjoin);
			pyggzdmod_cb_spectatorjoin = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN, pyggzdmod_cb_spectatorjoin_hook);
			break;
		case GGZDMOD_EVENT_SPECTATOR_LEAVE:
			Py_XDECREF(pyggzdmod_cb_spectatorleave);
			pyggzdmod_cb_spectatorleave = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, pyggzdmod_cb_spectatorleave_hook);
			break;
		case GGZDMOD_EVENT_SPECTATOR_DATA:
			Py_XDECREF(pyggzdmod_cb_spectatordata);
			pyggzdmod_cb_spectatordata = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, pyggzdmod_cb_spectatordata_hook);
			break;
		case GGZDMOD_EVENT_ERROR:
			Py_XDECREF(pyggzdmod_cb_error);
			pyggzdmod_cb_error = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_ERROR, pyggzdmod_cb_error_hook);
			break;
		case GGZDMOD_EVENT_LOG:
			Py_XDECREF(pyggzdmod_cb_log);
			pyggzdmod_cb_log = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LOG, pyggzdmod_cb_log_hook);
			break;
		case GGZDMOD_EVENT_STATE:
			Py_XDECREF(pyggzdmod_cb_state);
			pyggzdmod_cb_state = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_STATE, pyggzdmod_cb_state_hook);
			break;
	}
	Py_INCREF(Py_None);
	result = Py_None;
	return result;
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyggzdmod_methods[] =
{
	{"getNumSeats", pyggzdmod_get_num_seats, METH_VARARGS},
	{"getMaxSpectators", pyggzdmod_get_max_spectators, METH_VARARGS},
	{"countSpectators", pyggzdmod_count_spectators, METH_VARARGS},
	/*{"seatsOpen", pyggzdmod_seats_open, METH_VARARGS},*/
	{"connect", pyggzdmod_connect, METH_VARARGS},
	{"disconnect", pyggzdmod_disconnect, METH_VARARGS},
	{"dispatch", pyggzdmod_dispatch, METH_VARARGS},
	{"mainLoop", pyggzdmod_loop, METH_VARARGS},
	{"setHandler", pyggzdmod_set_handler, METH_VARARGS},
	{"test", pyggzdmod_test, METH_VARARGS},
	{NULL, NULL}
};

/**********************************************/
/* Internal callbacks                         */
/**********************************************/

void pyggzdmod_cb_join_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_join, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_leave_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_leave, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_data_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_data, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_spectatorjoin_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_spectatorjoin, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_spectatorleave_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_spectatorleave, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_spectatordata_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_spectatordata, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_error_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_error, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_log_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("()");
	res = PyEval_CallObject(pyggzdmod_cb_log, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_state_hook(GGZdMod *ggzdmod, GGZdModEvent event, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("()");
	res = PyEval_CallObject(pyggzdmod_cb_state, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

static PyObject *pyggzdmod_test(PyObject *self, PyObject *args)
{
	int z;

	z = 0;
	pyggzdmod_cb_state_hook(ggzdmod, GGZDMOD_EVENT_STATE, NULL);
	pyggzdmod_cb_join_hook(ggzdmod, GGZDMOD_EVENT_JOIN, (void*)(&z));
	return Py_None;
}

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initggzdmod(void)
{
	PyObject *mod;

	mod = Py_InitModule("ggzdmod", pyggzdmod_methods);

	PyModule_AddIntConstant(mod, "SEAT_OPEN", GGZ_SEAT_OPEN);
	PyModule_AddIntConstant(mod, "SEAT_BOT", GGZ_SEAT_BOT);
	PyModule_AddIntConstant(mod, "SEAT_RESERVED", GGZ_SEAT_RESERVED);
	PyModule_AddIntConstant(mod, "SEAT_NONE", GGZ_SEAT_NONE);
	PyModule_AddIntConstant(mod, "SEAT_PLAYER", GGZ_SEAT_PLAYER);

	PyModule_AddIntConstant(mod, "CHAT_UNKNOWN", GGZ_CHAT_UNKNOWN);
	PyModule_AddIntConstant(mod, "CHAT_NORMAL", GGZ_CHAT_NORMAL);
	PyModule_AddIntConstant(mod, "CHAT_ANNOUNCE", GGZ_CHAT_ANNOUNCE);
	PyModule_AddIntConstant(mod, "CHAT_BEEP", GGZ_CHAT_BEEP);
	PyModule_AddIntConstant(mod, "CHAT_PERSONAL", GGZ_CHAT_PERSONAL);
	PyModule_AddIntConstant(mod, "CHAT_TABLE", GGZ_CHAT_TABLE);

	PyModule_AddIntConstant(mod, "EVENT_JOIN", GGZDMOD_EVENT_JOIN);
	PyModule_AddIntConstant(mod, "EVENT_LEAVE", GGZDMOD_EVENT_LEAVE);
	PyModule_AddIntConstant(mod, "EVENT_DATA", GGZDMOD_EVENT_PLAYER_DATA);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATORJOIN", GGZDMOD_EVENT_SPECTATOR_JOIN);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATORLEAVE", GGZDMOD_EVENT_SPECTATOR_LEAVE);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATORDATA", GGZDMOD_EVENT_SPECTATOR_DATA);
	PyModule_AddIntConstant(mod, "EVENT_LOG", GGZDMOD_EVENT_LOG);
	PyModule_AddIntConstant(mod, "EVENT_ERROR", GGZDMOD_EVENT_ERROR);
	PyModule_AddIntConstant(mod, "EVENT_STATE", GGZDMOD_EVENT_STATE);

	PyModule_AddIntConstant(mod, "STATE_CREATED", GGZDMOD_STATE_CREATED);
	PyModule_AddIntConstant(mod, "STATE_WAITING", GGZDMOD_STATE_WAITING);
	PyModule_AddIntConstant(mod, "STATE_PLAYING", GGZDMOD_STATE_PLAYING);
	PyModule_AddIntConstant(mod, "STATE_DONE", GGZDMOD_STATE_DONE);

	PyModule_AddIntConstant(mod, "TYPE_GGZ", GGZDMOD_GGZ);
	PyModule_AddIntConstant(mod, "TYPE_GAME", GGZDMOD_GAME);

	ggzdmod = ggzdmod_new(GGZDMOD_GAME);
}

