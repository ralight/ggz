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

static PyObject *pyggzmod_get_type(PyObject *self, PyObject *args)
{
	GGZModType type;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	type = ggzmod_get_type(ggzmod);
	return Py_BuildValue("i", type);
}

static PyObject *pyggzmod_get_state(PyObject *self, PyObject *args)
{
	GGZModState state;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	state = ggzmod_get_state(ggzmod);
	return Py_BuildValue("i", state);
}

/*static PyObject *pyggzmod_get_server_fd(PyObject *self, PyObject *args)
{
	int serverfd;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	serverfd = ggzmod_get_server_fd(ggzmod);
	return Py_BuildValue("i", serverfd);
}*/

static PyObject *pyggzmod_get_num_seats(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzmod_get_num_seats(ggzmod);
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzmod_get_num_spectator_seats(PyObject *self, PyObject *args)
{
	int spectatorseats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	spectatorseats = ggzmod_get_num_spectator_seats(ggzmod);
	return Py_BuildValue("i", spectatorseats);
}

static PyObject *pyggzmod_get_player(PyObject *self, PyObject *args)
{
	const char *player;
	int *seat = NULL;
	int *spectator = NULL;

	if(!PyArg_ParseTuple(args, "OO", spectator, seat)) return NULL;
	player = ggzmod_get_player(ggzmod, spectator, seat);
	return Py_BuildValue("s", player);
}

static PyObject *pyggzmod_dispatch(PyObject *self, PyObject *args)
{
	int dispatch;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	dispatch = ggzmod_dispatch(ggzmod);
	return Py_BuildValue("i", dispatch);
}

static PyObject *pyggzmod_set_state(PyObject *self, PyObject *args)
{
	int state;
	int ret;

	if(!PyArg_ParseTuple(args, "i", &state)) return NULL;
	ret = ggzmod_set_state(ggzmod, state);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzmod_connect(PyObject *self, PyObject *args)
{
	int connect;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	connect = ggzmod_connect(ggzmod);
	return Py_BuildValue("i", connect);
}

static PyObject *pyggzmod_disconnect(PyObject *self, PyObject *args)
{
	int disconnect;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	disconnect = ggzmod_disconnect(ggzmod);
	return Py_BuildValue("i", disconnect);
}

static PyObject *pyggzmod_get_seat_num(PyObject *self, PyObject *args)
{
	unsigned int num;
	int seat;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	num = ggzmod_get_seat(ggzmod, seat).num;
	return Py_BuildValue("i", num);
}

static PyObject *pyggzmod_get_seat_type(PyObject *self, PyObject *args)
{
	GGZSeatType type;
	int seat;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	type = ggzmod_get_seat(ggzmod, seat).type;
	return Py_BuildValue("i", type);
}

static PyObject *pyggzmod_get_seat_name(PyObject *self, PyObject *args)
{
	const char *name;
	int seat;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	name = ggzmod_get_seat(ggzmod, seat).name;
	return Py_BuildValue("s", name);
}

static PyObject *pyggzmod_get_spectator_seat_num(PyObject *self, PyObject *args)
{
	unsigned int num;
	int seat;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	num = ggzmod_get_spectator_seat(ggzmod, seat).num;
	return Py_BuildValue("i", num);
}

static PyObject *pyggzmod_get_spectator_seat_name(PyObject *self, PyObject *args)
{
	const char *name;
	int seat;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	name = ggzmod_get_spectator_seat(ggzmod, seat).name;
	return Py_BuildValue("s", name);
}

static PyObject *pyggzmod_autonetwork(PyObject *self, PyObject *args)
{
	int networked;
	int ret, actualret;

	int maxfd;
	fd_set set;
	struct timeval tv;
	int serverfd;
	int gamefd;

	if(!PyArg_ParseTuple(args, "ii", &networked, &gamefd)) return NULL;

	FD_ZERO(&set);

	serverfd = ggzmod_get_fd(ggzmod);

	FD_SET(serverfd, &set);
	maxfd = serverfd;
	if(gamefd >= 0)
	{
		FD_SET(gamefd, &set);
		if(gamefd > maxfd) maxfd = gamefd;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	ret = select(maxfd + 1, &set, NULL, NULL, &tv);

	actualret = 0;
	if(ret)
	{
		if(gamefd >= 0)
		{
			if(FD_ISSET(gamefd, &set)) /*game_handle_io()*/actualret |= 1;
		}
		if(FD_ISSET(serverfd, &set)) /*handle_ggz()*/actualret |= 2;
	}

	return Py_BuildValue("i", actualret);
}

/* (ggzmod_get_seat) */
/* (ggzmod_get_spectator_seat) */
/* (ggzmod_get_player) */
/* ggzmod_request_stand */
/* ggzmod_request_sit */
/* ggzmod_request_boot */
/* ggzmod_request_bot */
/* ggzmod_request_open */

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
	{"getType", pyggzmod_get_type, METH_VARARGS},
	{"getState", pyggzmod_get_state, METH_VARARGS},
	/*{"getServerFd", pyggzmod_get_server_fd, METH_VARARGS},*/
	{"getNumSeats", pyggzmod_get_num_seats, METH_VARARGS},
	{"getNumSpectatorSeats", pyggzmod_get_num_spectator_seats, METH_VARARGS},
	{"getSeatNum", pyggzmod_get_seat_num, METH_VARARGS},
	{"getSeatType", pyggzmod_get_seat_type, METH_VARARGS},
	{"getSeatName", pyggzmod_get_seat_name, METH_VARARGS},
	{"getSpectatorSeatNum", pyggzmod_get_spectator_seat_num, METH_VARARGS},
	{"getSpectatorSeatName", pyggzmod_get_spectator_seat_name, METH_VARARGS},
	{"getPlayer", pyggzmod_get_player, METH_VARARGS},
	{"dispatch", pyggzmod_dispatch, METH_VARARGS},
	{"setState", pyggzmod_set_state, METH_VARARGS},
	{"connect", pyggzmod_connect, METH_VARARGS},
	{"disconnect", pyggzmod_disconnect, METH_VARARGS},
	{"setHandler", pyggzmod_set_handler, METH_VARARGS},
	{"autonetwork", pyggzmod_autonetwork, METH_VARARGS},
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

