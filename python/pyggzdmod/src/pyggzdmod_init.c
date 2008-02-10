/**********************************************/
/*                                            */
/* PyGGZDMod - Python wrapper for libggzdmod  */
/* Copyright (C) 2001 - 2006 Josef Spillner   */
/* josef@ggzgamingzone.org                    */
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

static void pyggzdmod_cb_state_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_join_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_leave_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_seat_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_data_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_spectatorjoin_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_spectatorleave_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_spectatorseat_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_spectatordata_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);
static void pyggzdmod_cb_error_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data);

/**********************************************/
/* Global variables                           */
/**********************************************/

static GGZdMod *ggzdmod;

static PyObject *pyggzdmod_cb_state = NULL;
static PyObject *pyggzdmod_cb_join = NULL;
static PyObject *pyggzdmod_cb_leave = NULL;
static PyObject *pyggzdmod_cb_seat = NULL;
static PyObject *pyggzdmod_cb_data = NULL;
static PyObject *pyggzdmod_cb_spectatorjoin = NULL;
static PyObject *pyggzdmod_cb_spectatorleave = NULL;
static PyObject *pyggzdmod_cb_spectatorseat = NULL;
static PyObject *pyggzdmod_cb_spectatordata = NULL;
static PyObject *pyggzdmod_cb_error = NULL;

/**********************************************/
/* GGZDMod object methods just like in C      */
/**********************************************/

static PyObject *pyggzdmod_get_num_seats(PyObject *self, PyObject *args)
{
	int seats;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	seats = ggzdmod_get_num_seats(ggzdmod);
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzdmod_get_num_spectators(PyObject *self, PyObject *args)
{
	int spectators;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	spectators = ggzdmod_get_max_num_spectators(ggzdmod);
	return Py_BuildValue("i", spectators);
}

static PyObject *pyggzdmod_count_seats(PyObject *self, PyObject *args)
{
	int seats;
	GGZSeatType type;

	if(!PyArg_ParseTuple(args, "i", &type)) return NULL;
	seats = ggzdmod_count_seats(ggzdmod, type);
	return Py_BuildValue("i", seats);
}

static PyObject *pyggzdmod_count_spectators(PyObject *self, PyObject *args)
{
	int spectators;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	spectators = ggzdmod_count_spectators(ggzdmod);
	return Py_BuildValue("i", spectators);
}

/* FIXME: only handles simple winner/loser scheme */
static PyObject *pyggzdmod_statistics_report(PyObject *self, PyObject *args)
{
	int winner, i;
	GGZGameResult results[ggzdmod_get_num_seats(ggzdmod)];

	if(!PyArg_ParseTuple(args, "i", &winner)) return NULL;
	if((winner < 0) || (winner >= ggzdmod_get_num_seats(ggzdmod))) return NULL;
	for(i = 0; i < ggzdmod_get_num_seats(ggzdmod); i++)
		results[i] = GGZ_GAME_LOSS;
	results[winner] = GGZ_GAME_WIN;
	ggzdmod_report_game(ggzdmod, NULL, results, NULL);

	return Py_None;
}

static PyObject *pyggzdmod_savegame_report(PyObject *self, PyObject *args)
{
	char *savegame;

	if(!PyArg_ParseTuple(args, "s", &savegame)) return NULL;
	ggzdmod_report_savegame(ggzdmod, savegame);

	return Py_None;
}

static PyObject *pyggzdmod_get_seat(PyObject *self, PyObject *args)
{
	GGZSeat seat;
	int seatnum;

	if(!PyArg_ParseTuple(args, "i", &seatnum)) return NULL;
	seat = ggzdmod_get_seat(ggzdmod, seatnum);
	if(seat.num == -1)
	{
		return Py_None;
	}
	return Py_BuildValue("(iisi)", seat.num, seat.type, seat.name, seat.fd);
}

static PyObject *pyggzdmod_get_spectator(PyObject *self, PyObject *args)
{
	GGZSpectator seat;
	int seatnum;

	if(!PyArg_ParseTuple(args, "i", &seatnum)) return NULL;
	seat = ggzdmod_get_spectator(ggzdmod, seatnum);
	if(seat.num == -1)
	{
		return Py_None;
	}
	return Py_BuildValue("(isi)", seat.num, seat.name, seat.fd);
}

static PyObject *pyggzdmod_get_bot_class(PyObject *self, PyObject *args)
{
	char *name;
	char *botclass;

	if(!PyArg_ParseTuple(args, "s", &name)) return NULL;
	botclass = ggzdmod_get_bot_class(ggzdmod, name);
	if(!botclass) return Py_None;
	return Py_BuildValue("s", botclass);
}

static PyObject *pyggzdmod_request_num_seats(PyObject *self, PyObject *args)
{
	int num;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	ggzdmod_request_num_seats(ggzdmod, num);

	return Py_None;
}

static PyObject *pyggzdmod_request_boot(PyObject *self, PyObject *args)
{
	char *name;

	if(!PyArg_ParseTuple(args, "s", &name)) return NULL;
	ggzdmod_request_boot(ggzdmod, name);

	return Py_None;
}

static PyObject *pyggzdmod_request_bot(PyObject *self, PyObject *args)
{
	int num;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	ggzdmod_request_bot(ggzdmod, num);

	return Py_None;
}

static PyObject *pyggzdmod_request_open(PyObject *self, PyObject *args)
{
	int num;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	ggzdmod_request_open(ggzdmod, num);

	return Py_None;
}

static PyObject *pyggzdmod_log(PyObject *self, PyObject *args)
{
	char *message;

	if(!PyArg_ParseTuple(args, "s", &message)) return NULL;
	ggzdmod_log(ggzdmod, "%s", message);

	return Py_None;
}

/**********************************************/
/* GGZDMod control functions                  */
/**********************************************/

static PyObject *pyggzdmod_connect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzdmod_connect(ggzdmod);
	return Py_BuildValue("i", (ret == 0 ? 1 : 0));
}

