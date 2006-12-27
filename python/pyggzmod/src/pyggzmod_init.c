/**********************************************/
/*                                            */
/* PyGGZMod - Python wrapper for libggzmod    */
/* Copyright (C) 2003 - 2006 Josef Spillner   */
/* josef@ggzgamingzone.org                    */
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

static void pyggzmod_cb_state_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_server_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_player_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_seat_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_spectatorseat_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_chat_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_stats_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_info_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);
static void pyggzmod_cb_error_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data);

/**********************************************/
/* Global variables                           */
/**********************************************/

static GGZMod *ggzmod;

static PyObject *pyggzmod_cb_state = NULL;
static PyObject *pyggzmod_cb_server = NULL;
static PyObject *pyggzmod_cb_player = NULL;
static PyObject *pyggzmod_cb_seat = NULL;
static PyObject *pyggzmod_cb_spectatorseat = NULL;
static PyObject *pyggzmod_cb_chat = NULL;
static PyObject *pyggzmod_cb_stats = NULL;
static PyObject *pyggzmod_cb_info = NULL;
static PyObject *pyggzmod_cb_error = NULL;

/************************************************/
/* Information on status of the Python bindings */
/************************************************/

/* Methods missing on purpose */

/*   ggzmod_new - happens automatically */
/*   ggzmod_free - happens automatically */
/*   ggzmod_get_fd - called by autonetwork() */
/*   ggzmod_dispatch - called by autonetwork() */

/*   Enumeration of GGZModType (GGZMOD_GAME, GGZMOD_GGZ) */

/**********************************************/
/* GGZMod object methods just like in C       */
/**********************************************/

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
	int num;
	int isspectator;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	player = ggzmod_get_player(ggzmod, &isspectator, &num);
	if(!player) return Py_None;
	return Py_BuildValue("(sii)", player, isspectator, num);
}

static PyObject *pyggzmod_get_seat(PyObject *self, PyObject *args)
{
	unsigned int num;
	GGZSeat seat;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	seat = ggzmod_get_seat(ggzmod, num);
	if(seat.num == -1) return Py_None;
	return Py_BuildValue("(iis)", seat.num, seat.type, seat.name);
}

static PyObject *pyggzmod_get_spectator_seat(PyObject *self, PyObject *args)
{
	unsigned int num;
	GGZSpectatorSeat spectator;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	spectator = ggzmod_get_spectator_seat(ggzmod, num);
	if(spectator.num == -1) return Py_None;
	return Py_BuildValue("(is)", spectator.num, spectator.name);
}

static PyObject *pyggzmod_request_stand(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	ggzmod_request_stand(ggzmod);
	return Py_None;
}

static PyObject *pyggzmod_request_sit(PyObject *self, PyObject *args)
{
	int num;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	ggzmod_request_sit(ggzmod, num);
	return Py_None;
}

static PyObject *pyggzmod_request_boot(PyObject *self, PyObject *args)
{
	char *name;

	if(!PyArg_ParseTuple(args, "s", &name)) return NULL;
	ggzmod_request_boot(ggzmod, name);
	return Py_None;
}

static PyObject *pyggzmod_request_bot(PyObject *self, PyObject *args)
{
	int num;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	ggzmod_request_bot(ggzmod, num);
	return Py_None;
}

static PyObject *pyggzmod_request_open(PyObject *self, PyObject *args)
{
	int num;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	ggzmod_request_open(ggzmod, num);
	return Py_None;
}

static PyObject *pyggzmod_request_chat(PyObject *self, PyObject *args)
{
	char *message;

	if(!PyArg_ParseTuple(args, "s", &message)) return NULL;
	ggzmod_request_chat(ggzmod, message);
	return Py_None;
}

static PyObject *pyggzmod_get_record(PyObject *self, PyObject *args)
{
	int num;
	int record;
	int wins, losses, ties, forfeits;
	GGZSeat seat;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	seat.num = num;
	record = ggzmod_player_get_record(ggzmod, &seat, &wins, &losses, &ties, &forfeits);
	if(!record) return Py_None;
	return Py_BuildValue("(iiii)", wins, losses, ties, forfeits);
}

static PyObject *pyggzmod_get_rating(PyObject *self, PyObject *args)
{
	int num;
	int res;
	int rating;
	GGZSeat seat;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	seat.num = num;
	res = ggzmod_player_get_rating(ggzmod, &seat, &rating);
	if(!res) return Py_None;
	return Py_BuildValue("(i)", rating);
}

