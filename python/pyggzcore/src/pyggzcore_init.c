/**********************************************/
/*                                            */
/* PyGGZCore - Python wrapper for libggzcore  */
/* Copyright (C) 2004 Josef Spillner          */
/* josef@ggzgamingzone.org                    */
/* Published under GNU GPL conditions         */
/*                                            */
/**********************************************/

/**********************************************/
/* Include files                              */
/**********************************************/

#include <Python.h>
#include <ggzcore.h>

/**********************************************/
/* Definitions                                */
/**********************************************/

#define GGZCORE_EVENT_SERVER 1
#define GGZCORE_EVENT_ROOM 2
#define GGZCORE_EVENT_GAME 3

/**********************************************/
/* Function prototypes                        */
/**********************************************/

static GGZHookReturn pyggzcore_cb_server_hook(unsigned int id, void *event_data, void *user_data);
static GGZHookReturn pyggzcore_cb_room_hook(unsigned int id, void *event_data, void *user_data);
static GGZHookReturn pyggzcore_cb_game_hook(unsigned int id, void *event_data, void *user_data);

/**********************************************/
/* Global variables                           */
/**********************************************/

static GGZServer *ggzserver;
static GGZRoom *ggzroom;
static GGZGame *ggzgame;

static PyObject *pyggzcore_cb_server = NULL;
static PyObject *pyggzcore_cb_room = NULL;
static PyObject *pyggzcore_cb_game = NULL;

/**********************************************/
/* GGZDMod object methods just like in C       */
/**********************************************/

static PyObject *pyggzcore_reload(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	ggzcore_reload();
	return Py_BuildValue("");
}

static PyObject *pyggzcore_set_handler(PyObject *self, PyObject *args)
{
	int id;
	PyObject *result = NULL;
	PyObject *temp;

	if(!PyArg_ParseTuple(args, "iO", &id, &temp)) return NULL;
	Py_XINCREF(temp);
	switch(id)
	{
		case GGZCORE_EVENT_SERVER:
			Py_XDECREF(pyggzcore_cb_server);
			pyggzcore_cb_server = temp;
			ggzcore_server_add_event_hook(ggzserver, GGZ_CONNECTED, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_CONNECT_FAIL, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_NEGOTIATED, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_NEGOTIATE_FAIL, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_LOGGED_IN, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_LOGIN_FAIL, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_MOTD_LOADED, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_ROOM_LIST, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_TYPE_LIST, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_ENTERED, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_ENTER_FAIL, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_LOGOUT, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_NET_ERROR, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_PROTOCOL_ERROR, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_CHAT_FAIL, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_STATE_CHANGE, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_CHANNEL_CONNECTED, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_CHANNEL_READY, pyggzcore_cb_server_hook);
			ggzcore_server_add_event_hook(ggzserver, GGZ_CHANNEL_FAIL, pyggzcore_cb_server_hook);
			break;
		case GGZCORE_EVENT_ROOM:
			Py_XDECREF(pyggzcore_cb_room);
			pyggzcore_cb_room = temp;
			ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_LIST, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LIST, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_CHAT_EVENT, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_ROOM_ENTER, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_ROOM_LEAVE, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_UPDATE, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LAUNCHED, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LAUNCH_FAIL, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_JOINED, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_JOIN_FAIL, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LEFT, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LEAVE_FAIL, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_LAG, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_STATS, pyggzcore_cb_room_hook);
			ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_COUNT, pyggzcore_cb_room_hook);
			break;
		case GGZCORE_EVENT_GAME:
			Py_XDECREF(pyggzcore_cb_game);
			pyggzcore_cb_game = temp;
			ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_LAUNCHED, pyggzcore_cb_game_hook);
			ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_LAUNCH_FAIL, pyggzcore_cb_game_hook);
			ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_NEGOTIATED, pyggzcore_cb_game_hook);
			ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_NEGOTIATE_FAIL, pyggzcore_cb_game_hook);
			ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_PLAYING, pyggzcore_cb_game_hook);
			break;
	}
	Py_INCREF(Py_None);
	result = Py_None;
	return result;
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyggzcore_methods[] =
{
	{"setHandler", pyggzcore_set_handler, METH_VARARGS},
	{"reload", pyggzcore_reload, METH_VARARGS},
	{NULL, NULL}
};

