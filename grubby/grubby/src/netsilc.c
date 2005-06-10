/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 - 2005 Josef Spillner <josef@ggzgamingzone.org>
* Original written by Rich Gade and enhanced by Justin Zaun
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "net.h"
#include <silcincludes.h>
#include <silcclient.h>

/* Prototypes */
static void silc_say(SilcClient client, SilcClientConnection conn, SilcClientMessageType type, char *msg, ...);
static void silc_channel_message(SilcClient client, SilcClientConnection conn, SilcClientEntry sender,
	SilcChannelEntry channel, SilcMessagePayload payload, SilcChannelPrivateKey key, SilcMessageFlags flags,
	const unsigned char *message, SilcUInt32 message_len);
static void silc_private_message(SilcClient client, SilcClientConnection conn, SilcClientEntry sender,
	SilcMessagePayload payload, SilcMessageFlags flags, const unsigned char *message, SilcUInt32 message_len);
static void silc_notify(SilcClient client, SilcClientConnection conn, SilcNotifyType type, ...);
static void silc_connected(SilcClient client, SilcClientConnection conn, SilcClientConnectionStatus status);
static void silc_disconnected(SilcClient client, SilcClientConnection conn, SilcStatus status, const char *message);
static void silc_failure(SilcClient client, SilcClientConnection conn, SilcProtocol protocol, void *failure);

/* Unused prototypes */
static void no_silc_command(SilcClient client, SilcClientConnection conn, SilcClientCommandContext cmd_context,
	bool success, SilcCommand command, SilcStatus status);
static void no_silc_command_reply(SilcClient client, SilcClientConnection conn, SilcCommandPayload cmd_payload,
	bool success, SilcCommand command, SilcStatus status, ...);
static void no_silc_get_auth_method(SilcClient client, SilcClientConnection conn, char *hostname, SilcUInt16 port,
	SilcGetAuthMeth completion, void *context);
static void no_silc_verify_public_key(SilcClient client, SilcClientConnection conn, SilcSocketType conn_type,
	unsigned char *pk, SilcUInt32 pk_len, SilcSKEPKType pk_type, SilcVerifyPublicKey completion, void *context);
static void no_silc_ask_passphrase(SilcClient client, SilcClientConnection conn, SilcAskPassphrase completion,
	void *context);
static bool no_silc_key_agreement(SilcClient client, SilcClientConnection conn, SilcClientEntry client_entry,
	const char *hostname, SilcUInt16 port, SilcKeyAgreementCallback *completion, void **context);
static void no_silc_ftp(SilcClient client, SilcClientConnection conn, SilcClientEntry client_entry,
	SilcUInt32 session_id, const char *hostname, SilcUInt16 port);
static void no_silc_detach(SilcClient client, SilcClientConnection conn, const unsigned char *detach_data,
	SilcUInt32 detach_data_len);

/* Globals */
int status = NET_NOOP;
Guru **queue = NULL;
int queuelen = 1;
char *guruname = NULL;
char *gurupassword = NULL;
FILE *logstream = NULL;
/*char *playername;*/

static SilcClient client;
static SilcClientConnection connection;
static SilcPKCS pkcs;
static SilcPublicKey pubkey;
static SilcPrivateKey privkey;
static SilcChannelEntry lastchannel;

static SilcClientOperations ops =
{
	silc_say,
	silc_channel_message,
	silc_private_message,
	silc_notify,
	no_silc_command,
	no_silc_command_reply,
	silc_connected,
	silc_disconnected,
	no_silc_get_auth_method,
	no_silc_verify_public_key,
	no_silc_ask_passphrase,
	silc_failure,
	no_silc_key_agreement,
	no_silc_ftp,
	no_silc_detach
};

/* Set up the logfile or close it again */
void net_logfile(const char *logfile)
{
	if(logfile)
	{
		if(!logstream) logstream = fopen(logfile, "a");
	}
	else
	{
		if(logstream)
		{
			fclose(logstream);
			logstream = NULL;
		}
	}
}

