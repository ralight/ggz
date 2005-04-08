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

static GGZHookReturn pyggzcoresimple_cb_server_hook(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn pyggzcoresimple_cb_room_hook(unsigned int id, const void *event_data, const void *user_data);
static GGZHookReturn pyggzcoresimple_cb_game_hook(unsigned int id, const void *event_data, const void *user_data);

static void pyggzcoresimple_set_handler_game(void);
static PyObject *pyggzcoresimple_new_game(PyObject *self, PyObject *args);
static int pyggzcore_starttable(void);

/**********************************************/
/* Global variables                           */
/**********************************************/

static PyObject *core = NULL;
static PyObject *coreserver = NULL;
static PyObject *coreroom = NULL;
static PyObject *coregame = NULL;

static PyObject *roomlist = NULL;

static GGZServer *ggzserver = NULL;
static GGZRoom *ggzroom = NULL;
static GGZGame *ggzgame = NULL;
static GGZGameType *gametype = NULL;

static PyObject *pyggzcoresimple_cb_server = NULL;
static PyObject *pyggzcoresimple_cb_room = NULL;
static PyObject *pyggzcoresimple_cb_game = NULL;

static int join_table = -1;
static int launch_table = 0;
static int read_channel = 0;
static int read_game = 0;

/**********************************************/
/* Object definitions                         */
/**********************************************/

staticforward PyTypeObject pyggzcoresimple_ServerType;
staticforward PyTypeObject pyggzcoresimple_RoomType;
staticforward PyTypeObject pyggzcoresimple_GameType;

typedef struct
{
	PyObject_HEAD
}
pyggzcoresimple_ServerObject;

typedef struct
{
	PyObject_HEAD
}
pyggzcoresimple_RoomObject;

typedef struct
{
	PyObject_HEAD
}
pyggzcoresimple_GameObject;

static void pyggzcoresimple_delete_server(PyObject *self)
{
	if(ggzserver)
	{
		ggzcore_server_free(ggzserver);
		ggzserver = NULL;
	}
	coreserver = Py_None;

	PyObject_Del(self);
}

static void pyggzcoresimple_delete_room(PyObject *self)
{
	if(ggzroom)
	{
		/*ggzcore_room_free(ggzroom);*/
		/*ggzroom = NULL;*/ /*!!! this would also destroy the current one*/
	}
	coreroom = Py_None;

	PyObject_Del(self);
}

static void pyggzcoresimple_delete_game(PyObject *self)
{
	if(ggzgame)
	{
		/*ggzcore_game_free(ggzgame);
		ggzgame = NULL;*/
	}
	coregame = Py_None;

	PyObject_Del(self);
}

static PyTypeObject pyggzcoresimple_ServerType =
{
	PyObject_HEAD_INIT(NULL)
	0,
	"GGZCore Server",
	sizeof(pyggzcoresimple_ServerObject),
	0,
	pyggzcoresimple_delete_server,
};

static PyTypeObject pyggzcoresimple_RoomType =
{
	PyObject_HEAD_INIT(NULL)
	0,
	"GGZCore Room",
	sizeof(pyggzcoresimple_RoomObject),
	0,
	pyggzcoresimple_delete_room,
};

static PyTypeObject pyggzcoresimple_GameType =
{
	PyObject_HEAD_INIT(NULL)
	0,
	"GGZCore Game",
	sizeof(pyggzcoresimple_GameObject),
	0,
	pyggzcoresimple_delete_game,
};

/*****************************************************/
/* GGZCoreSimple object methods just like in C       */
/*****************************************************/

static PyObject *pyggzcoresimple_reload(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, "")) return NULL;
	ggzcore_reload();
	return Py_BuildValue("");
}