/**********************************************/
/* Internal callbacks                         */
/**********************************************/

static GGZHookReturn pyggzcore_cb_server_hook(unsigned int id, void *event_data, void *user_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(iO)", id, event_data);
	res = PyEval_CallObject(pyggzcore_cb_server, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzcore callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);

	return GGZ_HOOK_OK;
}

static GGZHookReturn pyggzcore_cb_room_hook(unsigned int id, void *event_data, void *user_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(iO)", id, event_data);
	res = PyEval_CallObject(pyggzcore_cb_room, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzcore callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);

	return GGZ_HOOK_OK;
}

static GGZHookReturn pyggzcore_cb_game_hook(unsigned int id, void *event_data, void *user_data)
{
	PyObject *arg, *res;

	arg = Py_BuildValue("(iO)", id, event_data);
	res = PyEval_CallObject(pyggzcore_cb_game, arg);
	if(res == NULL)
	{
		printf("----------------------------\n");
		printf("ERROR in pyggzcore callback!\n");
		PyErr_Print();
		printf("----------------------------\n");
	}
	Py_DECREF(arg);

	return GGZ_HOOK_OK;
}

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initggzcore(void)
{
	PyObject *core;
	GGZOptions opt;

	core = Py_InitModule("ggzcore", pyggzcore_methods);

	PyModule_AddIntConstant(core, "LOGIN", GGZ_LOGIN);
	PyModule_AddIntConstant(core, "LOGIN_GUEST", GGZ_LOGIN_GUEST);
	PyModule_AddIntConstant(core, "LOGIN_NEW", GGZ_LOGIN_NEW);

	PyModule_AddIntConstant(core, "EVENT_SERVER", GGZCORE_EVENT_SERVER);
	PyModule_AddIntConstant(core, "EVENT_ROOM", GGZCORE_EVENT_ROOM);
	PyModule_AddIntConstant(core, "EVENT_GAME", GGZCORE_EVENT_GAME);

	PyModule_AddIntConstant(core, "SERVER_CONNECTED", GGZ_CONNECTED);
	PyModule_AddIntConstant(core, "SERVER_CONNECT_FAIL", GGZ_CONNECT_FAIL);
	PyModule_AddIntConstant(core, "SERVER_NEGOTIATED", GGZ_NEGOTIATED);
	PyModule_AddIntConstant(core, "SERVER_NEGOTIATE_FAIL", GGZ_NEGOTIATE_FAIL);
	PyModule_AddIntConstant(core, "SERVER_LOGGED_IN", GGZ_LOGGED_IN);
	PyModule_AddIntConstant(core, "SERVER_LOGIN_FAIL", GGZ_LOGIN_FAIL);
	PyModule_AddIntConstant(core, "SERVER_MOTD_LOADED", GGZ_MOTD_LOADED);
	PyModule_AddIntConstant(core, "SERVER_ROOM_LIST", GGZ_ROOM_LIST);
	PyModule_AddIntConstant(core, "SERVER_TYPE_LIST", GGZ_TYPE_LIST);
	PyModule_AddIntConstant(core, "SERVER_ENTERED", GGZ_ENTERED);
	PyModule_AddIntConstant(core, "SERVER_ENTER_FAIL", GGZ_ENTER_FAIL);
	PyModule_AddIntConstant(core, "SERVER_LOGOUT", GGZ_LOGOUT);
	PyModule_AddIntConstant(core, "SERVER_NET_ERROR", GGZ_NET_ERROR);
	PyModule_AddIntConstant(core, "SERVER_PROTOCOL_ERROR", GGZ_PROTOCOL_ERROR);
	PyModule_AddIntConstant(core, "SERVER_CHAT_FAIL", GGZ_CHAT_FAIL);
	PyModule_AddIntConstant(core, "SERVER_STATE_CHANGE", GGZ_STATE_CHANGE);
	PyModule_AddIntConstant(core, "SERVER_CHANNEL_CONNECTED", GGZ_CHANNEL_CONNECTED);
	PyModule_AddIntConstant(core, "SERVER_CHANNEL_READY", GGZ_CHANNEL_READY);
	PyModule_AddIntConstant(core, "SERVER_CHANNEL_FAIL", GGZ_CHANNEL_FAIL);

	PyModule_AddIntConstant(core, "ROOM_PLAYER_LIST", GGZ_PLAYER_LIST);
	PyModule_AddIntConstant(core, "ROOM_TABLE_LIST", GGZ_TABLE_LIST);
	PyModule_AddIntConstant(core, "ROOM_CHAT_EVENT", GGZ_CHAT_EVENT);
	PyModule_AddIntConstant(core, "ROOM_ENTER", GGZ_ROOM_ENTER);
	PyModule_AddIntConstant(core, "ROOM_LEAVE", GGZ_ROOM_LEAVE);
	PyModule_AddIntConstant(core, "ROOM_TABLE_UPDATE", GGZ_TABLE_UPDATE);
	PyModule_AddIntConstant(core, "ROOM_TABLE_LAUNCHED", GGZ_TABLE_LAUNCHED);
	PyModule_AddIntConstant(core, "ROOM_TABLE_LAUNCH_FAIL", GGZ_TABLE_LAUNCH_FAIL);
	PyModule_AddIntConstant(core, "ROOM_TABLE_JOINED", GGZ_TABLE_JOINED);
	PyModule_AddIntConstant(core, "ROOM_TABLE_JOIN_FAIL", GGZ_TABLE_JOIN_FAIL);
	PyModule_AddIntConstant(core, "ROOM_TABLE_LEFT", GGZ_TABLE_LEFT);
	PyModule_AddIntConstant(core, "ROOM_TABLE_LEAVE_FAIL", GGZ_TABLE_LEAVE_FAIL);
	PyModule_AddIntConstant(core, "ROOM_PLAYER_LAG", GGZ_PLAYER_LAG);
	PyModule_AddIntConstant(core, "ROOM_PLAYER_STATS", GGZ_PLAYER_STATS);
	PyModule_AddIntConstant(core, "ROOM_PLAYER_COUNT", GGZ_PLAYER_COUNT);

	PyModule_AddIntConstant(core, "GAME_LAUNCHED", GGZ_GAME_LAUNCHED);
	PyModule_AddIntConstant(core, "GAME_LAUNCH_FAIL", GGZ_GAME_LAUNCH_FAIL);
	PyModule_AddIntConstant(core, "GAME_NEGOTIATED", GGZ_GAME_NEGOTIATED);
	PyModule_AddIntConstant(core, "GAME_NEGOTIATE_FAIL", GGZ_GAME_NEGOTIATE_FAIL);
	PyModule_AddIntConstant(core, "GAME_PLAYING", GGZ_GAME_PLAYING);

	PyModule_AddIntConstant(core, "STATE_OFFLINE", GGZ_STATE_OFFLINE);
	PyModule_AddIntConstant(core, "STATE_CONNECTING", GGZ_STATE_CONNECTING);
	PyModule_AddIntConstant(core, "STATE_ONLINE", GGZ_STATE_ONLINE);
	PyModule_AddIntConstant(core, "STATE_LOGGING_IN", GGZ_STATE_LOGGING_IN);
	PyModule_AddIntConstant(core, "STATE_LOGGED_IN", GGZ_STATE_LOGGED_IN);
	PyModule_AddIntConstant(core, "STATE_ENTERING_ROOM", GGZ_STATE_ENTERING_ROOM);
	PyModule_AddIntConstant(core, "STATE_IN_ROOM", GGZ_STATE_IN_ROOM);
	PyModule_AddIntConstant(core, "STATE_BETWEEN_ROOMS", GGZ_STATE_BETWEEN_ROOMS);
	PyModule_AddIntConstant(core, "STATE_LAUNCHING_TABLE", GGZ_STATE_LAUNCHING_TABLE);
	PyModule_AddIntConstant(core, "STATE_JOINING_TABLE", GGZ_STATE_JOINING_TABLE);
	PyModule_AddIntConstant(core, "STATE_AT_TABLE", GGZ_STATE_AT_TABLE);
	PyModule_AddIntConstant(core, "STATE_LEAVING_TABLE", GGZ_STATE_LEAVING_TABLE);
	PyModule_AddIntConstant(core, "STATE_LOGGING_OUT", GGZ_STATE_LOGGING_OUT);

	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_MODULES;
	ggzcore_init(opt);
}