static PyObject *pyggzmod_get_ranking(PyObject *self, PyObject *args)
{
	int num;
	int res;
	int ranking;
	GGZSeat seat;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	seat.num = num;
	res = ggzmod_player_get_ranking(ggzmod, &seat, &ranking);
	if(!res) return Py_None;
	return Py_BuildValue("(i)", ranking);
}

static PyObject *pyggzmod_get_highscore(PyObject *self, PyObject *args)
{
	int num;
	int res;
	int highscore;
	GGZSeat seat;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	seat.num = num;
	res = ggzmod_player_get_highscore(ggzmod, &seat, &highscore);
	if(!res) return Py_None;
	return Py_BuildValue("(i)", highscore);
}

static PyObject *pyggzmod_request_info(PyObject *self, PyObject *args)
{
	int num;
	int res;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	res = ggzmod_player_request_info(ggzmod, num);
	if(!res) return Py_None;
	return Py_BuildValue("(i)", (res == 0 ? 1 : 0));
}

static PyObject *pyggzmod_get_info(PyObject *self, PyObject *args)
{
	int num;
	GGZPlayerInfo *info;

	if(!PyArg_ParseTuple(args, "i", &num)) return NULL;
	info = ggzmod_player_get_info(ggzmod, num);
	if(!info) return Py_None;
	return Py_BuildValue("(isss)", info->num, info->realname, info->photo, info->host);
}

/***********************************************/
/* Methods for controlling and querying GGZMod */
/***********************************************/

static PyObject *pyggzmod_set_state(PyObject *self, PyObject *args)
{
	int state;
	int ret;

	if(!PyArg_ParseTuple(args, "i", &state)) return NULL;
	ret = ggzmod_set_state(ggzmod, state);
	return Py_BuildValue("i", (ret == 0 ? 1 : 0));
}

static PyObject *pyggzmod_get_state(PyObject *self, PyObject *args)
{
	GGZModState state;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	state = ggzmod_get_state(ggzmod);
	return Py_BuildValue("i", state);
}

static PyObject *pyggzmod_connect(PyObject *self, PyObject *args)
{
	int connect;
	char *mode;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	connect = ggzmod_connect(ggzmod);
	if(connect == 0)
	{
		/* Force GGZMODE to be set */
		mode = getenv("GGZMODE");
		if(!mode) connect = -1;
	}
	return Py_BuildValue("i", (connect == 0 ? 1 : 0));
}

static PyObject *pyggzmod_disconnect(PyObject *self, PyObject *args)
{
	int disconnect;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	disconnect = ggzmod_disconnect(ggzmod);
	return Py_BuildValue("i", (disconnect == 0 ? 1 : 0));
}

static PyObject *pyggzmod_get_server_fd(PyObject *self, PyObject *args)
{
	int serverfd;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	serverfd = ggzmod_get_server_fd(ggzmod);
	return Py_BuildValue("i", serverfd);
}

/********************************************/
/* Networking with GGZ and the game server  */
/********************************************/

static PyObject *pyggzmod_autonetwork(PyObject *self, PyObject *args)
{
	int ret, actualret;
	struct timeval tv;
	fd_set set;
	int ggzfd;
	int gamefd;
	int maxfd;

	if(!PyArg_ParseTuple(args, "")) return NULL;

	FD_ZERO(&set);

	ggzfd = ggzmod_get_fd(ggzmod);
	gamefd = ggzmod_get_server_fd(ggzmod);

	FD_SET(ggzfd, &set);
	maxfd = ggzfd;
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
			if(FD_ISSET(gamefd, &set)) actualret = 1;
		}
		if(FD_ISSET(ggzfd, &set)) ggzmod_dispatch(ggzmod);
	}

	return Py_BuildValue("i", actualret);
}

/***************************************/
/* Proxy handler setups for all events */
/***************************************/