static PyObject *pyggzcoresimple_server_set_hostinfo(PyObject *self, PyObject *args)
{
	char *host;
	int port, use_tls;
	int ret;

	if(!PyArg_ParseTuple(args, "sii", &host, &port, &use_tls)) return NULL;
	ret = ggzcore_server_set_hostinfo(ggzserver, host, port, use_tls);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_set_logininfo(PyObject *self, PyObject *args)
{
	char *handle, *password;
	int type;
	int ret;

	if(!PyArg_ParseTuple(args, "iss", &type, &handle, &password)) return NULL;
	ret = ggzcore_server_set_logininfo(ggzserver, type, handle, password);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_get_state(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_server_get_state(ggzserver);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_get_tls(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_server_get_tls(ggzserver);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_connect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_server_connect(ggzserver);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_login(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_server_login(ggzserver);
	return Py_BuildValue("i", ret);
}

/*
static PyObject *pyggzcoresimple_server_motd(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_server_motd(ggzserver);
	return Py_BuildValue("i", ret);
}
*/

static PyObject *pyggzcoresimple_server_logout(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_server_logout(ggzserver);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_disconnect(PyObject *self, PyObject *args)
{
	int ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_server_disconnect(ggzserver);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_process(PyObject *self, PyObject *args)
{
	int ret, sret;
	fd_set set;
	int fd;
	struct timeval to;
	struct timeval *top;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	if(ggzcore_server_data_is_pending(ggzserver))
		ret = ggzcore_server_read_data(ggzserver, ggzcore_server_get_fd(ggzserver));
	else ret = 0;

	if(read_game)
		if(ggzcore_game_get_control_fd(ggzgame) != -1)
		{
			to.tv_sec = 0;
			to.tv_usec = 0;
			top = &to;

			fd = ggzcore_game_get_control_fd(ggzgame);
			FD_ZERO(&set);
			FD_SET(fd, &set);
			sret = select(fd + 1, &set, NULL, NULL, top);
			if(sret == 1)
				ggzcore_game_read_data(ggzgame);
		}
	if(read_channel)
		if(ggzcore_server_get_channel(ggzserver) != -1)
			ggzcore_server_read_data(ggzserver, ggzcore_server_get_channel(ggzserver));
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_server_join_room(PyObject *self, PyObject *args)
{
	int ret;
	char *room, *room2;
	int i, number;
	PyObject *item;

	if(!PyArg_ParseTuple(args, "s", &room)) return NULL;
	if(!roomlist) return Py_BuildValue("i", -1); // no room list available yet
	number = 0;
	for(i = 0; i < PyList_Size(roomlist); i++)
	{
		item = PyList_GetItem(roomlist, i);
		room2 = PyString_AsString(item);
		if(!strcmp(room, room2))
		{
			number = i;
			/*printf("(pyggzcoresimple) join room %s yields: %i\n", room, number);*/
		}
	}
	ret = ggzcore_server_join_room(ggzserver, number);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_room_get_name(PyObject *self, PyObject *args)
{
	char *ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_room_get_name(ggzroom);
	return Py_BuildValue("s", ret);
}

static PyObject *pyggzcoresimple_room_get_desc(PyObject *self, PyObject *args)
{
	char *ret;

	if(!PyArg_ParseTuple(args, "")) return NULL;
	ret = ggzcore_room_get_desc(ggzroom);
	return Py_BuildValue("s", ret);
}

static PyObject *pyggzcoresimple_room_chat(PyObject *self, PyObject *args)
{
	char *player, *message;
	int type;
	int ret;

	if(!PyArg_ParseTuple(args, "iss", &type, &player, &message)) return NULL;
	ret = ggzcore_room_chat(ggzroom, type, player, message);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_room_play(PyObject *self, PyObject *args)
{
	char *description;
	int type;
	int ret;

	if(!PyArg_ParseTuple(args, "si", &description, &type)) return NULL;
	ret = -1; //ggzcore_room_chat(ggzroom, type, player, message);
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_room_launch(PyObject *self, PyObject *args)
{
	int ret;

	launch_table = 1;
	ret = pyggzcore_starttable();
	return Py_BuildValue("i", ret);
}

static PyObject *pyggzcoresimple_room_join(PyObject *self, PyObject *args)
{
	int ret;
	int table;

	if(!PyArg_ParseTuple(args, "i", &table)) return NULL;
	join_table = table;
	ret = pyggzcore_starttable();
	return Py_BuildValue("i", ret);
}

static int pyggzcore_starttable(void)
{
	int ret;
	int i, modnum;
	GGZModule *module;

	ret = 1;
	module = NULL;
	gametype = ggzcore_room_get_gametype(ggzroom);
	modnum = ggzcore_module_get_num_by_type(
		ggzcore_gametype_get_name(gametype),
		ggzcore_gametype_get_prot_engine(gametype),
		ggzcore_gametype_get_prot_version(gametype));
	for(i = 0; i < modnum; i++)
	{
		module = ggzcore_module_get_nth_by_type(
			ggzcore_gametype_get_name(gametype),
			ggzcore_gametype_get_prot_engine(gametype),
			ggzcore_gametype_get_prot_version(gametype),
			i);
		if(ggzcore_module_get_environment(module) == GGZ_ENVIRONMENT_XWINDOW) break;
		if(ggzcore_module_get_environment(module) == GGZ_ENVIRONMENT_XFULLSCREEN) break;
		module = NULL;
	}
	if(!module) ret = 0;
	ggzgame = ggzcore_game_new();
	ggzcore_game_init(ggzgame, ggzserver, module);

	pyggzcoresimple_set_handler_game();

	coregame = pyggzcoresimple_new_game(NULL, NULL);
	PyModule_AddObject(core, "game", coregame);

	ggzcore_game_launch(ggzgame);

	return ret;
}

static void pyggzcoresimple_set_handler_server(void)
{
	if(!ggzserver) return;

	ggzcore_server_add_event_hook(ggzserver, GGZ_CONNECTED, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_CONNECT_FAIL, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_NEGOTIATED, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_NEGOTIATE_FAIL, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_LOGGED_IN, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_LOGIN_FAIL, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_MOTD_LOADED, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_ROOM_LIST, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_TYPE_LIST, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_ENTERED, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_ENTER_FAIL, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_LOGOUT, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_NET_ERROR, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_PROTOCOL_ERROR, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_CHAT_FAIL, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_STATE_CHANGE, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_CHANNEL_CONNECTED, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_CHANNEL_READY, pyggzcoresimple_cb_server_hook);
	ggzcore_server_add_event_hook(ggzserver, GGZ_CHANNEL_FAIL, pyggzcoresimple_cb_server_hook);
}

static void pyggzcoresimple_remove_handler_room(void)
{
	if(!ggzroom) return;

	ggzcore_room_remove_event_hook(ggzroom, GGZ_PLAYER_LIST, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_LIST, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_CHAT_EVENT, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_ROOM_ENTER, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_ROOM_LEAVE, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_UPDATE, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_LAUNCHED, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_LAUNCH_FAIL, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_JOINED, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_JOIN_FAIL, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_LEFT, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_TABLE_LEAVE_FAIL, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_PLAYER_LAG, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_PLAYER_STATS, pyggzcoresimple_cb_room_hook);
	ggzcore_room_remove_event_hook(ggzroom, GGZ_PLAYER_COUNT, pyggzcoresimple_cb_room_hook);
}

static void pyggzcoresimple_set_handler_room(void)
{
	if(!ggzroom) return;

	ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_LIST, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LIST, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_CHAT_EVENT, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_ROOM_ENTER, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_ROOM_LEAVE, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_UPDATE, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LAUNCHED, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LAUNCH_FAIL, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_JOINED, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_JOIN_FAIL, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LEFT, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_TABLE_LEAVE_FAIL, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_LAG, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_STATS, pyggzcoresimple_cb_room_hook);
	ggzcore_room_add_event_hook(ggzroom, GGZ_PLAYER_COUNT, pyggzcoresimple_cb_room_hook);
}

static void pyggzcoresimple_set_handler_game(void)
{
	if(!ggzgame) return;

	ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_LAUNCHED, pyggzcoresimple_cb_game_hook);
	ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_LAUNCH_FAIL, pyggzcoresimple_cb_game_hook);
	ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_NEGOTIATED, pyggzcoresimple_cb_game_hook);
	ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_NEGOTIATE_FAIL, pyggzcoresimple_cb_game_hook);
	ggzcore_game_add_event_hook(ggzgame, GGZ_GAME_PLAYING, pyggzcoresimple_cb_game_hook);
}

static PyObject *pyggzcoresimple_set_handler(PyObject *self, PyObject *args)
{
	int id;
	PyObject *result = NULL;
	PyObject *temp;

	if(!PyArg_ParseTuple(args, "iO", &id, &temp)) return NULL;
	Py_XINCREF(temp);
	switch(id)
	{
		case GGZCORE_EVENT_SERVER:
			Py_XDECREF(pyggzcoresimple_cb_server);
			pyggzcoresimple_cb_server = temp;

			pyggzcoresimple_set_handler_server();
			break;
		case GGZCORE_EVENT_ROOM:
			Py_XDECREF(pyggzcoresimple_cb_room);
			pyggzcoresimple_cb_room = temp;
			
			//pyggzcoresimple_set_handler_room();
			break;
		case GGZCORE_EVENT_GAME:
			Py_XDECREF(pyggzcoresimple_cb_game);
			pyggzcoresimple_cb_game = temp;

			//pyggzcoresimple_set_handler_game();
			break;
	}
	Py_INCREF(Py_None);
	result = Py_None;
	return result;
}

/**********************************************/
/* Function lookup table                      */
/**********************************************/

static PyMethodDef pyggzcoresimple_server_methods[] =
{
	{"set_hostinfo", pyggzcoresimple_server_set_hostinfo, METH_VARARGS},
	{"set_logininfo", pyggzcoresimple_server_set_logininfo, METH_VARARGS},
	{"get_state", pyggzcoresimple_server_get_state, METH_VARARGS},
	{"get_tls", pyggzcoresimple_server_get_tls, METH_VARARGS},
	{"connect", pyggzcoresimple_server_connect, METH_VARARGS},
	{"login", pyggzcoresimple_server_login, METH_VARARGS},
	/*{"motd", pyggzcoresimple_server_motd, METH_VARARGS},*/
	{"process", pyggzcoresimple_server_process, METH_VARARGS},
	{"logout", pyggzcoresimple_server_logout, METH_VARARGS},
	{"disconnect", pyggzcoresimple_server_disconnect, METH_VARARGS},
	{"join_room", pyggzcoresimple_server_join_room, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyMethodDef pyggzcoresimple_room_methods[] =
{
	{"get_name", pyggzcoresimple_room_get_name, METH_VARARGS},
	{"get_desc", pyggzcoresimple_room_get_desc, METH_VARARGS},
	{"chat", pyggzcoresimple_room_chat, METH_VARARGS},
	{"play", pyggzcoresimple_room_play, METH_VARARGS},
	{"join", pyggzcoresimple_room_join, METH_VARARGS},
	{"launch", pyggzcoresimple_room_launch, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyMethodDef pyggzcoresimple_game_methods[] =
{
	{NULL, NULL, 0}
};

static PyMethodDef pyggzcoresimple_methods[] =
{
	{"setHandler", pyggzcoresimple_set_handler, METH_VARARGS},
	{"reload", pyggzcoresimple_reload, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyObject *pyggzcoresimple_server_getattr(PyObject *self, char *name)
{
	PyObject *ret = NULL;

	ret = Py_FindMethod(pyggzcoresimple_server_methods, self, name);

	return ret;
}

static PyObject *pyggzcoresimple_room_getattr(PyObject *self, char *name)
{
	PyObject *ret = NULL;

	ret = Py_FindMethod(pyggzcoresimple_room_methods, self, name);

	return ret;
}

static PyObject *pyggzcoresimple_game_getattr(PyObject *self, char *name)
{
	PyObject *ret = NULL;

	ret = Py_FindMethod(pyggzcoresimple_game_methods, self, name);

	return ret;
}

static PyObject *pyggzcoresimple_new_server(PyObject *self, PyObject *args)
{
	pyggzcoresimple_ServerObject *server;

	pyggzcoresimple_ServerType.ob_type = &PyType_Type;

	pyggzcoresimple_ServerType.tp_methods = (struct PyMethodDef*)pyggzcoresimple_server_methods;
	pyggzcoresimple_ServerType.tp_getattr = (getattrfunc)pyggzcoresimple_server_getattr;

	server = PyObject_New(pyggzcoresimple_ServerObject, &pyggzcoresimple_ServerType);

	return (PyObject*)server;
}

static PyObject *pyggzcoresimple_new_room(PyObject *self, PyObject *args)
{
	pyggzcoresimple_RoomObject *room;

	pyggzcoresimple_RoomType.ob_type = &PyType_Type;

	pyggzcoresimple_RoomType.tp_methods = (struct PyMethodDef*)pyggzcoresimple_room_methods;
	pyggzcoresimple_RoomType.tp_getattr = (getattrfunc)pyggzcoresimple_room_getattr;

	room = PyObject_New(pyggzcoresimple_RoomObject, &pyggzcoresimple_RoomType);

	return (PyObject*)room;
}

static PyObject *pyggzcoresimple_new_game(PyObject *self, PyObject *args)
{
	pyggzcoresimple_GameObject *game;

	pyggzcoresimple_GameType.ob_type = &PyType_Type;

	pyggzcoresimple_GameType.tp_methods = (struct PyMethodDef*)pyggzcoresimple_game_methods;
	pyggzcoresimple_GameType.tp_getattr = (getattrfunc)pyggzcoresimple_game_getattr;

	game = PyObject_New(pyggzcoresimple_GameObject, &pyggzcoresimple_GameType);

	return (PyObject*)game;
}

/**********************************************/
/* Internal callbacks                         */
/**********************************************/

static GGZHookReturn pyggzcoresimple_cb_server_hook(unsigned int id, const void *event_data, const void *user_data)
{
	PyObject *arg, *res;
	PyObject *element;
	char *str;
	int i;
	GGZRoom *room;
	const GGZMotdEventData *motd;

	/*printf("(pyggzcoresimple) server event: %i %p %p\n", id, event_data, user_data);*/

	arg = NULL;
	str = NULL;

	if((id == GGZ_CONNECT_FAIL) || (id == GGZ_NEGOTIATE_FAIL))
	{
		str = strdup((char*)event_data);
	}

	if(id == GGZ_MOTD_LOADED)
	{
		motd = event_data;

		if((motd) && (motd->motd))
		{
			str = strdup(motd->motd);
		}
	}
	if(id == GGZ_ROOM_LIST)
	{
		roomlist = PyList_New(0);
		//ggzcore_server_get_num_rooms(ggzserver));
		for(i = 0; i < ggzcore_server_get_num_rooms(ggzserver); i++)
		{
			room = ggzcore_server_get_nth_room(ggzserver, i);
			element = Py_BuildValue("s", ggzcore_room_get_name(room));
			PyList_Append(roomlist, element);
		}
		arg = Py_BuildValue("(iO)", id, roomlist);
	}
	if(id == GGZ_ENTERED)
	{
		pyggzcoresimple_remove_handler_room();
		ggzroom = ggzcore_server_get_cur_room(ggzserver);
		pyggzcoresimple_set_handler_room();

		coreroom = pyggzcoresimple_new_room(NULL, NULL);
		PyModule_AddObject(core, "room", coreroom);
	}


	if(!arg)
	{
		//arg = Py_BuildValue("(iO)", id, event_data);
		arg = Py_BuildValue("(is)", id, str);
	}
	res = PyEval_CallObject(pyggzcoresimple_cb_server, arg);
	if(res == NULL)
	{
		printf("----------------------------------\n");
		printf("ERROR in pyggzcoresimple callback!\n");
		PyErr_Print();
		printf("----------------------------------\n");
	}
//	Py_DECREF(arg);

	if(str) free(str);

	if(id == GGZ_LOGGED_IN)
	{
		ggzcore_server_list_rooms(ggzserver, 0, 1);
		ggzcore_server_list_gametypes(ggzserver, 1);
	}
	if(id == GGZ_ENTERED)
	{
		ggzcore_room_list_players(ggzroom);
		ggzcore_room_list_tables(ggzroom, -1, 0);
	}
	if(id == GGZ_CHANNEL_CONNECTED)
	{
		read_channel = 1;
	}
	if(id == GGZ_CHANNEL_READY)
	{
		ggzcore_game_set_server_fd(ggzgame, ggzcore_server_get_channel(ggzserver));
		read_channel = 0;
	}

	return GGZ_HOOK_OK;
}

static const char *playertype_to_string(GGZPlayerType t)
{
	switch(t)
	{
		case GGZ_PLAYER_NORMAL:
			return "registered";
		case GGZ_PLAYER_GUEST:
			return "guest";
		case GGZ_PLAYER_ADMIN:
			return "admin";
		case GGZ_PLAYER_BOT:
			return "bot";
		case GGZ_PLAYER_UNKNOWN:
		default:
			return "unknown";
	}
	return NULL;
}

static GGZHookReturn pyggzcoresimple_cb_room_hook(unsigned int id, const void *event_data, const void *user_data)
{
	PyObject *arg, *res;
	PyObject *list, *element;
	char *str;
	GGZChatEventData *cd;
	PyObject *playerlist, *tablelist;
	GGZPlayer *player;
	GGZTable *table;
	int i, j;

	/*printf("(pyggzcore) room event: %i %p %p\n", id, event_data, user_data);*/

	arg = NULL;
	str = NULL;

	if(id == GGZ_CHAT_EVENT)
	{
		cd = (GGZChatEventData*)event_data;
		/*printf("(pyggzcore) chat data: %i %s %s\n", cd->type, cd->sender, cd->message);*/

		list = PyList_New(0);
		element = Py_BuildValue("i", cd->type);
		PyList_Append(list, element);
		element = Py_BuildValue("s", cd->sender);
		PyList_Append(list, element);
		element = Py_BuildValue("s", cd->message);
		PyList_Append(list, element);

		arg = Py_BuildValue("(iO)", id, list);

		//str = strdup((char*)event_data);
	}
	if(id == GGZ_PLAYER_LIST)
	{
		playerlist = PyList_New(0);
		//ggzcore_server_get_num_rooms(ggzserver));
		for(i = 0; i < ggzcore_room_get_num_players(ggzroom); i++)
		{
			player = ggzcore_room_get_nth_player(ggzroom, i);
			element = Py_BuildValue("ss",
				ggzcore_player_get_name(player),
				playertype_to_string(ggzcore_player_get_type(player)));
			PyList_Append(playerlist, element);
		}
		arg = Py_BuildValue("(iO)", id, playerlist);
	}
	if((id == GGZ_TABLE_LIST) || (id == GGZ_TABLE_UPDATE))
	{
		tablelist = PyList_New(0);
		for(i = 0; i < ggzcore_room_get_num_tables(ggzroom); i++)
		{
			table = ggzcore_room_get_nth_table(ggzroom, i);
			element = Py_BuildValue("si",
				ggzcore_table_get_desc(table),
				ggzcore_table_get_num_seats(table));
			PyList_Append(tablelist, element);
			for(j = 0; j < ggzcore_table_get_num_seats(table); j++)
			{
				if(ggzcore_table_get_nth_player_type(table, j) == GGZ_SEAT_PLAYER)
					element = Py_BuildValue("s",
						ggzcore_table_get_nth_player_name(table, j));
				else if(ggzcore_table_get_nth_player_type(table, j) == GGZ_SEAT_BOT)
					element = Py_BuildValue("s", "(bot)");
				else
					element = Py_BuildValue("s", "");
				PyList_Append(tablelist, element);
			}
		}
		arg = Py_BuildValue("(iO)", id, tablelist);
	}
	if(id == GGZ_TABLE_LAUNCHED) /* launched or joined? */
	{
	}
	if(id == GGZ_TABLE_JOINED) /* launched or joined? */
	{
	}
	if(id == GGZ_TABLE_LEFT)
	{
		ggzcore_game_free(ggzgame);
		ggzgame = NULL;
		read_game = 0;
	}

	if(!arg)
	{
		arg = Py_BuildValue("(is)", id, str);
	}
	res = PyEval_CallObject(pyggzcoresimple_cb_room, arg);
	if(res == NULL)
	{
		printf("----------------------------------\n");
		printf("ERROR in pyggzcoresimple callback!\n");
		PyErr_Print();
		printf("----------------------------------\n");
	}
//	Py_DECREF(arg);

	if(str) free(str);

	return GGZ_HOOK_OK;
}

static GGZHookReturn pyggzcoresimple_cb_game_hook(unsigned int id, const void *event_data, const void *user_data)
{
	PyObject *arg, *res;
	char *str;
	GGZTable *table;

	/*printf("(pyggzcore) game event: %i %p %p\n", id, event_data, user_data);*/

	arg = NULL;
	str = NULL;

	if(id == GGZ_GAME_LAUNCHED)
	{
		read_game = 1;
	}
	if(id == GGZ_GAME_NEGOTIATED)
	{
		ggzcore_server_create_channel(ggzserver);
	}
	if(id == GGZ_GAME_PLAYING)
	{
		if(launch_table)
		{
			table = ggzcore_table_new();
			ggzcore_table_init(table, gametype, "Vibora game", 2);
			ggzcore_table_set_seat(table, 1, GGZ_SEAT_BOT, NULL);

			ggzcore_room_launch_table(ggzroom, table);

			//ggzcore_table_free(table);
		}
		else
		{
			ggzcore_room_join_table(ggzroom, join_table, 0);
		}
		join_table = -1;
		launch_table = 0;
	}

	if(!arg)
	{
		arg = Py_BuildValue("(is)", id, str);
	}
	res = PyEval_CallObject(pyggzcoresimple_cb_game, arg);
	if(res == NULL)
	{
		printf("----------------------------------\n");
		printf("ERROR in pyggzcoresimple callback!\n");
		PyErr_Print();
		printf("----------------------------------\n");
	}
//	Py_DECREF(arg);

	if(str) free(str);

	return GGZ_HOOK_OK;
}

/**********************************************/
/* Initialization code and constants          */
/**********************************************/

void initggzcoresimple(void)
{
	PyObject *coreserver;
	GGZOptions opt;

	core = Py_InitModule("ggzcoresimple", pyggzcoresimple_methods);

	PyModule_AddIntConstant(core, "LOGIN", GGZ_LOGIN);
	PyModule_AddIntConstant(core, "LOGIN_GUEST", GGZ_LOGIN_GUEST);
	PyModule_AddIntConstant(core, "LOGIN_NEW", GGZ_LOGIN_NEW);

	PyModule_AddIntConstant(core, "CHAT_NORMAL", GGZ_CHAT_NORMAL);
	PyModule_AddIntConstant(core, "CHAT_ANNOUNCE", GGZ_CHAT_ANNOUNCE);
	PyModule_AddIntConstant(core, "CHAT_BEEP", GGZ_CHAT_BEEP);
	PyModule_AddIntConstant(core, "CHAT_PERSONAL", GGZ_CHAT_PERSONAL);
	PyModule_AddIntConstant(core, "CHAT_TABLE", GGZ_CHAT_TABLE);

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

	PyModule_AddIntConstant(core, "ENVIRONMENT_PASSIVE", GGZ_ENVIRONMENT_PASSIVE);
	PyModule_AddIntConstant(core, "ENVIRONMENT_CONSOLE", GGZ_ENVIRONMENT_CONSOLE);
	PyModule_AddIntConstant(core, "ENVIRONMENT_FRAMEBUFFER", GGZ_ENVIRONMENT_FRAMEBUFFER);
	PyModule_AddIntConstant(core, "ENVIRONMENT_XWINDOW", GGZ_ENVIRONMENT_XWINDOW);
	PyModule_AddIntConstant(core, "ENVIRONMENT_XFULLSCREEN", GGZ_ENVIRONMENT_XFULLSCREEN);

	coreserver = pyggzcoresimple_new_server(NULL, NULL);
	coreroom = Py_None;
	coregame = Py_None;
	PyModule_AddObject(core, "server", coreserver);
	PyModule_AddObject(core, "room", coreroom);
	PyModule_AddObject(core, "game", coregame);

	ggzserver = ggzcore_server_new();

	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_MODULES;
	ggzcore_init(opt);
}

