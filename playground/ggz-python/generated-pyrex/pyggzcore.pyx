#!/usr/bin/python

'''
Compile with warning:
    warning: use of cast expressions as lvalues is deprecated
see:
    http://lists.copyleft.no/pipermail/pyrex/2004-April/000690.html
-- hashao
'''

include "pyggzcommon.pxi"

cdef extern from "ggzcore.h":
    cdef enum:
        GGZCORE_VERSION_MAJOR "GGZCORE_VERSION_MAJOR"
        GGZCORE_VERSION_MINOR "GGZCORE_VERSION_MINOR"
        GGZCORE_VERSION_MICRO "GGZCORE_VERSION_MICRO"

    cdef char* GGZCORE_VERSION_IFACE "GGZCORE_VERSION_IFACE"
    cdef char* GGZCORE_DBG_CONF "GGZCORE_DBG_CONF"
    cdef char* GGZCORE_DBG_GAME "GGZCORE_DBG_GAME"
    cdef char* GGZCORE_DBG_HOOK "GGZCORE_DBG_HOOK"
    cdef char* GGZCORE_DBG_MODULE "GGZCORE_DBG_MODULE"
    cdef char* GGZCORE_DBG_NET "GGZCORE_DBG_NET" 
    cdef char* GGZCORE_DBG_POLL "GGZCORE_DBG_POLL"
    cdef char* GGZCORE_DBG_ROOM "GGZCORE_DBG_ROOM"
    cdef char* GGZCORE_DBG_SERVER "GGZCORE_DBG_SERVER"
    cdef char* GGZCORE_DBG_STATE "GGZCORE_DBG_STATE"
    cdef char* GGZCORE_DBG_TABLE "GGZCORE_DBG_TABLE"
    cdef char* GGZCORE_DBG_XML "GGZCORE_DBG_XML" 

    ctypedef enum GGZOptionFlags:
        GGZ_OPT_PARSER "GGZ_OPT_PARSER"
        GGZ_OPT_MODULES "GGZ_OPT_MODULES"
        GGZ_OPT_THREADED_IO "GGZ_OPT_THREADED_IO"

    ctypedef struct GGZOptions:
        GGZOptionFlags flags

    int ggzcore_init(GGZOptions options)

    void ggzcore_destroy()

    ctypedef enum GGZHookReturn:
        GGZ_HOOK_OK
        GGZ_HOOK_REMOVE
        GGZ_HOOK_ERROR
        GGZ_HOOK_CRISIS

    ctypedef GGZHookReturn (*GGZHookFunc)(unsigned int id, 
                                         void* event_data, 
                                         void* user_data)
    ctypedef void (*GGZDestroyFunc)(void* data)

    ctypedef enum GGZLoginType:
        GGZ_LOGIN
        GGZ_LOGIN_GUEST
        GGZ_LOGIN_NEW

    ctypedef enum GGZPlayerType:
        GGZ_PLAYER_NONE
        GGZ_PLAYER_NORMAL
        GGZ_PLAYER_GUEST
        GGZ_PLAYER_ADMIN

    ctypedef struct GGZErrorEventData:
        char message[128]
        GGZClientReqError status

    ctypedef enum GGZServerEvent:
        GGZ_CONNECTED
        GGZ_CONNECT_FAIL
        GGZ_NEGOTIATED
        GGZ_NEGOTIATE_FAIL
        GGZ_LOGGED_IN
        GGZ_LOGIN_FAIL
        GGZ_MOTD_LOADED
        GGZ_ROOM_LIST
        GGZ_TYPE_LIST
        GGZ_ENTERED
        GGZ_ENTER_FAIL
        GGZ_LOGOUT
        GGZ_NET_ERROR
        GGZ_PROTOCOL_ERROR
        GGZ_CHAT_FAIL
        GGZ_STATE_CHANGE
        GGZ_CHANNEL_CONNECTED
        GGZ_CHANNEL_READY
        GGZ_CHANNEL_FAIL

    ctypedef struct GGZChatEventData:
        GGZChatType type
        char *sender
        char *message

    ctypedef struct GGZTableLeaveEventData:
        GGZLeaveType reason
        char *player

    ctypedef enum GGZRoomEvent:
        GGZ_PLAYER_LIST
        GGZ_TABLE_LIST
        GGZ_CHAT_EVENT
        GGZ_ROOM_ENTER
        GGZ_ROOM_LEAVE
        GGZ_TABLE_UPDATE
        GGZ_TABLE_LAUNCHED
        GGZ_TABLE_LAUNCH_FAIL
        GGZ_TABLE_JOINED
        GGZ_TABLE_JOIN_FAIL
        GGZ_TABLE_LEFT
        GGZ_TABLE_LEAVE_FAIL
        GGZ_PLAYER_LAG
        GGZ_PLAYER_STATS

    ctypedef enum GGZGameEvent:
        GGZ_GAME_LAUNCHED
        GGZ_GAME_LAUNCH_FAIL
        GGZ_GAME_NEGOTIATED
        GGZ_GAME_NEGOTIATE_FAIL
        GGZ_GAME_PLAYING
        GGZ_GAME_OVER
        GGZ_GAME_IO_ERROR
        GGZ_GAME_PROTO_ERROR

    ctypedef enum GGZStateID:
        GGZ_STATE_OFFLINE
        GGZ_STATE_CONNECTING
        GGZ_STATE_ONLINE
        GGZ_STATE_LOGGING_IN
        GGZ_STATE_LOGGED_IN
        GGZ_STATE_ENTERING_ROOM
        GGZ_STATE_IN_ROOM
        GGZ_STATE_BETWEEN_ROOMS
        GGZ_STATE_LAUNCHING_TABLE
        GGZ_STATE_JOINING_TABLE
        GGZ_STATE_AT_TABLE
        GGZ_STATE_LEAVING_TABLE
        GGZ_STATE_LOGGING_OUT


    ctypedef extern struct       GGZNet
    ctypedef extern struct    GGZServer
    ctypedef extern struct      GGZRoom
    ctypedef extern struct    GGZPlayer
    ctypedef extern struct     GGZTable
    ctypedef extern struct  GGZGameType
    ctypedef extern struct    GGZModule
    ctypedef extern struct      GGZGame


    GGZServer* ggzcore_server_new()

    int ggzcore_server_reset(GGZServer *server)
    int ggzcore_server_add_event_hook(GGZServer *server,
                                      GGZServerEvent event, 
                                      GGZHookFunc func)

    int ggzcore_server_add_event_hook_full(GGZServer *server,
                                           GGZServerEvent event, 
                                           GGZHookFunc func,
                                           void *data)

    int ggzcore_server_remove_event_hook(GGZServer *server,
                                         GGZServerEvent event, 
                                         GGZHookFunc func)

    int ggzcore_server_remove_event_hook_id(GGZServer *server,
                                            GGZServerEvent event, 
                                            unsigned int hook_id)

    int ggzcore_server_set_hostinfo(GGZServer *server, 
                                    char *host, 
                                    unsigned int port,
                                    unsigned int use_tls)

    int ggzcore_server_set_logininfo(GGZServer *server, 
                                     GGZLoginType type, 
                                     char *handle, 
                                     char *password)

    int          ggzcore_server_log_session(GGZServer *server, char *filename)
    char*        ggzcore_server_get_host(GGZServer *server)
    int          ggzcore_server_get_port(GGZServer *server)
    GGZLoginType ggzcore_server_get_type(GGZServer *server)
    char*        ggzcore_server_get_handle(GGZServer *server)
    char*        ggzcore_server_get_password(GGZServer *server)
    int          ggzcore_server_get_fd(GGZServer *server)
    int          ggzcore_server_get_channel(GGZServer *server)
    GGZStateID   ggzcore_server_get_state(GGZServer *server)
    int          ggzcore_server_get_tls(GGZServer *server)
    int          ggzcore_server_get_num_rooms(GGZServer *server)
    GGZRoom*     ggzcore_server_get_cur_room(GGZServer *server)
    GGZRoom*     ggzcore_server_get_nth_room(GGZServer *server, 
                                             unsigned int num)
    int          ggzcore_server_get_num_gametypes(GGZServer *server)
    GGZGameType* ggzcore_server_get_nth_gametype(GGZServer *server, 
                                            unsigned int num)
    GGZGame* ggzcore_server_get_cur_game(GGZServer *server)

    int ggzcore_server_is_online(GGZServer *server)
    int ggzcore_server_is_logged_in(GGZServer *server)
    int ggzcore_server_is_in_room(GGZServer *server)
    int ggzcore_server_is_at_table(GGZServer *server)

    int ggzcore_server_connect(GGZServer *server)
    int ggzcore_server_create_channel(GGZServer *server)
    int ggzcore_server_login(GGZServer *server)
    int ggzcore_server_motd(GGZServer *server)
    int ggzcore_server_list_rooms(GGZServer *server, int type, char verbose)
    int ggzcore_server_list_gametypes(GGZServer *server, char verbose)
    int ggzcore_server_join_room(GGZServer *server, unsigned int room)
    int ggzcore_server_logout(GGZServer *server)
    int ggzcore_server_disconnect(GGZServer *server)
    int ggzcore_server_data_is_pending(GGZServer *server)
    int ggzcore_server_read_data(GGZServer *server, int fd)
    void ggzcore_server_free(GGZServer *server)

    GGZRoom* ggzcore_room_new()
    int ggzcore_room_init(GGZRoom *room, 
                          GGZServer *server, 
                          unsigned int id, 
                          char *name, 
                          unsigned int game, 
                          char *desc)

    void ggzcore_room_free(GGZRoom *room)
    char*        ggzcore_room_get_name(GGZRoom *room)
    char*        ggzcore_room_get_desc(GGZRoom *room)
    GGZGameType* ggzcore_room_get_gametype(GGZRoom *room)
    int        ggzcore_room_get_num_players(GGZRoom *room)
    GGZPlayer* ggzcore_room_get_nth_player(GGZRoom *room, unsigned int num)
    int       ggzcore_room_get_num_tables(GGZRoom *room)
    GGZTable* ggzcore_room_get_nth_table(GGZRoom *room, unsigned int num)
    GGZTable* ggzcore_room_get_table_by_id(GGZRoom *room, unsigned int id)

    int ggzcore_room_add_event_hook(GGZRoom *room,
                                    GGZRoomEvent event, 
                                    GGZHookFunc func)

    int ggzcore_room_add_event_hook_full(GGZRoom *room,
                                         GGZRoomEvent event, 
                                         GGZHookFunc func,
                                         void *data)

    int ggzcore_room_remove_event_hook(GGZRoom *room,
                                       GGZRoomEvent event, 
                                       GGZHookFunc func)

    int ggzcore_room_remove_event_hook_id(GGZRoom *room,
                                          GGZRoomEvent event, 
                                          unsigned int hook_id)
    int ggzcore_room_list_players(GGZRoom *room)
    int ggzcore_room_list_tables(GGZRoom *room,
                                 int type,
                                 char cglobal)

    int ggzcore_room_chat(GGZRoom *room,
                          GGZChatType opcode,
                          char *player,
                          char *msg)

    int ggzcore_room_launch_table(GGZRoom *room, GGZTable *table)
    int ggzcore_room_join_table(GGZRoom *room, unsigned int table_id, 
                                int spectator)
    int ggzcore_room_leave_table(GGZRoom *room, int force)


    char*         ggzcore_player_get_name(GGZPlayer *player)
    GGZPlayerType ggzcore_player_get_type(GGZPlayer *player)
    GGZTable*     ggzcore_player_get_table(GGZPlayer *player)
    int	          ggzcore_player_get_lag(GGZPlayer *player)

    int ggzcore_player_get_record(GGZPlayer *player,
                                  int *wins, int *losses,
                                  int *ties, int *forfeits)
    int ggzcore_player_get_rating(GGZPlayer *player, int *rating)
    int ggzcore_player_get_ranking(GGZPlayer *player, int *ranking)
    int ggzcore_player_get_highscore(GGZPlayer *player, long *highscore)


    GGZTable* ggzcore_table_new()
    int ggzcore_table_init(GGZTable *table,
                           GGZGameType *gametype,
                           char *desc,
                           unsigned int num_seats)
    void ggzcore_table_free(GGZTable *table)
    int ggzcore_table_set_seat(GGZTable *table,
                               unsigned int seat,
                               GGZSeatType type,
                               char *name)

    int ggzcore_table_remove_player(GGZTable *table, char *name)
    int           ggzcore_table_get_id(GGZTable *table)
    GGZGameType*  ggzcore_table_get_type(GGZTable *table)
    char*         ggzcore_table_get_desc(GGZTable *table)
    GGZTableState ggzcore_table_get_state(GGZTable *table)
    int           ggzcore_table_get_num_seats(GGZTable *table)
    int           ggzcore_table_set_desc(GGZTable *table, char *desc)
    int           ggzcore_table_get_seat_count(GGZTable *table, GGZSeatType type)
    char* ggzcore_table_get_nth_player_name(GGZTable *table,
                                                   unsigned int num)
    int ggzcore_table_get_num_spectator_seats(GGZTable *table)
    char* ggzcore_table_get_nth_spectator_name(GGZTable *table,
                                                     unsigned int num)
    GGZSeatType ggzcore_table_get_nth_player_type(GGZTable *table, 
                                                   unsigned int num)


    char* ggzcore_gametype_get_name(GGZGameType *type)
    char* ggzcore_gametype_get_prot_engine(GGZGameType *type)
    char* ggzcore_gametype_get_prot_version(GGZGameType *type)
    char* ggzcore_gametype_get_version(GGZGameType *type)
    char* ggzcore_gametype_get_author(GGZGameType *type)
    char* ggzcore_gametype_get_url(GGZGameType *type)
    char* ggzcore_gametype_get_desc(GGZGameType *type)
    int ggzcore_gametype_get_max_players(GGZGameType *type)
    int ggzcore_gametype_get_max_bots(GGZGameType *type)
    int ggzcore_gametype_get_spectators_allowed(GGZGameType *type)
    int ggzcore_gametype_num_players_is_valid(GGZGameType *type, unsigned int num)
    int ggzcore_gametype_num_bots_is_valid(GGZGameType *type, unsigned int num)


    int ggzcore_conf_initialize	(char *g_path, char *u_path)
    int ggzcore_conf_write_string(char *section, 
                                  char *key, 
                                  char *value)
    int ggzcore_conf_write_int(char *section, 
                               char *key, 
                               int value)
    int ggzcore_conf_write_list(char *section, 
                                char *key, 
                                int argc, 
                                char **argv)
    char * ggzcore_conf_read_string(char *section, 
                                    char *key, 
                                    char *locdef)
    int ggzcore_conf_read_int(char *section, char *key, int locdef)
    int ggzcore_conf_read_list(char *section, 
                               char *key, 
                               int *argcp, 
                               char ***argvp)
    int ggzcore_conf_remove_section(char *section)
    int ggzcore_conf_remove_key(char *section, char *key)
    int ggzcore_conf_commit()


    unsigned int ggzcore_module_get_num()
    int ggzcore_module_add(char *name,
                           char *version,
                           char *prot_engine,
                           char *prot_version,
                           char *author,
                           char *frontend,
                           char *url,
                           char *exe_path,
                           char *icon_path,
                           char *help_path)

    int ggzcore_module_get_num_by_type(char *game, 
                                       char *engine,
                                       char *version)

    GGZModule* ggzcore_module_get_nth_by_type(char *game, 
                                              char *engine,
                                              char *version,
                                              unsigned int num)

    int ggzcore_module_launch(GGZModule *module)
    char* ggzcore_module_get_name(GGZModule *module)
    char* ggzcore_module_get_version(GGZModule *module)
    char* ggzcore_module_get_prot_engine(GGZModule *module)
    char* ggzcore_module_get_prot_version(GGZModule *module)
    char* ggzcore_module_get_author(GGZModule *module)
    char* ggzcore_module_get_frontend(GGZModule *module)
    char* ggzcore_module_get_url(GGZModule *module)
    char* ggzcore_module_get_icon_path(GGZModule *module)
    char* ggzcore_module_get_help_path(GGZModule *module)
    char** ggzcore_module_get_argv(GGZModule *module)


    GGZGame* ggzcore_game_new()
    int ggzcore_game_init(GGZGame *game, GGZServer *server, GGZModule *module)
    void ggzcore_game_free(GGZGame *game)
    int ggzcore_game_add_event_hook(GGZGame *game,
                                    GGZGameEvent event, 
                                    GGZHookFunc func)
    int ggzcore_game_add_event_hook_full(GGZGame *game,
                                         GGZGameEvent event, 
                                         GGZHookFunc func,
                                         void *data)
    int ggzcore_game_remove_event_hook(GGZGame *game,
                                       GGZGameEvent event, 
                                       GGZHookFunc func)
    int ggzcore_game_remove_event_hook_id(GGZGame *game,
                                          GGZGameEvent event, 
                                          unsigned int hook_id)

    int  ggzcore_game_get_control_fd(GGZGame *game)
    void ggzcore_game_set_server_fd(GGZGame *game, unsigned int fd)
    GGZModule* ggzcore_game_get_module(GGZGame *game)
    int ggzcore_game_launch(GGZGame *game)
    int ggzcore_game_read_data(GGZGame *game)