static PyObject *pyggzdmod_disconnect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzdmod_disconnect(ggzdmod);
	return Py_BuildValue("i", (ret == 0 ? 1 : 0));
}

static PyObject *pyggzdmod_dispatch(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	ggzdmod_dispatch(ggzdmod);
	return Py_None;
}

static PyObject *pyggzdmod_loop(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzdmod_loop(ggzdmod);
	return Py_BuildValue("i", (ret == 0 ? 1 : 0));
}

static PyObject *pyggzdmod_get_state(PyObject *self, PyObject *args)
{
	int state;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	state = ggzdmod_get_state(ggzdmod);
	return Py_BuildValue("i", state);
}

static PyObject *pyggzdmod_set_state(PyObject *self, PyObject *args)
{
	int state;
	int ret;

	if(!PyArg_ParseTuple(args, "i", &state)) return NULL;
	ret = ggzdmod_set_state(ggzdmod, state);
	return Py_BuildValue("i", (ret == 0 ? 1 : 0));
}

/**********************************************/
/* Function to set callback methods in Python */
/**********************************************/

static PyObject *pyggzdmod_set_handler(PyObject *self, PyObject *args)
{
	int id;
	PyObject *result = NULL;
	PyObject *temp;

	if(!PyArg_ParseTuple(args, "iO", &id, &temp)) return NULL;
	Py_XINCREF(temp);
	switch(id)
	{
		case GGZDMOD_EVENT_STATE:
			Py_XDECREF(pyggzdmod_cb_state);
			pyggzdmod_cb_state = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_STATE, pyggzdmod_cb_state_hook);
			break;
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
		case GGZDMOD_EVENT_SEAT:
			Py_XDECREF(pyggzdmod_cb_seat);
			pyggzdmod_cb_seat = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SEAT, pyggzdmod_cb_seat_hook);
			break;
		case GGZDMOD_EVENT_PLAYER_DATA:
			Py_XDECREF(pyggzdmod_cb_data);
			pyggzdmod_cb_data = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, pyggzdmod_cb_data_hook);
			break;
		case GGZDMOD_EVENT_SPECTATOR_JOIN:
			Py_XDECREF(pyggzdmod_cb_spectatorjoin);
			pyggzdmod_cb_spectatorjoin = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN, pyggzdmod_cb_spectatorjoin_hook);
			break;
		case GGZDMOD_EVENT_SPECTATOR_LEAVE:
			Py_XDECREF(pyggzdmod_cb_spectatorleave);
			pyggzdmod_cb_spectatorleave = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE, pyggzdmod_cb_spectatorleave_hook);
			break;
		case GGZDMOD_EVENT_SPECTATOR_SEAT:
			Py_XDECREF(pyggzdmod_cb_spectatorseat);
			pyggzdmod_cb_spectatorseat = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_SEAT, pyggzdmod_cb_spectatorseat_hook);
			break;
		case GGZDMOD_EVENT_SPECTATOR_DATA:
			Py_XDECREF(pyggzdmod_cb_spectatordata);
			pyggzdmod_cb_spectatordata = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_DATA, pyggzdmod_cb_spectatordata_hook);
			break;
		case GGZDMOD_EVENT_ERROR:
			Py_XDECREF(pyggzdmod_cb_error);
			pyggzdmod_cb_error = temp;
			ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_ERROR, pyggzdmod_cb_error_hook);
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
	{"getNumSpectators", pyggzdmod_get_num_spectators, METH_VARARGS},
	{"countSeats", pyggzdmod_count_seats, METH_VARARGS},
	{"countSpectators", pyggzdmod_count_spectators, METH_VARARGS},
	{"getSeat", pyggzdmod_get_seat, METH_VARARGS},
	{"getSpectator", pyggzdmod_get_spectator, METH_VARARGS},
	{"getBotClass", pyggzdmod_get_bot_class, METH_VARARGS},
	{"reportStatistics", pyggzdmod_statistics_report, METH_VARARGS},
	{"reportSavegame", pyggzdmod_savegame_report, METH_VARARGS},
	{"requestNumSeats", pyggzdmod_request_num_seats, METH_VARARGS},
	{"requestBoot", pyggzdmod_request_boot, METH_VARARGS},
	{"requestBot", pyggzdmod_request_bot, METH_VARARGS},
	{"requestOpen", pyggzdmod_request_open, METH_VARARGS},

	{"connect", pyggzdmod_connect, METH_VARARGS},
	{"disconnect", pyggzdmod_disconnect, METH_VARARGS},
	{"dispatch", pyggzdmod_dispatch, METH_VARARGS},
	{"mainLoop", pyggzdmod_loop, METH_VARARGS},
	{"getState", pyggzdmod_get_state, METH_VARARGS},
	{"setState", pyggzdmod_set_state, METH_VARARGS},
	{"log", pyggzdmod_log, METH_VARARGS},

	{"setHandler", pyggzdmod_set_handler, METH_VARARGS},
	{NULL, NULL}
};

