/**********************************************/
/*                                            */
/* PyGGZDMod - Python wrapper for libggzdmod  */
/* Copyright (C) 2001 Josef Spillner          */
/* dr_maux@users.sourceforge.net              */
/* Published under GNU GPL conditions         */
/*                                            */
/**********************************************/

/**********************************************/
/* Include files                              */
/**********************************************/

#include <Python.h>
#include <ggz_server.h>

/**********************************************/
/* Function prototypes                        */
/**********************************************/

void pyggzd_cb_launch_hook(ggzd_event_t event_id, void *handler_data);
void pyggzd_cb_join_hook(ggzd_event_t event_id, void *handler_data);
void pyggzd_cb_leave_hook(ggzd_event_t event_id, void *handler_data);
void pyggzd_cb_quit_hook(ggzd_event_t event_id, void *handler_data);
void pyggzd_cb_player_hook(ggzd_event_t event_id, void *handler_data);
static PyObject *pyggzd_test(PyObject *self, PyObject *args);

/**********************************************/
/* Global variables                           */
/**********************************************/

static PyObject *pyggzd_cb_launch = NULL;
static PyObject *pyggzd_cb_join = NULL;
static PyObject *pyggzd_cb_leave = NULL;
static PyObject *pyggzd_cb_quit = NULL;
static PyObject *pyggzd_cb_player = NULL;

/**********************************************/
/* GGZDMod object methods just like in C       */
/**********************************************/