# Export to python module.
VERSION_MAJOR = GGZCORE_VERSION_MAJOR
VERSION_MINOR = GGZCORE_VERSION_MINOR
VERSION_MICRO = GGZCORE_VERSION_MICRO

VERSION_IFACE = GGZCORE_VERSION_IFACE
DBG_CONF = GGZCORE_DBG_CONF
DBG_GAME = GGZCORE_DBG_GAME
DBG_HOOK = GGZCORE_DBG_HOOK
DBG_MODULE = GGZCORE_DBG_MODULE
DBG_NET = GGZCORE_DBG_NET 
DBG_POLL = GGZCORE_DBG_POLL
DBG_ROOM = GGZCORE_DBG_ROOM
DBG_SERVER = GGZCORE_DBG_SERVER
DBG_STATE = GGZCORE_DBG_STATE
DBG_TABLE = GGZCORE_DBG_TABLE
DBG_XML = GGZCORE_DBG_XML 

OPT_PARSER = GGZ_OPT_PARSER
OPT_MODULES = GGZ_OPT_MODULES
OPT_THREADED_IO = GGZ_OPT_THREADED_IO

HOOK_OK = GGZ_HOOK_OK
HOOK_REMOVE = GGZ_HOOK_REMOVE
HOOK_ERROR = GGZ_HOOK_ERROR
HOOK_CRISIS = GGZ_HOOK_CRISIS