static PyObject *pyggzmod_set_handler(PyObject *self, PyObject *args)
{
	int id;
	PyObject *result = NULL;
	PyObject *temp;

	if(!PyArg_ParseTuple(args, "iO", &id, &temp)) return NULL;
	Py_XINCREF(temp);
	switch(id)
	{
		case GGZMOD_EVENT_STATE:
			Py_XDECREF(pyggzmod_cb_state);
			pyggzmod_cb_state = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_STATE, pyggzmod_cb_state_hook);
			break;
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
		case GGZMOD_EVENT_CHAT:
			Py_XDECREF(pyggzmod_cb_chat);
			pyggzmod_cb_chat = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_CHAT, pyggzmod_cb_chat_hook);
			break;
		case GGZMOD_EVENT_STATS:
			Py_XDECREF(pyggzmod_cb_stats);
			pyggzmod_cb_stats = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_STATS, pyggzmod_cb_stats_hook);
			break;
		case GGZMOD_EVENT_INFO:
			Py_XDECREF(pyggzmod_cb_info);
			pyggzmod_cb_info = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_INFO, pyggzmod_cb_info_hook);
			break;
		case GGZMOD_EVENT_ERROR:
			Py_XDECREF(pyggzmod_cb_error);
			pyggzmod_cb_error = temp;
			ggzmod_set_handler(ggzmod, GGZMOD_EVENT_ERROR, pyggzmod_cb_error_hook);
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
	/* Object information and manipulation */
	{"getNumSeats", pyggzmod_get_num_seats, METH_VARARGS},
	{"getNumSpectators", pyggzmod_get_num_spectator_seats, METH_VARARGS},
	{"getSeat", pyggzmod_get_seat, METH_VARARGS},
	{"getSpectator", pyggzmod_get_spectator_seat, METH_VARARGS},
	{"getPlayer", pyggzmod_get_player, METH_VARARGS},

	/* Seat change requests */
	{"requestStand", pyggzmod_request_stand, METH_VARARGS},
	{"requestSit", pyggzmod_request_sit, METH_VARARGS},
	{"requestBoot", pyggzmod_request_boot, METH_VARARGS},
	{"requestBot", pyggzmod_request_bot, METH_VARARGS},
	{"requestOpen", pyggzmod_request_open, METH_VARARGS},

	/* Other requests */
	{"requestChat", pyggzmod_request_chat, METH_VARARGS},
	{"requestInfo", pyggzmod_request_info, METH_VARARGS},

	/* Statistics and information */
	{"getInfo", pyggzmod_get_info, METH_VARARGS},
	{"getRecord", pyggzmod_get_record, METH_VARARGS},
	{"getRating", pyggzmod_get_rating, METH_VARARGS},
	{"getRanking", pyggzmod_get_ranking, METH_VARARGS},
	{"getHighscore", pyggzmod_get_highscore, METH_VARARGS},

	/* Basic game handling */
	{"connect", pyggzmod_connect, METH_VARARGS},
	{"disconnect", pyggzmod_disconnect, METH_VARARGS},
	{"setState", pyggzmod_set_state, METH_VARARGS},
	{"getState", pyggzmod_get_state, METH_VARARGS},
	{"autonetwork", pyggzmod_autonetwork, METH_VARARGS},
	{"getFd", pyggzmod_get_server_fd, METH_VARARGS},

	/* Interfacing with the python game code */
	{"setHandler", pyggzmod_set_handler, METH_VARARGS},

	{NULL, NULL}
};

/**********************************************/
/* Internal callbacks                         */
/**********************************************/