static PyObject *pyggzd_get_seat_status(PyObject *self, PyObject *args)
{
	int seat;
	int ret;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	ret = ggzd_get_seat_status(seat);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_set_seat_status(PyObject *self, PyObject *args)
{
	int seat, status;
	int ret;

	if(!PyArg_ParseTuple(args, "ii", &seat, &status)) return NULL;
	ret = ggzd_set_seat_status(seat, status);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_get_player_name(PyObject *self, PyObject *args)
{
	const char *name;
	int seat;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	name = ggzd_get_player_name(seat);
	return Py_BuildValue("s", name);
}

static PyObject *pyggzd_set_player_name(PyObject *self, PyObject *args)
{
	char *name;
	int seat, ret;

	if(!PyArg_ParseTuple(args, "is", &seat, &name)) return NULL;
	ret = ggzd_set_player_name(seat, name);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_get_player_socket(PyObject *self, PyObject *args)
{
	int seat, socket;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	socket = ggzd_get_player_socket(seat);
	return Py_BuildValue("i", socket);
}

static PyObject *pyggzd_get_player_udp_socket(PyObject *self, PyObject *args)
{
	int seat, socket;

	if(!PyArg_ParseTuple(args, "i", &seat)) return NULL;
	socket = ggzd_get_player_udp_socket(seat);
	return Py_BuildValue("i", socket);
}

static PyObject *pyggzd_seats_num(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzd_seats_num();
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzd_seats_open(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzd_seats_open();
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzd_seats_bot(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzd_seats_bot();
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzd_seats_reserved(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzd_seats_reserved();
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzd_seats_human(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzd_seats_human();
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzd_gameover(PyObject *self, PyObject *args)
{
	int status;

	if(!PyArg_ParseTuple(args, "i", &status)) return NULL;
	ggzd_gameover(status);
	return Py_BuildValue("");
}

static PyObject *pyggzd_get_gameover(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzd_get_gameover();
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_connect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzd_connect();
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_disconnect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzd_disconnect();
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_dispatch(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	ggzd_dispatch();
	return Py_BuildValue("");
}

static PyObject *pyggzd_io_is_pending(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzd_io_is_pending();
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_io_read_all(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	ggzd_io_read_all();
	return Py_BuildValue("");
}

static PyObject *pyggzd_main_loop(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzd_main_loop();
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzd_set_handler(PyObject *self, PyObject *args)
{
	int id;
	PyObject *result = NULL;
	PyObject *temp;

	if(!PyArg_ParseTuple(args, "iO", &id, &temp)) return NULL;
	Py_XINCREF(temp);
	switch(id)
	{
		case GGZ_EVENT_LAUNCH:
			Py_XDECREF(pyggzd_cb_launch);
			pyggzd_cb_launch = temp;
			ggzd_set_handler(GGZ_EVENT_LAUNCH, pyggzd_cb_launch_hook);
			break;
		case GGZ_EVENT_JOIN:
			Py_XDECREF(pyggzd_cb_join);
			pyggzd_cb_join = temp;
			ggzd_set_handler(GGZ_EVENT_JOIN, pyggzd_cb_join_hook);
			break;
		case GGZ_EVENT_LEAVE:
			Py_XDECREF(pyggzd_cb_leave);
			pyggzd_cb_leave = temp;
			ggzd_set_handler(GGZ_EVENT_LEAVE, pyggzd_cb_leave_hook);
			break;
		case GGZ_EVENT_QUIT:
			Py_XDECREF(pyggzd_cb_quit);
			pyggzd_cb_quit = temp;
			ggzd_set_handler(GGZ_EVENT_QUIT, pyggzd_cb_quit_hook);
			break;
		case GGZ_EVENT_PLAYER:
			Py_XDECREF(pyggzd_cb_player);
			pyggzd_cb_player = temp;
			ggzd_set_handler(GGZ_EVENT_PLAYER, pyggzd_cb_player_hook);
			break;
	}
	Py_INCREF(Py_None);
	result = Py_None;
	return result;
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyggzd_methods[] =
{
	{"getSeatStatus", pyggzd_get_seat_status, METH_VARARGS},
	{"setSeatStatus", pyggzd_set_seat_status, METH_VARARGS},
	{"getPlayerName", pyggzd_get_player_name, METH_VARARGS},
	{"setPlayerName", pyggzd_set_player_name, METH_VARARGS},
	{"getPlayerSocket", pyggzd_get_player_socket, METH_VARARGS},
	{"getPlayerUDPSocket", pyggzd_get_player_udp_socket, METH_VARARGS},
	{"seatsNum", pyggzd_seats_num, METH_VARARGS},
	{"seatsOpen", pyggzd_seats_open, METH_VARARGS},
	{"seatsBot", pyggzd_seats_bot, METH_VARARGS},
	{"seatsReserved", pyggzd_seats_reserved, METH_VARARGS},
	{"seatsHuman", pyggzd_seats_human, METH_VARARGS},
	{"gameOver", pyggzd_gameover, METH_VARARGS},
	{"getGameOver", pyggzd_get_gameover, METH_VARARGS},
	{"connect", pyggzd_connect, METH_VARARGS},
	{"disconnect", pyggzd_disconnect, METH_VARARGS},
	{"dispatch", pyggzd_dispatch, METH_VARARGS},
	{"ioIsPending", pyggzd_io_is_pending, METH_VARARGS},
	{"ioReadAll", pyggzd_io_read_all, METH_VARARGS},
	{"mainLoop", pyggzd_main_loop, METH_VARARGS},
	{"setHandler", pyggzd_set_handler, METH_VARARGS},
	{"test", pyggzd_test, METH_VARARGS},
	{NULL, NULL}
};

/**********************************************/
/* Internal callbacks                         */
/**********************************************/

void pyggzd_cb_launch_hook(ggzd_event_t event_id, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("()");
	res = PyEval_CallObject(pyggzd_cb_launch, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzd_cb_join_hook(ggzd_event_t event_id, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzd_cb_join, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzd_cb_leave_hook(ggzd_event_t event_id, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzd_cb_leave, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzd_cb_quit_hook(ggzd_event_t event_id, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("()");
	res = PyEval_CallObject(pyggzd_cb_quit, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzd_cb_player_hook(ggzd_event_t event_id, void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzd_cb_player, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzdmod callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);
}

static PyObject *pyggzd_test(PyObject *self, PyObject *args)
{
	int z;

	z = 0;
	pyggzd_cb_launch_hook(GGZ_EVENT_LAUNCH, NULL);
	pyggzd_cb_join_hook(GGZ_EVENT_JOIN, (void*)(&z));
	return Py_None;
}

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initggzdmod(void)
{
	PyObject *mod;

	mod = Py_InitModule("ggzdmod", pyggzd_methods);

	PyModule_AddIntConstant(mod, "SEAT_OPEN", GGZ_SEAT_OPEN);
	PyModule_AddIntConstant(mod, "SEAT_BOT", GGZ_SEAT_BOT);
	PyModule_AddIntConstant(mod, "SEAT_RESV", GGZ_SEAT_RESV);
	PyModule_AddIntConstant(mod, "SEAT_NONE", GGZ_SEAT_NONE);
	PyModule_AddIntConstant(mod, "SEAT_PLAYER", GGZ_SEAT_PLAYER);

	PyModule_AddIntConstant(mod, "EVENT_LAUNCH", GGZ_EVENT_LAUNCH);
	PyModule_AddIntConstant(mod, "EVENT_JOIN", GGZ_EVENT_JOIN);
	PyModule_AddIntConstant(mod, "EVENT_LEAVE", GGZ_EVENT_LEAVE);
	PyModule_AddIntConstant(mod, "EVENT_QUIT", GGZ_EVENT_QUIT);
	PyModule_AddIntConstant(mod, "EVENT_PLAYER", GGZ_EVENT_PLAYER);
}