LOGIN = GGZ_LOGIN
LOGIN_GUEST = GGZ_LOGIN_GUEST
LOGIN_NEW = GGZ_LOGIN_NEW

PLAYER_NONE = GGZ_PLAYER_NONE
PLAYER_NORMAL = GGZ_PLAYER_NORMAL
PLAYER_GUEST = GGZ_PLAYER_GUEST
PLAYER_ADMIN = GGZ_PLAYER_ADMIN

CONNECTED = GGZ_CONNECTED
CONNECT_FAIL = GGZ_CONNECT_FAIL
NEGOTIATED = GGZ_NEGOTIATED
NEGOTIATE_FAIL = GGZ_NEGOTIATE_FAIL
LOGGED_IN = GGZ_LOGGED_IN
LOGIN_FAIL = GGZ_LOGIN_FAIL
MOTD_LOADED = GGZ_MOTD_LOADED
ROOM_LIST = GGZ_ROOM_LIST
TYPE_LIST = GGZ_TYPE_LIST
ENTERED = GGZ_ENTERED
ENTER_FAIL = GGZ_ENTER_FAIL
LOGOUT = GGZ_LOGOUT
NET_ERROR = GGZ_NET_ERROR
PROTOCOL_ERROR = GGZ_PROTOCOL_ERROR
CHAT_FAIL = GGZ_CHAT_FAIL
STATE_CHANGE = GGZ_STATE_CHANGE
CHANNEL_CONNECTED = GGZ_CHANNEL_CONNECTED
CHANNEL_READY = GGZ_CHANNEL_READY
CHANNEL_FAIL = GGZ_CHANNEL_FAIL