void pyggzmod_cb_state_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZModState oldstate;

	oldstate = *(GGZModState*)handler_data;

	arg = Py_BuildValue("(i)", oldstate);
	res = PyEval_CallObject(pyggzmod_cb_state, arg);
	if(res == NULL)
	{
		printf("-----------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_STATE)!\n");
		PyErr_Print();
		printf("-----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_server_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	int fd;

	fd = *(int*)handler_data;

	arg = Py_BuildValue("(i)", fd);
	res = PyEval_CallObject(pyggzmod_cb_server, arg);
	if(res == NULL)
	{
		printf("------------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_SERVER)!\n");
		PyErr_Print();
		printf("------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_player_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	struct playerdata
	{
		int is_spectator;
		int seat_num;
	};
	struct playerdata data;

	data = *(struct playerdata*)handler_data;

	arg = Py_BuildValue("(ii)", data.is_spectator, data.seat_num);
	res = PyEval_CallObject(pyggzmod_cb_player, arg);
	if(res == NULL)
	{
		printf("------------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_PLAYER)!\n");
		PyErr_Print();
		printf("------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_seat_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZSeat oldseat;

	oldseat = *(GGZSeat*)handler_data;

	arg = Py_BuildValue("(i)", oldseat.num);
	res = PyEval_CallObject(pyggzmod_cb_seat, arg);
	if(res == NULL)
	{
		printf("----------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_SEAT)!\n");
		PyErr_Print();
		printf("----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_spectatorseat_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZSpectatorSeat oldspectator;

	oldspectator = *(GGZSpectatorSeat*)handler_data;

	arg = Py_BuildValue("(i)", oldspectator.num);
	res = PyEval_CallObject(pyggzmod_cb_spectatorseat, arg);
	if(res == NULL)
	{
		printf("--------------------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_SPECTATOR_SEAT)!\n");
		PyErr_Print();
		printf("--------------------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_chat_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	GGZChat chat;

	chat = *(GGZChat*)handler_data;

	arg = Py_BuildValue("(ss)", chat.player, chat.message);
	res = PyEval_CallObject(pyggzmod_cb_chat, arg);
	if(res == NULL)
	{
		printf("----------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_CHAT)!\n");
		PyErr_Print();
		printf("----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_stats_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("()");
	res = PyEval_CallObject(pyggzmod_cb_stats, arg);
	if(res == NULL)
	{
		printf("-----------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_STATS)!\n");
		PyErr_Print();
		printf("-----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_info_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	/*GGZPlayerInfo info;*/

	/*
	if(handler_data)
	{
		info = *(GGZPlayerInfo*)handler_data;
		arg = Py_BuildValue("(isss)", info.num, info.realname, info.photo, info.host);
	}
	else
	{
		arg = Py_BuildValue("(isss)", -1, NULL, NULL, NULL);
	}
	*/
	arg = Py_BuildValue("()");
	res = PyEval_CallObject(pyggzmod_cb_info, arg);
	if(res == NULL)
	{
		printf("----------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_INFO)!\n");
		PyErr_Print();
		printf("----------------------------------------\n");
	}
	Py_DECREF(arg);
}

void pyggzmod_cb_error_hook(GGZMod *ggzmod, GGZModEvent event, const void *handler_data)
{
	PyObject *arg, *res;
	char *error;

	error = (char*)handler_data;

	arg = Py_BuildValue("(s)", error);
	res = PyEval_CallObject(pyggzmod_cb_error, arg);
	if(res == NULL)
	{
		printf("-----------------------------------------\n");
		printf("ERROR in pyggzmod callback (EVENT_ERROR)!\n");
		PyErr_Print();
		printf("-----------------------------------------\n");
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

	PyModule_AddIntConstant(mod, "SEAT_NONE", GGZ_SEAT_NONE);
	PyModule_AddIntConstant(mod, "SEAT_OPEN", GGZ_SEAT_OPEN);
	PyModule_AddIntConstant(mod, "SEAT_BOT", GGZ_SEAT_BOT);
	PyModule_AddIntConstant(mod, "SEAT_PLAYER", GGZ_SEAT_PLAYER);
	PyModule_AddIntConstant(mod, "SEAT_RESERVED", GGZ_SEAT_RESERVED);
	PyModule_AddIntConstant(mod, "SEAT_ABANDONED", GGZ_SEAT_ABANDONED);

	PyModule_AddIntConstant(mod, "STATE_CREATED", GGZMOD_STATE_CREATED);
	PyModule_AddIntConstant(mod, "STATE_CONNECTED", GGZMOD_STATE_CONNECTED);
	PyModule_AddIntConstant(mod, "STATE_WAITING", GGZMOD_STATE_WAITING);
	PyModule_AddIntConstant(mod, "STATE_PLAYING", GGZMOD_STATE_PLAYING);
	PyModule_AddIntConstant(mod, "STATE_DONE", GGZMOD_STATE_DONE);

	PyModule_AddIntConstant(mod, "EVENT_STATE", GGZMOD_EVENT_STATE);
	PyModule_AddIntConstant(mod, "EVENT_SERVER", GGZMOD_EVENT_SERVER);
	PyModule_AddIntConstant(mod, "EVENT_PLAYER", GGZMOD_EVENT_PLAYER);
	PyModule_AddIntConstant(mod, "EVENT_SEAT", GGZMOD_EVENT_SEAT);
	PyModule_AddIntConstant(mod, "EVENT_SPECTATOR", GGZMOD_EVENT_SPECTATOR_SEAT);
	PyModule_AddIntConstant(mod, "EVENT_CHAT", GGZMOD_EVENT_CHAT);
	PyModule_AddIntConstant(mod, "EVENT_STATS", GGZMOD_EVENT_STATS);
	PyModule_AddIntConstant(mod, "EVENT_INFO", GGZMOD_EVENT_INFO);
	PyModule_AddIntConstant(mod, "EVENT_ERROR", GGZMOD_EVENT_ERROR);

	ggzmod = ggzmod_new(GGZMOD_GAME);
}