/* Add a message to the incoming queue */
/* FIXME: Provide real queue */
static void net_internal_queueadd(const char *player, const char *message, int type)
{
	Guru *guru;
	char *listtoken;
	char *token;
	int i;
	char realmessage[1024];

	/* Add dummy field on dummy messages */
	if((message) && (type == GURU_PRIVMSG))
	{
		sprintf(realmessage, "%s %s", guruname, message);
		message = realmessage;
	}

	/* Insert new grubby structure */
	guru = (Guru*)malloc(sizeof(Guru));
	guru->type = type;
	if(player) guru->player = strdup(player);
	else guru->player = NULL;
	if(message)
	{
		guru->message = strdup(message);
		guru->list = NULL;
		listtoken = strdup(message);
		token = strtok(listtoken, " ,./:?!\'");
		i = 0;
		while(token)
		{
			guru->list = (char**)realloc(guru->list, (i + 2) * sizeof(char*));
			guru->list[i] = (char*)malloc(strlen(token) + 1);
			strcpy(guru->list[i], token);
			guru->list[i + 1] = NULL;
			i++;
			token = strtok(NULL, " ,./:?!\'");
		}
		free(listtoken);
	}
	else
	{
		guru->message = NULL;
		guru->list = NULL;
	}

	/* Recognize direct speech */
	if((guru->type == GURU_CHAT) && (guru->list) && (guru->list[0]))
		if(!strcasecmp(guru->list[0], guruname))
			guru->type = GURU_DIRECT;

	/* Insert structure into queue */
	queuelen++;
	queue = (Guru**)realloc(queue, sizeof(Guru*) * queuelen);
	queue[queuelen - 2] = guru;
	queue[queuelen - 1] = NULL;
}

/* Connect to a SILC server */
void net_connect(const char *host, int port, const char *name, const char *password)
{
/*	SilcClientParams params;*/
	int ret;
/*	char *algorithms;
	char *algorithm;*/
	
	guruname = strdup(name);
	if(password) gurupassword = strdup(password);
	else gurupassword = NULL;

/*
	params.task_max = 0;
	params.rekey_secs = 0;
	params.connauth_request_secs = 0;
	strcpy(params.nickname_format, strdup("%n"));
	params.nickname_force_format = FALSE;
	params.nickname_parse = NULL;
	params.ignore_requested_attributes = TRUE;
*/

	/* XXX */
	client = silc_client_alloc(&ops, /*&params*/NULL, NULL, silc_version_string);

	client->username = strdup(name);
	client->hostname = strdup(host);
	client->realname = strdup("Grubby the GGZ chatbot");

	/*algorithms = silc_pkcs_get_supported();
	fprintf(stderr, "(SILC) >> silc_pkcs_get_supported returns %s\n", algorithms);
	algorithm = strtok(algorithms, ",");
	while(algorithm)
	{
		fprintf(stderr, "(SILC) ALGO: %s\n", algorithm);
		algorithm = strtok(NULL, ",");
	}
	ret = silc_pkcs_alloc("grubbypkcs", &pkcs);
	fprintf(stderr, "(SILC) >> silc_pkcs_alloc returns %i\n", ret);
	if(!ret) status = NET_ERROR;*/

	ret = silc_pkcs_register_default();
	fprintf(stderr, "(SILC) >> silc_pkcs_register_default returns %i\n", ret);
	if(!ret) status = NET_ERROR;

	ret = silc_hash_register_default();
	fprintf(stderr, "(SILC) >> silc_hash_register_default returns %i\n", ret);
	if(!ret) status = NET_ERROR;

	ret = silc_cipher_register_default();
	fprintf(stderr, "(SILC) >> silc_cipher_register_default returns %i\n", ret);
	if(!ret) status = NET_ERROR;

	ret = silc_hmac_register_default();
	fprintf(stderr, "(SILC) >> silc_hmac_register_default returns %i\n", ret);
	if(!ret) status = NET_ERROR;

	ret = silc_create_key_pair(NULL, 0, NULL, NULL, NULL, NULL, &pkcs, &pubkey, &privkey, FALSE);
	fprintf(stderr, "(SILC) >> silc_create_key_pair returns %i\n", ret);
	if(!ret) status = NET_ERROR;

	/*ret = silc_pkcs_alloc("rsa", &pkcs);
	fprintf(stderr, "(SILC) >> silc_pkcs_alloc returns %i\n", ret);
	if(!ret) status = NET_ERROR;*/

	client->pkcs = pkcs;
	client->public_key = pubkey;
	client->private_key = privkey;

	ret = silc_client_init(client);
	fprintf(stderr, "(SILC) >> silc_client_init returns %i\n", ret);
	if(!ret) status = NET_ERROR;

	silc_client_connect_to_server(client, NULL, 706, strdup(host), NULL);

	/*status = NET_GOTREADY;*/
}

/* Join a SILC channel */
void net_join(const char *room)
{
	fprintf(stderr, "Join room: %s\n", room);
	silc_client_command_call(client, connection, NULL, "JOIN", room, NULL);
}