PLAYER_LIST = GGZ_PLAYER_LIST
TABLE_LIST = GGZ_TABLE_LIST
CHAT_EVENT = GGZ_CHAT_EVENT
ROOM_ENTER = GGZ_ROOM_ENTER
ROOM_LEAVE = GGZ_ROOM_LEAVE
TABLE_UPDATE = GGZ_TABLE_UPDATE
TABLE_LAUNCHED = GGZ_TABLE_LAUNCHED
TABLE_LAUNCH_FAIL = GGZ_TABLE_LAUNCH_FAIL
TABLE_JOINED = GGZ_TABLE_JOINED
TABLE_JOIN_FAIL = GGZ_TABLE_JOIN_FAIL
TABLE_LEFT = GGZ_TABLE_LEFT
TABLE_LEAVE_FAIL = GGZ_TABLE_LEAVE_FAIL
PLAYER_LAG = GGZ_PLAYER_LAG
PLAYER_STATS = GGZ_PLAYER_STATS

GAME_LAUNCHED = GGZ_GAME_LAUNCHED
GAME_LAUNCH_FAIL = GGZ_GAME_LAUNCH_FAIL
GAME_NEGOTIATED = GGZ_GAME_NEGOTIATED
GAME_NEGOTIATE_FAIL = GGZ_GAME_NEGOTIATE_FAIL
GAME_PLAYING = GGZ_GAME_PLAYING
GAME_OVER = GGZ_GAME_OVER
GAME_IO_ERROR = GGZ_GAME_IO_ERROR
GAME_PROTO_ERROR = GGZ_GAME_PROTO_ERROR