/**********************************************/
/* Internal callbacks                         */
/**********************************************/

void pyggzdmod_cb_state_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(GGZdModState*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_state, arg);
	if(res == NULL)
	{
		printf("------------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_STATE)!\n");
		PyErr_Print();
		printf("------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_join_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZSeat seat;

	seat = *(GGZSeat*)handler_data;
	seat = ggzdmod_get_seat(ggzdmod, seat.num);
	arg = Py_BuildValue("(iisi)", seat.num, seat.type, seat.name, seat.fd);
	res = PyEval_CallObject(pyggzdmod_cb_join, arg);
	if(res == NULL)
	{
		printf("-----------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_JOIN)!\n");
		PyErr_Print();
		printf("-----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_leave_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZSeat seat;

	/* FIXME: ggzdmod docs say it's a GGZSeat* and not a GGZSeat */
	seat = *(GGZSeat*)handler_data;
	seat = ggzdmod_get_seat(ggzdmod, seat.num);
	arg = Py_BuildValue("(iisi)", seat.num, seat.type, seat.name, seat.fd);
	res = PyEval_CallObject(pyggzdmod_cb_leave, arg);
	if(res == NULL)
	{
		printf("------------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_LEAVE)!\n");
		PyErr_Print();
		printf("------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_seat_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZSeat *oldseat;

	oldseat = (GGZSeat*)handler_data;
	arg = Py_BuildValue("(iisi)", oldseat->num, oldseat->type, oldseat->name, oldseat->fd);
	res = PyEval_CallObject(pyggzdmod_cb_seat, arg);
	if(res == NULL)
	{
		printf("-----------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_SEAT)!\n");
		PyErr_Print();
		printf("-----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_data_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggzdmod, *(int*)handler_data);
	arg = Py_BuildValue("(iisi)", seat.num, seat.type, seat.name, seat.fd);
	res = PyEval_CallObject(pyggzdmod_cb_data, arg);
	if(res == NULL)
	{
		printf("-----------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_DATA)!\n");
		PyErr_Print();
		printf("-----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_spectatorjoin_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;

	/* FIXME: ggzdmod docs talk about GGZSeat* here */
	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_spectatorjoin, arg);
	if(res == NULL)
	{
		printf("---------------------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_SPECTATOR_JOIN)!\n");
		PyErr_Print();
		printf("---------------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_spectatorleave_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;

	/* FIXME: ggzdmod docs talk about GGZSeat* here */
	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_spectatorleave, arg);
	if(res == NULL)
	{
		printf("----------------------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_SPECTATOR_LEAVE)!\n");
		PyErr_Print();
		printf("----------------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_spectatorseat_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZSeat *oldseat;

	oldseat = (GGZSeat*)handler_data;
	arg = Py_BuildValue("(iisi)", oldseat->num, oldseat->type, oldseat->name, oldseat->fd);
	res = PyEval_CallObject(pyggzdmod_cb_spectatorseat, arg);
	if(res == NULL)
	{
		printf("---------------------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_SPECTATOR_SEAT)!\n");
		PyErr_Print();
		printf("---------------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_spectatordata_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(i)", *(int*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_spectatordata, arg);
	if(res == NULL)
	{
		printf("---------------------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_SPECTATOR_DATA)!\n");
		PyErr_Print();
		printf("---------------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzdmod_cb_error_hook(GGZdMod *ggzdmod, GGZdModEvent event, const void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(s)", (char*)handler_data);
	res = PyEval_CallObject(pyggzdmod_cb_error, arg);
	if(res == NULL)
	{
		printf("------------------------------------------\n");
		printf("ERROR in pyggzdmod callback (EVENT_ERROR)!\n");
		PyErr_Print();
		printf("------------------------------------------\n");
	}
	Py_DECREF(arg);
}

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initggzdmod(void)
{
	PyObject *mod;

	mod = Py_InitModule("ggzdmod", pyggzdmod_methods);

	PyModule_AddIntConstant(mod, "SEAT_NONE", GGZ_SEAT_NONE);
	PyModule_AddIntConstant(mod, "SEAT_OPEN", GGZ_SEAT_OPEN);
	PyModule_AddIntConstant(mod, "SEAT_BOT", GGZ_SEAT_BOT);
	PyModule_AddIntConstant(mod, "SEAT_PLAYER", GGZ_SEAT_PLAYER);
	PyModule_AddIntConstant(mod, "SEAT_RESERVED", GGZ_SEAT_RESERVED);
	PyModule_AddIntConstant(mod, "SEAT_ABANDONED", GGZ_SEAT_ABANDONED);

	PyModule_AddIntConstant(mod, "EVENT_STATE", GGZDMOD_EVENT_STATE);
	PyModule_AddIntConstant(mod, "EVENT_JOIN", GGZDMOD_EVENT_JOIN);
	PyModule_AddIntConstant(mod, "EVENT_LEAVE", GGZDMOD_EVENT_LEAVE);
	PyModule_AddIntConstant(mod, "EVENT_SEAT", GGZDMOD_EVENT_SEAT);
	PyModule_AddIntConstant(mod, "EVENT_DATA", GGZDMOD_EVENT_PLAYER_DATA);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATORJOIN", GGZDMOD_EVENT_SPECTATOR_JOIN);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATORLEAVE", GGZDMOD_EVENT_SPECTATOR_LEAVE);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATORSEAT", GGZDMOD_EVENT_SPECTATOR_SEAT);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATORDATA", GGZDMOD_EVENT_SPECTATOR_DATA);
	PyModule_AddIntConstant(mod, "EVENT_ERROR", GGZDMOD_EVENT_ERROR);

	PyModule_AddIntConstant(mod, "STATE_CREATED", GGZDMOD_STATE_CREATED);
	PyModule_AddIntConstant(mod, "STATE_WAITING", GGZDMOD_STATE_WAITING);
	PyModule_AddIntConstant(mod, "STATE_PLAYING", GGZDMOD_STATE_PLAYING);
	PyModule_AddIntConstant(mod, "STATE_DONE", GGZDMOD_STATE_DONE);

	PyModule_AddIntConstant(mod, "GAME_WIN", GGZ_GAME_WIN);
	PyModule_AddIntConstant(mod, "GAME_LOSS", GGZ_GAME_LOSS);
	PyModule_AddIntConstant(mod, "GAME_TIE", GGZ_GAME_TIE);
	PyModule_AddIntConstant(mod, "GAME_FORFEIT", GGZ_GAME_FORFEIT);
	PyModule_AddIntConstant(mod, "GAME_NONE", GGZ_GAME_NONE);

	ggzdmod = ggzdmod_new(GGZDMOD_GAME);
}