/* Loop function */
int net_status()
{
	int ret;

    /* XXX -> net_output(...) */
	silc_client_run_one(client);

	ret = status;
	if(status == NET_GOTREADY) status = NET_NOOP;
	if(status == NET_LOGIN) status = NET_NOOP;
	if(status == NET_INPUT) status = NET_NOOP;
	return ret;
}

/* Get an entry from the queue */
Guru *net_input()
{
	if(queue)
	{
		return queue[queuelen - 2];
	}
	return NULL;
}

/* Let grubby speak */
void net_output(Guru *output)
{
	char *token;
	char *msg;

	/* Handle multi-line answers */
	if(!output->message) return;
	msg = strdup(output->message);
	token = strtok(msg, "\n");
	while(token)
	{
		switch(output->type)
		{
			case GURU_CHAT:
				printf("> %s\n", token);
				silc_client_send_channel_message(client, connection,
					lastchannel, NULL, 0, (unsigned char*)token, strlen(token), TRUE);
				break;
			case GURU_PRIVMSG:
				printf("-> %s: %s\n", output->player, token);
				silc_client_send_channel_message(client, connection,
					lastchannel, NULL, 0, (unsigned char*)token, strlen(token), TRUE);
				break;
			case GURU_ADMIN:
				printf(">> %s\n", token);
				break;
		}
		token = strtok(NULL, "\n");
	}
	free(msg);
}

/*
static void chat(const char *message)
{
	time_t t;
	char *ts;

	net_internal_queueadd(playername, message, GURU_PRIVMSG);
	status = NET_INPUT;

	if(logstream)
	{
		t = time(NULL);
		ts = ctime(&t);
		ts[strlen(ts) - 1] = 0;
		fprintf(logstream, "%s (%s) [%s]: %s\n", ts, "-", playername, message);
		fflush(logstream);
	}
}
*/