STATE_OFFLINE = GGZ_STATE_OFFLINE
STATE_CONNECTING = GGZ_STATE_CONNECTING
STATE_ONLINE = GGZ_STATE_ONLINE
STATE_LOGGING_IN = GGZ_STATE_LOGGING_IN
STATE_LOGGED_IN = GGZ_STATE_LOGGED_IN
STATE_ENTERING_ROOM = GGZ_STATE_ENTERING_ROOM
STATE_IN_ROOM = GGZ_STATE_IN_ROOM
STATE_BETWEEN_ROOMS = GGZ_STATE_BETWEEN_ROOMS
STATE_LAUNCHING_TABLE = GGZ_STATE_LAUNCHING_TABLE
STATE_JOINING_TABLE = GGZ_STATE_JOINING_TABLE
STATE_AT_TABLE = GGZ_STATE_AT_TABLE
STATE_LEAVING_TABLE = GGZ_STATE_LEAVING_TABLE
STATE_LOGGING_OUT = GGZ_STATE_LOGGING_OUT


def init(options):
    cdef GGZOptions aoptions
    if isinstance(options, int):
        aoptions.flags = options
    return ggzcore_init(aoptions)

def destroy():
    ggzcore_destroy()

# Forward declaration
cdef class ErrorEventData
cdef class TableLeaveEventData
cdef class ChatEventData
cdef class Server
cdef class Room
cdef class Player
cdef class Table
cdef class GameType
cdef class Conf
cdef class Module
cdef class Game

# callback wrappers
cdef GGZHookReturn hookfunc_wrapper(unsigned int id, void* event_data, 
        void* user_data):
    callback, mydata = <object>user_data
    return callback(id, <object>event_data, mydata)

cdef void destroyfunc_wrapper(void* data):
    callback, mydata = <object>data
    callback(mydata)

cdef class ErrorEventData:
    def __init__(self):
        self.message = None
        self.status = None
    cdef void set_event(self, void *adata):
        cdef GGZErrorEventData *data
        data = <GGZErrorEventData*> adata 
        self.message = <char*>(data.message)
        self.status = <GGZClientReqError>data.status

cdef class ChatEventData:
    def __init__(self):
        self.type = None
        self.sender = None
        self.message = None
    cdef void set_event(self, void *adata):
        cdef GGZChatEventData *data
        data = <GGZChatEventData*>adata
        self.type = <GGZChatType>data.type
        self.sender = <char*>(data.sender)
        self.message = <char*>(data.message)

cdef class TableLeaveEventData:
    def __init__(self):
        self.reason = 0
        self.player = None
    cdef void set_event(self, void *adata):
        cdef GGZTableLeaveEventData *data
        data = <GGZTableLeaveEventData*> adata 
        self.reason = <GGZLeaveType>data.reason
        self.player = <char*>(data.player)

cdef class Server:
    cdef GGZServer* _server
    def __new__(self, *unused, **unused_kw):
        # We are only called once here.
        self._server = NULL
        self._server = ggzcore_server_new()
    def __init__(self, *unused, **unused_kw):
        self._callbacks = {}
    def __dealloc__(self):
        if self._server:
            ggzcore_server_free(self._server)
    def free(self):
        if self._server:
            ggzcore_server_free(self._server)
    def reset(self):
        return ggzcore_server_reset(self._server)
    def add_event_hook(self, event, callback, data=None):
        '''Handle reference counts?'''
        calldata = (callback, data)
        id = ggzcore_server_add_event_hook_full(self._server, event, 
                hookfunc_wrapper, <void*>calldata)
        self._callbacks[id] = callback # Save a reference for callback
        return id
    def remove_event_hook(self, event, hook):
        # We cannot distinguish functions. :(
        if isinstance(hook, int):
            retval = ggzcore_server_remove_event_hook_id(self._server, event, hook)
            if hook in self._callbacks:
                del self._callbacks[hook]
        return retval
    def set_hostinfo(self, host, port, use_tls):
        return ggzcore_server_set_hostinfo(self._server, host, port, use_tls)
    def set_logininfo(self, type, handle, password):
        return ggzcore_server_set_logininfo(self._server, type, handle, password)
    def log_session(self, filename):
        return ggzcore_server_log_session(self._server, filename)
    def get_host(self):
        return ggzcore_server_get_host(self._server)
    def get_port(self):
        return ggzcore_server_get_port(self._server)
    def get_type(self):
        return ggzcore_server_get_type(self._server)
    def get_handle(self):
        return ggzcore_server_get_handle(self._server)
    def get_password(self):
        return ggzcore_server_get_password(self._server)
    def get_fd(self):
        return ggzcore_server_get_fd(self._server)
    def get_channel(self):
        return ggzcore_server_get_channel(self._server)
    def get_stat(self):
        return ggzcore_server_get_state(self._server)
    def get_tls(self):
        return ggzcore_server_get_tls(self._server)
    def get_num_rooms(self):
        return ggzcore_server_get_num_rooms(self._server)
    def get_cur_room(self):
        cdef GGZRoom *aroom
        cdef Room pyroom
        aroom = ggzcore_server_get_cur_room(self._server)
        pyroom = Room(None)
        pyroom.set_room(aroom)
        return pyroom
    def get_nth_room(self, num):
        cdef GGZRoom *aroom
        cdef Room pyroom
        aroom = ggzcore_server_get_nth_room(self._server, num)
        pyroom = Room(None)
        pyroom.set_room(aroom)
        return pyroom
    def get_num_gametypes(self):
        return ggzcore_server_get_num_gametypes(self._server)
    def get_nth_gametype(self):
        cdef GGZGameType *atype
        cdef GameType pygtype
        atype = ggzcore_server_get_nth_gametype(self._server, num)
        pygtype = GameType()
        pygtype.set_type(atype)
        return pygtype
    def get_cur_game(self):
        cdef GGZGame *agame
        cdef Game pygame
        agame = ggzcore_server_get_cur_game(self._server)
        pygame = Game(None)
        pygame.set_game(agame)
        return pygame
    def is_online(self):
        return ggzcore_server_is_online(self._server)
    def is_logged_in(self):
        return ggzcore_server_is_logged_in(self._server)
    def is_in_room(self):
        return ggzcore_server_is_in_room(self._server)
    def is_at_table(self):
        return ggzcore_server_is_at_table(self._server)
    def connect(self):
        return ggzcore_server_connect(self._server)
    def create_channel(self):
        return ggzcore_server_create_channel(self._server)
    def login(self):
        return ggzcore_server_login(self._server)
    def motd(self):
        return ggzcore_server_motd(self._server)
    def list_rooms(self, type, verbose):
        return ggzcore_server_list_rooms(self._server, type, verbose)
    def list_gametypes(self, verbose):
        return ggzcore_server_list_gametypes(self._server, verbose)
    def join_room(self, room):
        return ggzcore_server_join_room(self._server, room)
    def logout(self):
        return ggzcore_server_logout(self._server)
    def disconnect(self):
        return ggzcore_server_disconnect(self._server)
    def data_is_pending(self):
        return ggzcore_server_data_is_pending(self._server)
    def read_data(self, fd):
        return ggzcore_server_read_data(self._server, fd)

cdef class Room:
    cdef GGZRoom* _room
    def __new__(self, Server server, id=None, 
            name=None, game=None, desc=None):
        self._room = NULL
        if server:
            self._room = ggzcore_room_new()
            ggzcore_room_init(self._room, server._server, id, name, game, desc);
    def __init__(self, *unused, **unused_kw):
        self._callbacks = {}
    def __dealloc__(self):
        if self._room:
            ggzcore_room_free(self._room)

    def free(self):
        if self._room:
            ggzcore_room_free(self._room)

    cdef void set_room(self, GGZRoom* aroom):
        self.free()
        self._room = aroom

    def get_name(self):
        return ggzcore_room_get_name(self._room)
    def get_desc(self):
        return ggzcore_room_get_desc(self._room)
    def get_gametype(self):
        cdef GGZGameType *atype
        cdef GameType pygtype
        atype = ggzcore_room_get_gametype(self._room)
        pygtype = GameType()
        pygtype.set_type(atype)
        return pygtype
    def get_num_players(self):
        return ggzcore_room_get_num_players(self._room)
    def get_nth_player(self, num):
        cdef GGZPlayer *aplayer
        cdef Player pyplayer
        aplayer = ggzcore_room_get_nth_player(self._room, num)
        pyplayer = Player()
        pyplayer.set_player(aplayer)
        return pyplayer
    def get_num_tables(self):
        return ggzcore_room_get_num_tables(self._room)
    def get_nth_table(self, num):
        cdef GGZTable *atable
        cdef Table pytable
        atable = ggzcore_room_get_nth_table(self._room, num)
        pytable = Table(None)
        pytable.set_table(atable)
        return pytable
    def get_table_by_id(self, id):
        cdef GGZTable *atable
        cdef Table pytable
        atable = ggzcore_room_get_table_by_id(self._room, id)
        pytable = Table(None)
        pytable.set_table(atable)
        return pytable

    def add_event_hook(self, event, callback, data=None):
        '''Handle reference counts?'''
        calldata = (callback, data)
        id = ggzcore_room_add_event_hook_full(self._room, event, 
                hookfunc_wrapper, <void*>calldata)
        self._callbacks[id] = callback # Save a reference for callback
        return id
    def remove_event_hook(self, event, hook):
        # We cannot distinguish functions. :(
        if isinstance(hook, int):
            retval = ggzcore_room_remove_event_hook_id(self._room, event, hook)
            if hook in self._callbacks:
                del self._callbacks[hook]
        return retval
    def list_players(self):
        return ggzcore_room_list_players(self._room)
    def list_tables(self, type, cglobal):
        return ggzcore_room_list_tables(self._room, type, cglobal)
    def chat(self, opcode, player, msg):
        return ggzcore_room_chat(self._room, opcode, player, msg)
    def launch_table(self, Table table):
        cdef GGZTable *atable
        atable = table._table
        return ggzcore_room_launch_table(self._room, atable)
    def join_table(self, table_id, spectator):
        return ggzcore_room_join_table(self._room, table_id, spectator)
    def leave_table(self, force):
        return ggzcore_room_leave_table(self._room, force)