static void silc_say(SilcClient client, SilcClientConnection conn, SilcClientMessageType type, char *msg, ...)
{
	char *typestr;

	fprintf(stderr, "(SILC) say\n");

	typestr = NULL;
	if(type == SILC_CLIENT_MESSAGE_INFO) typestr = "info";
	else if(type == SILC_CLIENT_MESSAGE_WARNING) typestr = "warning";
	else if(type == SILC_CLIENT_MESSAGE_ERROR) typestr = "error";
	else if(type == SILC_CLIENT_MESSAGE_AUDIT) typestr = "audit";

	fprintf(stderr, "Say message type: %s\n", typestr);
	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

static void silc_channel_message(SilcClient client, SilcClientConnection conn, SilcClientEntry sender,
	SilcChannelEntry channel, SilcMessagePayload payload, SilcChannelPrivateKey key, SilcMessageFlags flags,
	const unsigned char *message, SilcUInt32 message_len)
{
	fprintf(stderr, "(SILC) channel message\n");

	fprintf(stderr, "Message sender: %s\n", sender->nickname);
	fprintf(stderr, "Message: %s\n", message);

	lastchannel = channel;
	net_internal_queueadd(sender->nickname, (char*)message, GURU_CHAT);
	status = NET_INPUT;
}

static void silc_private_message(SilcClient client, SilcClientConnection conn, SilcClientEntry sender,
	SilcMessagePayload payload, SilcMessageFlags flags, const unsigned char *message, SilcUInt32 message_len)
{
	fprintf(stderr, "(SILC) private message\n");
}

static void silc_notify(SilcClient client, SilcClientConnection conn, SilcNotifyType type, ...)
{
	char *typestr;
	char *str;

	fprintf(stderr, "(SILC) notify\n");

	typestr = NULL;
	if(type == SILC_NOTIFY_TYPE_INVITE) typestr = "invite";
	else if(type == SILC_NOTIFY_TYPE_JOIN) typestr = "join";
	else if(type == SILC_NOTIFY_TYPE_LEAVE) typestr = "leave";
	else if(type == SILC_NOTIFY_TYPE_SIGNOFF) typestr = "signoff";
	else if(type == SILC_NOTIFY_TYPE_TOPIC_SET) typestr = "topic set";
	else if(type == SILC_NOTIFY_TYPE_NICK_CHANGE) typestr = "nick change";
	else if(type == SILC_NOTIFY_TYPE_MOTD) typestr = "motd";
	else if(type == SILC_NOTIFY_TYPE_KICKED) typestr = "kicked";
	else if(type == SILC_NOTIFY_TYPE_KILLED) typestr = "killed";

	fprintf(stderr, "Notification type: %s\n", typestr);

	va_list ap;
	va_start(ap, type);

	if(type == SILC_NOTIFY_TYPE_MOTD)
	{
		str = va_arg(ap, char*);
		fprintf(stderr, "Notify MOTD: %s\n", str);

		status = NET_LOGIN;
	}
	else if(type == SILC_NOTIFY_TYPE_JOIN)
	{
		status = NET_GOTREADY;
	}

	va_end(ap);
}

static void silc_connected(SilcClient client, SilcClientConnection conn, SilcClientConnectionStatus status)
{
	char *statusstr;

	fprintf(stderr, "(SILC) connected\n");

	statusstr = NULL;
	if(status == SILC_CLIENT_CONN_SUCCESS) statusstr = "success";
	else if(status == SILC_CLIENT_CONN_ERROR) statusstr = "unknown error";
	else if(status == SILC_CLIENT_CONN_ERROR_TIMEOUT) statusstr = "timeout error";
	else if(status == SILC_CLIENT_CONN_ERROR_AUTH) statusstr = "auth error";
	else if(status == SILC_CLIENT_CONN_ERROR_KE) statusstr = "key exchange error";

	fprintf(stderr, "Connection status: %s\n", statusstr);

	if((status != SILC_CLIENT_CONN_SUCCESS)
	&& (status != SILC_CLIENT_CONN_SUCCESS_RESUME))
	{
		status = NET_ERROR;
		silc_client_close_connection(client, conn);
		return;
	}

	connection = conn;

	/*status = NET_LOGIN;*/
}

static void silc_disconnected(SilcClient client, SilcClientConnection conn, SilcStatus status, const char *message)
{
	fprintf(stderr, "(SILC) disconnected\n");

	status = NET_ERROR;
}

static void silc_failure(SilcClient client, SilcClientConnection conn, SilcProtocol protocol, void *failure)
{
	fprintf(stderr, "(SILC) failure\n");

	status = NET_ERROR;
}

static void no_silc_command(SilcClient client, SilcClientConnection conn, SilcClientCommandContext cmd_context,
	bool success, SilcCommand command, SilcStatus status)
{
	fprintf(stderr, "(SILC) (NO) command\n");
}

static void no_silc_command_reply(SilcClient client, SilcClientConnection conn, SilcCommandPayload cmd_payload,
	bool success, SilcCommand command, SilcStatus status, ...)
{
	fprintf(stderr, "(SILC) (NO) command reply\n");
}

static void no_silc_get_auth_method(SilcClient client, SilcClientConnection conn, char *hostname, SilcUInt16 port,
	SilcGetAuthMeth completion, void *context)
{
	/*InternalGetAuthMethod internal;*/

	fprintf(stderr, "(SILC) (NO) get auth method\n");

	/* ok this we have to do if we also do the thingie below */
	/* otherwise it'll disconnect us*/

	/*internal = silc_calloc(1, sizeof(*internal));
	internal->completion = completion;
	internal->context = context;

	silc_client_request_authentication_method(client, conn,
		silc_get_auth_method_callback, internal);*/
	
	(completion)(TRUE, SILC_AUTH_NONE, NULL, 0, context);
}

static void no_silc_verify_public_key(SilcClient client, SilcClientConnection conn, SilcSocketType conn_type,
	unsigned char *pk, SilcUInt32 pk_len, SilcSKEPKType pk_type, SilcVerifyPublicKey completion, void *context)
{
	fprintf(stderr, "(SILC) (NO) verify public key\n");

	(completion)(TRUE, context); /* not essential but speeds things up it seems :) */
}

static void no_silc_ask_passphrase(SilcClient client, SilcClientConnection conn, SilcAskPassphrase completion,
	void *context)
{
	fprintf(stderr, "(SILC) (NO) ask passphrase\n");
}

static bool no_silc_key_agreement(SilcClient client, SilcClientConnection conn, SilcClientEntry client_entry,
	const char *hostname, SilcUInt16 port, SilcKeyAgreementCallback *completion, void **context)
{
	fprintf(stderr, "(SILC) (NO) key agreement\n");

	return TRUE;
}

static void no_silc_ftp(SilcClient client, SilcClientConnection conn, SilcClientEntry client_entry,
	SilcUInt32 session_id, const char *hostname, SilcUInt16 port)
{
	fprintf(stderr, "(SILC) (NO) ftp\n");
}

static void no_silc_detach(SilcClient client, SilcClientConnection conn, const unsigned char *detach_data,
	SilcUInt32 detach_data_len)
{
	fprintf(stderr, "(SILC) (NO) detach\n");
}