cdef class Player:
    cdef GGZPlayer *_player
    def __init__(self):
        self._player = NULL
    cdef void set_player(self, GGZPlayer *aplayer):
        self._player = aplayer
    def get_name(self):
        return ggzcore_player_get_name(self._player)
    def get_type(self):
        return ggzcore_player_get_type(self._player)
    def get_table(self):
        cdef GGZTable *atable
        cdef Table pytable
        atable = ggzcore_player_get_table(self._player)
        pytable = Table(None)
        pytable.set_table(atable)
        return pytable
    def get_lag(self):
        return ggzcore_player_get_lag(self._player)
    def get_record(self):
        cdef int wins, loses, ties, forfeits, retval
        retval =  ggzcore_player_get_record(self._player, &wins, &loses, &ties, 
                &forfeits)
        if retval:
            return wins, loses, ties, forfeits
        else:
            return retval
    def get_rating(self):
        cdef int rating, retval
        retval = ggzcore_player_get_rating(self._player, &rating)
        if retval:
            return rating
        else:
            return retval
    def get_ranking(self):
        cdef int ranking, retval
        retval = ggzcore_player_get_ranking(self._player, &ranking)
        if retval:
            return ranking
        else:
            return retval
    def get_highscore(self):
        cdef long highscore
        cdef int retval
        retval = ggzcore_player_get_highscore(self._player, &highscore)
        if retval:
            return highscore
        else:
            return retval

cdef class Table:
    cdef GGZTable *_table
    def __new__(self, GameType gametype, desc=None, num_seats=None):
        self._table = NULL
        if gametype:
            self._table = ggzcore_table_new()
            ggzcore_table_init(self._table, gametype._type, desc, num_seats)
    def __init__(self, *unused, **unused_kw):
        pass
    def __dealloc__(self):
        if self._table:
            ggzcore_table_free(self._table)
    def free(self):
        if self._table:
            ggzcore_table_free(self._table)
    cdef void set_table(self, GGZTable *atable):
        if self._table != NULL:
            self.free()
        self._table = atable
    def set_seat(self, seat, type, name):
        return ggzcore_table_set_seat(self._table, seat, type, name)
    def remove_player(self, name):
        return ggzcore_table_remove_player(self._table, name)
    def get_id(self):
        return ggzcore_table_get_id(self._table)
    def get_type(self):
        cdef GGZGameType *atype
        cdef GameType pygtype
        atype = ggzcore_table_get_type(self._table)
        pygtype = GameType()
        pygtype.set_type(atype)
        return pygtype
    def get_desc(self):
        return ggzcore_table_get_desc(self._table)
    def get_state(self):
        return ggzcore_table_get_state(self._table)
    def get_num_seats(self):
        return ggzcore_table_get_num_seats(self._table)
    def set_desc(self, desc):
        return ggzcore_table_set_desc(self._table, desc)
    def get_seat_count(self, type):
        return ggzcore_table_get_seat_count(self._table, type)
    def get_nth_player_name(self, num):
        return ggzcore_table_get_nth_player_name(self._table, num)
    def get_num_spectator_seats(self):
        return ggzcore_table_get_num_spectator_seats(self._table)
    def get_nth_spectator_name(self, num):
        return ggzcore_table_get_nth_spectator_name(self._table, num)
    def get_nth_player_type(self, num):
        return ggzcore_table_get_nth_player_type(self._table, num)

cdef class GameType:
    cdef GGZGameType *_type
    def __init__(self):
        self._type = NULL
    cdef void set_type(self, GGZGameType *atype):
        self._type = atype
    def get_name(self):
        return ggzcore_gametype_get_name(self._type)
    def get_prot_engine(self):
        return ggzcore_gametype_get_prot_engine(self._type)
    def get_prot_version(self):
        return ggzcore_gametype_get_prot_version(self._type)
    def get_version(self):
        return ggzcore_gametype_get_version(self._type)
    def get_author(self):
        return ggzcore_gametype_get_author(self._type)
    def get_url(self):
        return ggzcore_gametype_get_url(self._type)
    def get_desc(self):
        return ggzcore_gametype_get_desc(self._type)
    def get_max_players(self):
        return ggzcore_gametype_get_max_players(self._type)
    def get_max_bots(self):
        return ggzcore_gametype_get_max_bots(self._type)
    def get_spectors_allowed(self):
        return ggzcore_gametype_get_spectators_allowed(self._type)
    def num_players_is_valid(self, num):
        return ggzcore_gametype_num_players_is_valid(self._type, num)
    def num_bots_is_valid(self, num):
        return ggzcore_gametype_num_bots_is_valid(self._type, num)

cdef class Conf:
    # FIXME: Should most of the methods be static?
    def __init__(self, g_path, u_path):
        ggzcore_conf_initialize(g_path, u_path)
    def write(self, section, key, value):
        cdef char **argv
        if isinstance(value, str):
            ggzcore_conf_write_string(section, key, value)
        elif isinstance(self, int):
            ggzcore_conf_write_int(section, key, value)
        elif isinstance(self, list):
            tmpargv = PyMem_Malloc(len(list))
            argv[0] = tmpargv
            for i from 0 <= i < len(list):
                tmpi = PyString_AsString(list[i])
                argv[i] = tmpi
            ggzcore_conf_write_list(section, key, len(list), argv)
            for i from 0 <= i < len(list):
                PyMem_Free(argv[i])
            PyMem_Free(tmpargv)
        else:
            # bad!
            pass
    def read_string(self, section, key, locdef):
        return ggzcore_conf_read_string(section, key, locdef)
    def read_int(self, section, key, locdef):
        return ggzcore_conf_read_int(section, key, locdef)
    def read_list(self, section, key):
        cdef int argcp
        cdef char** argvp
        ggzcore_conf_read_list(section, key, &argcp, &argvp)
        alist = []
        # FIXME: Do I need to free argv?
        for i from 0 <= i < argcp:
            alist.append(PyString_FromString(argv[i]))
            PyMem_Free(argv[i])
        PyMem_Free(argv)
        return alist
    def remove_section(self, section):
        return ggzcore_conf_remove_section(section)
    def remove_key(section, key):
        return ggzcore_conf_remove_key(section, key)
    def commit(self):
        return ggzcore_conf_commit()

cdef class Module:
    # FIXME: Should some of the methods be static?
    cdef GGZModule *_module
    def __init__(self):
        self._module = NULL
    cdef void set_module(self, GGZModule *amodule):
        self._module = amodule

    def get_num(self):
        return ggzcore_module_get_num()
    def add(name, version, prot_engine, prot_version, author, frontend, 
            url, exe_path, icon_path, help_path):
        return ggzcore_module_add(name, version, prot_engine, prot_version,
                author, frontend, url, exe_path, icon_path, help_path)
    def get_num_by_type(self, game, engine, version):
        return ggzcore_module_get_num_by_type(game, engine, version)
    def get_nth_by_type(self, name, engine, version, num):
        cdef GGZModule *amodule
        cdef Module pymodule
        amodule = ggzcore_module_get_nth_by_type(name, engine, version, num)
        pymodule = Module()
        pymodule.set_module(amodule)
        return pymodule

    def launch(self, module):
        ggzcore_module_launch(self._module)
    def get_name(self):
        return ggzcore_module_get_name(self._module)
    def get_version(self):
        return ggzcore_module_get_version(self._module)
    def get_prot_engine(self):
        return ggzcore_module_get_prot_engine(self._module)
    def get_prot_version(self):
        return ggzcore_module_get_prot_version(self._module)
    def get_author(self):
        return ggzcore_module_get_author(self._module)
    def get_frontend(self):
        return ggzcore_module_get_frontend(self._module)
    def get_url(self):
        return ggzcore_module_get_url(self._module)
    def get_icon_path(self):
        return ggzcore_module_get_icon_path(self._module)
    def get_help_path(self):
        return ggzcore_module_get_help_path(self._module)
    def get_argv(self):
        cdef char **argv
        argv = ggzcore_module_get_argv(self._module)
        alist = []
        # FIXME: How to create the list, no length?

cdef class Game:
    cdef GGZGame *_game
    def __new__(self, Server aserver, Module amodule):
        self._game = NULL
        if aserver:
            self._game = ggzcore_game_new()
            ggzcore_game_init(self._game, aserver._server, amodule._module)
    def __init__(self, *unused, **unused_kw):
        self._callbacks = {}
    def __dealloc__(self):
        if self._game:
            ggzcore_game_free(self._game)
    def free(self):
        if self._game:
            ggzcore_game_free(self._game)
    cdef void set_game(self, GGZGame *agame):
        if self._game:
            self.free()
        self._game = agame
    def add_event_hook(self, event, callback, data=None):
        '''Handle reference counts?'''
        calldata = (callback, data)
        id = ggzcore_game_add_event_hook_full(self._game, event, 
                hookfunc_wrapper, <void*>calldata)
        self._callbacks[id] = callback # Save a reference for callback
        return id
    def remove_event_hook(self, event, hook):
        # We cannot distinguish functions. :(
        if isinstance(hook, int):
            retval = ggzcore_game_remove_event_hook_id(self._game, event, hook)
            if hook in self._callbacks:
                del self._callbacks[hook]
        return retval
    def get_control_fd(self):
        return ggzcore_game_get_control_fd(self._game)
    def set_server_fd(self, fd):
        ggzcore_game_set_server_fd(self._game, fd)
    def get_module(self):
        cdef GGZModule *amodule
        cdef Module pymodule
        amodule = ggzcore_game_get_module(self._game)
        pymodule = Module()
        pymodule.set_module(amodule)
        return pymodule
    def launch(self):
        return ggzcore_game_launch(self._game)
    def read_data(self):
        return ggzcore_game_read_data(self._game)

# vim:ts=8:sw=4:expandtab
