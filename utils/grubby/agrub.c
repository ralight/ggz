/* agrub.c
 *
 * A GRUB is A Ggz Really Useful Bot
 *
 * Rich Gade
 * April 8, 2000
 */

#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <popt.h>
#include <signal.h>
#include <easysock.h>
#include <protocols.h>

/* ~/.agrub version */
#define FILE_VERSION "0.0.3"

#define DEFAULT_PORT 5687
#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_NAME "Grubby"
#define DEFAULT_OWNER "(none)"

void handle_read(void);
void req_login(void);
void change_room(void);
void handle_chat(char *, char *);
void handle_command(char *, char *);
void send_chat(char *);
void send_chat_insert_name(char *);
void do_logout(int);
void save_known();
void do_greet(char *);
void store_aka(char *, char *);
void store_email(char *, char *);
void store_url(char *, char *);
void store_info(char *, char *);
void store_bday(char *, char *);
void check_seen(char *, char *);
void whois(char *sender, char *name);
void queue_message(char *, char *);
void graceless_exit(int);
void log_on(char *sender, char *command);
void log_off(char *sender, char *command);
log_it(char *left, char *right);

int my_socket, rooms=0, cur_room=-1, logged_in=0;
char **room_list;
char out_msg[1024];
int player_greet = 0;
int last_used = 0;
int sig_logout = 0;
FILE *log_file;
int stay_still = 0;

#define MAX_KNOWN 50
#define MAX_MESSAGES 4

typedef struct {
	char *msg_from;
	char *msg;
} smsg;

struct {
	char *name;
	char *aka;
	char *email;
	char *url;
	char *info;
	char *bday;
	time_t last_seen;
	char *last_room;
	int msg_count;
	smsg msg[MAX_MESSAGES];
} known[MAX_KNOWN];
int num_known = 0;

float total_greating=6.0;
char *great_strings[] = {
	"Hey, Howsit?",
	"Glad to see ya again!",
	"Hello there.",
	"Welcome to the room!",
	"Howsit goin?",
	"Hello, whats up?",
	"Hey, ready for a good game or two?"
};
char *help_strings[] = {
	"I am aware of the following commands:",
	"    %s: About",
	"    %s: Help",
	"    %s: Go away",
	"    %s: My name is <name>",
	"    %s: My email is <email>",
	"    %s: My url is <URL>",
	"    %s: My info is <info about yourself>",
	"    %s: My birthday is <MM-DD-YYYY>",
	"    %s: Have you seen <name>",
	"    %s: Tell <name> <A message>",
	"    %s: Whois <name>"
} ;
char *about_strings[] = {
	"Hi, I'm %s, and I'm A GRUB.",
	" ",
	"A GRUB is A Ggz Really Useful Bot.  I can",
	"remember all sorts of things, and as time",
	"goes on I will be able to learn even more!",
	" ",
	"I was coded by Rich Gade, and if you have",
	"problems with me, be sure to inform him!"
} ;

int port = -1;
char *host = NULL;
char *bot_name = NULL;
char *owner = NULL;

struct poptOption args[] = {
	{"port", 'p', POPT_ARG_INT, &port, 0, "Port to connect to", "PORT"},
	{"host", 'h', POPT_ARG_STRING, &host, 0, "Host to connect to", "HOST"},
	{"name", 'n', POPT_ARG_STRING, &bot_name, 0, "Username for GGZ","NAME"},
	{"owner", 'o', POPT_ARG_STRING, &owner, 0,"Username for owner","OWNER"},
	POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}
};


int main(const int argc, const char *argv[])
{
	int opcode, i, j;
	fd_set select_mux;
	struct timeval timeout;
	poptContext context;
	int rc, num;
	char tmp[1024], *home;
	char file[1024];
	char ver[1024];
	FILE *grub_file;

	for(i=0; i<MAX_KNOWN; i++)
		known[i].name
		  = known[i].aka
		  = known[i].email
		  = known[i].url
		  = known[i].info
		    = NULL;

	known[i].msg_count=0;

	context = poptGetContext(NULL, argc, argv, args, 0);
	while((rc = poptGetNextOpt(context)) != -1) {
		switch(rc) {
			case POPT_ERROR_NOARG:
			case POPT_ERROR_BADOPT:
			case POPT_ERROR_BADNUMBER:
			case POPT_ERROR_OVERFLOW:
				printf("%s: %s\n",
					poptBadOption(context, 0),
					poptStrerror(rc));
				poptFreeContext(context);
				exit(1);
				break;
		}
	}
	poptFreeContext(context);

	if(port == -1)
		port = DEFAULT_PORT;
	if(host == NULL)
		host = DEFAULT_HOSTNAME;
	if(bot_name == NULL)
		bot_name = DEFAULT_NAME;
	if(owner == NULL)
		owner = DEFAULT_OWNER;

	srandom((unsigned)time(NULL));

	/* Read in known informaion */
	home = getenv("HOME");
	if (home == NULL)
	{
		printf("$HOME environment variable not defined!\n\n");
	}
	
	strcpy(file, home);
	strcat(file,"/.agrub");

	grub_file = fopen(file, "r");
	if (grub_file == NULL)
	{
		printf("Error loading known information\n\n");
	} else {
		printf("Loading known information\n");
		/* Load up known information */
		fgets(ver, 1024, grub_file);
		printf(">>> Version: %s\n",ver);
		fgets(tmp, 1024, grub_file);
		num_known = atoi(tmp);
		printf(">>> Entries: %d\n",num_known);
		for(j=0;j<num_known;j++)
		{
			fgets(tmp, 1024, grub_file);
			tmp[strlen(tmp)-1] = '\0';
			known[j].name = malloc(strlen(tmp));
			strcpy(known[j].name, tmp);
			if(!strcmp(known[j].name,"NULL"))
				known[j].name = NULL;
			printf(">>> %s ",known[j].name);

			fgets(tmp, 1024, grub_file);
			tmp[strlen(tmp)-1] = '\0';
			known[j].aka = malloc(strlen(tmp));
			strcpy(known[j].aka, tmp);
			if(!strcmp(known[j].aka,"NULL"))
				known[j].aka = NULL;
			printf("- %s",known[j].aka);

			fgets(tmp, 1024, grub_file);
			tmp[strlen(tmp)-1] = '\0';
			known[j].email = malloc(strlen(tmp));
			strcpy(known[j].email, tmp);
			if(!strcmp(known[j].email,"NULL"))
				known[j].email = NULL;
			printf("- %s",known[j].email);

			fgets(tmp, 1024, grub_file);
			tmp[strlen(tmp)-1] = '\0';
			known[j].url = malloc(strlen(tmp));
			strcpy(known[j].url, tmp);
			if(!strcmp(known[j].url,"NULL"))
				known[j].url = NULL;
			printf("- %s",known[j].url);

			fgets(tmp, 1024, grub_file);
			tmp[strlen(tmp)-1] = '\0';
			known[j].info = malloc(strlen(tmp));
			strcpy(known[j].info, tmp);
			if(!strcmp(known[j].info,"NULL"))
				known[j].info = NULL;
			printf("- %s",known[j].info);

			fgets(tmp, 1024, grub_file);
			tmp[strlen(tmp)-1] = '\0';
			known[j].bday = malloc(strlen(tmp));
			strcpy(known[j].bday, tmp);
			if(!strcmp(known[j].bday,"NULL"))
				known[j].bday = NULL;
			printf("- %s",known[j].bday);

			fgets(tmp, 1024, grub_file);
			known[j].last_seen = atoi (tmp);
			printf("- %d ",known[j].last_seen);

			fgets(tmp, 1024, grub_file);
			tmp[strlen(tmp)-1] = '\0';
			known[j].last_room = malloc(strlen(tmp));
			strcpy(known[j].last_room, tmp);
			if(!strcmp(known[j].last_room,"NULL"))
				known[j].last_room = NULL;
			printf("- %s\n",known[j].last_room);

			fgets(tmp, 1024, grub_file);
			num = atoi(tmp);
			printf(">>> Messages: %d\n",num);
			known[j].msg_count=num;

			for(i=0;i<num;i++)
			{
				fgets(tmp, 1024, grub_file);
				tmp[strlen(tmp)-1] = '\0';
				known[j].msg[i].msg_from = malloc(strlen(tmp));
				strcpy(known[j].msg[i].msg_from, tmp);
				printf("- %s",known[j].msg[i].msg_from);

				fgets(tmp, 1024, grub_file);
				tmp[strlen(tmp)-1] = '\0';
				known[j].msg[i].msg = malloc(strlen(tmp));
				strcpy(known[j].msg[i].msg, tmp);
				printf("- %s\n",known[j].msg[i].msg);
			}
			printf("\n\n");
		}
		fclose(grub_file);
	}

	/* Trap ^C and family */
	signal(SIGINT, graceless_exit);
	signal(SIGQUIT, graceless_exit);
	signal(SIGTERM, graceless_exit);

	/* Connect up to GGZ server */
	my_socket = es_make_socket(ES_CLIENT, port, host);
	if(my_socket < 0) {
		printf("Unable to connect to %s:%d\n", host, port);
		exit(1);
	}

	while(1)
	{
		FD_ZERO(&select_mux);
		FD_SET(my_socket, &select_mux);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		if(select(my_socket+1, &select_mux, NULL, NULL, &timeout)) {
			if(sig_logout)
				do_logout(1);
			else if(FD_ISSET(my_socket, &select_mux))
				handle_read();
		} else {
			if(sig_logout)
				do_logout(1);
			else if(logged_in == 0)
				req_login();
			else if(logged_in == 1)
			{
				change_room();
				logged_in=2;
			} else if(last_used > 480) {
				if(log_file == NULL && stay_still != 1)
					change_room();
				last_used = 0;
				save_known();
			}
		}
		last_used ++;
	}
}


void handle_read(void)
{
	int opcode, i, num, num2, j, num3;
	char *string, *string2;
	char chr;

	es_read_int(my_socket, &opcode);
	switch(opcode) {
		case MSG_SERVER_ID:
			es_read_string_alloc(my_socket, &string);
			free(string);
			break;
		case RSP_LOGIN_ANON:
			es_read_char(my_socket, &chr);
			es_read_int(my_socket, &num);
			if(chr != 0)
			{
				logged_in = 0;
			} else
				logged_in = 1;
			break;
		case MSG_MOTD:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_string_alloc(my_socket, &string);
				free(string);
			}
			break;
		case MSG_UPDATE_ROOMS:
			es_write_int(my_socket, REQ_LIST_ROOMS);
			es_write_int(my_socket, -1);
			es_write_char(my_socket, (char)0);
			break;
		case RSP_LIST_ROOMS:
			es_read_int(my_socket, &num);
			rooms = num;
			room_list = calloc(rooms, sizeof(char *));
			for(i=0; i<num; i++) {
				es_read_int(my_socket, &num2);
				es_read_string_alloc(my_socket, &string);
				room_list[i] = string;
				es_read_int(my_socket, &num2);
			}
			break;
		case RSP_ROOM_JOIN:
			es_read_char(my_socket, &chr);
			player_greet = 1;
			break;
		case MSG_CHAT:
			es_read_string_alloc(my_socket, &string);
			es_read_string_alloc(my_socket, &string2);
			handle_chat(string, string2);
			free(string);
			free(string2);
			break;
		case RSP_CHAT:
			es_read_char(my_socket, &chr);
			break;
		case MSG_UPDATE_PLAYERS:
			es_write_int(my_socket, REQ_LIST_PLAYERS);
			break;
		case MSG_UPDATE_TYPES:
			es_write_int(my_socket, REQ_LIST_TYPES);
			es_write_char(my_socket, 1);
			break;
		case MSG_UPDATE_TABLES:
			es_write_int(my_socket, REQ_LIST_TABLES);
			es_write_int(my_socket, -1);
			es_write_char(my_socket, 0);
			break;
		case RSP_LIST_PLAYERS:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_string_alloc(my_socket, &string);
				if(player_greet)
					do_greet(string);
				else
					free(string);
				es_read_int(my_socket, &num2);
			}
			player_greet = 0;
			break;
		case RSP_LIST_TYPES:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_int(my_socket, &num2);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_char(my_socket, &chr);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_string_alloc(my_socket, &string);
				free(string);
			}
			break;
		case RSP_LIST_TABLES:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_int(my_socket, &num2);
				es_read_int(my_socket, &num2);
				es_read_int(my_socket, &num2);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_char(my_socket, &chr);
				es_read_int(my_socket, &num2);
				for(j=0; j<num2; j++) {
					es_read_int(my_socket, &num3);
					es_read_string_alloc(my_socket,&string);
					free(string);
				}
			}
			break;
		case RSP_LOGOUT:
			es_read_int(my_socket, &num);
			es_read_char(my_socket, &chr);
			exit(0);
			break;
	}
}


void req_login(void)
{
	int i;

	es_write_int(my_socket, REQ_LOGIN_ANON);
	es_write_string(my_socket, bot_name);
}


void change_room(void)
{
	int new_room=cur_room;

	if(rooms == 1 && cur_room == 0)
		return;

	while(new_room == cur_room)
		new_room = random() % rooms;

	es_write_int(my_socket, REQ_ROOM_JOIN);
	es_write_int(my_socket, new_room);
	cur_room = new_room;
}


void handle_chat(char *sender, char *msg)
{
	char *newstr;

	sprintf(out_msg, "%s: ", bot_name);
	if(!strncasecmp(msg, out_msg, strlen(out_msg))) {
		log_it(sender, msg);
		handle_command(sender, msg+strlen(out_msg));
		return;
	}

	sprintf(out_msg, "%s, ", bot_name);
	if(!strncasecmp(msg, out_msg, strlen(out_msg))) {
		log_it(sender, msg);
		handle_command(sender, msg+strlen(out_msg));
		return;
	}

	sprintf(out_msg, "%s ", bot_name);
	if(!strncasecmp(msg, out_msg, strlen(out_msg))) {
		log_it(sender, msg);
		handle_command(sender, msg+strlen(out_msg));
		return;
	}

	if(!strncasecmp(msg, "/me ", 4)) {
		sprintf(out_msg, "%s %s", sender, msg+4);
		log_it("*", out_msg);
		return;
	}else if(!strcmp(msg, "/SjoinS")
	     && strcasecmp(sender, bot_name)) {
		newstr = malloc(strlen(sender)+1);
		strcpy(newstr, sender);
		log_it("-->", newstr);
		do_greet(newstr);
		return;
	}else if(!strcmp(msg, "/SpartS")
	     && strcasecmp(sender, bot_name)) {
		newstr = malloc(strlen(sender)+1);
		strcpy(newstr, sender);
		log_it("<--", newstr);
		return;
	} else {
		log_it (sender,msg);
	}
}


void handle_command(char *sender, char *command)
{
	int i;

	last_used = 0;

	if(!strcasecmp(command, "help")) {
		for(i=0; i<sizeof(help_strings)/sizeof(char *); i++)
			send_chat_insert_name(help_strings[i]);
	} else if(!strcasecmp(command, "about")) {
		for(i=0; i<sizeof(about_strings)/sizeof(char *); i++)
			send_chat_insert_name(about_strings[i]);
	} else if(!strcasecmp(command, "go away")) {
		send_chat("Fine, you don't want me around...");
 		send_chat("Then I don't want to be here!");
		change_room();
	} else if(!strcasecmp(command, "shutdown")) {
		if(!strcmp(sender, owner)) {
			send_chat("NOoooooo... I'm melting....");
			do_logout(0);
		} else {
			send_chat("You can't order me around like that!");
		}
	} else if(!strncasecmp(command, "my name is ", 11)) {
		store_aka(sender, command+11);
	} else if(!strncasecmp(command, "my email is ", 12)) {
		store_email(sender, command+12);
	} else if(!strncasecmp(command, "my url is ", 10)) {
		store_url(sender, command+10);
	} else if(!strncasecmp(command, "my info is ", 11)) {
		store_info(sender, command+11);
	} else if(!strncasecmp(command, "my birthday is ", 15)) {
		store_bday(sender, command+15);
	} else if(!strncasecmp(command, "have you seen ", 14)) {
		check_seen(sender, command+14);
	} else if(!strncasecmp(command, "tell ", 5)) {
		queue_message(sender, command+5);
	} else if(!strncasecmp(command, "who is ", 7)) {
		whois(sender, command+7);
	} else if(!strncasecmp(command, "whois ", 6)) {
		whois(sender, command+6);
	} else if(!strncasecmp(command, "log on ", 7)) {
		if(!strcmp(sender, owner)) {
			log_on(sender,command+7);
		} else {
			send_chat("Sorry, only my owner can tell me to do that.");
		}
	} else if(!strncasecmp(command, "log off", 7)) {
		if(!strcmp(sender, owner)) {
			log_off(sender, NULL);
		} else {
			send_chat("Sorry, only my owner can tell me to do that.");
		}
	} else if(!strncasecmp(command, "save memory", 14)) {
		if(!strcmp(sender, owner)) {
			save_known();
			send_chat("My memory is saved!");
		} else {
			send_chat("Sorry, only my owner can tell me to do that.");
		}
	} else if(!strncasecmp(command, "stay", 4)) {
		if(!strcmp(sender, owner)) {
			if(stay_still == 0){
				stay_still = 1;
				send_chat("Yes Master, I wont move again!");
			} else {
				send_chat("I'm already staying put");
			}
		} else {
			send_chat("Sorry, only my owner can tell me to do that.");
		}
	} else if(!strncasecmp(command, "move", 4)) {
		if(!strcmp(sender, owner)) {
			if(stay_still == 1){
				stay_still = 0;
				send_chat("Yes Master, I'll wonder around now.");
			} else {
				send_chat("I'm already moving about!");
			}
		} else {
			send_chat("Sorry, only my owner can tell me to do that.");
		}
	} else {
		send_chat("Huh?");
		sprintf(out_msg, "Type '%s: Help' to see what commands I know!",
			bot_name);
		send_chat(out_msg);
	}
}


void send_chat(char *msg)
{
	es_write_int(my_socket, REQ_CHAT);
	es_write_string(my_socket, msg);
}


void send_chat_insert_name(char *msg)
{
	char *p;
	int percent_s=0;

	for(p=msg; *p!='\0'; p++)
		if(*p == '%' && *(p+1) == 's')
			percent_s++;

	if(percent_s)
		sprintf(out_msg, msg, bot_name);
	else
		strcpy(out_msg, msg);

	send_chat(out_msg);
}


void graceless_exit(int sig)
{
	sig_logout = 1;
}


void do_logout(int on_sig)
{
	/* Logout of GGZ server */
	if(!on_sig)
		es_write_int(my_socket, REQ_LOGOUT);
	
	save_known();

	if(on_sig)
		exit(0);
}

	
void save_known()
{
	FILE *grub_file;
	char *home;
	char file[1024];
	int i, j;

	/* Save current information */
	home = getenv("HOME");
	if (home == NULL)
	{
		printf("$HOME environment variable not defined!\n\n");
		return;
	}
	
	strcpy(file, home);
	strcat(file,"/.agrub");

	grub_file = fopen(file, "w");
	if (grub_file == NULL)
	{
		printf("Error saveing known information\n\n");
		return;
	}

	/* Write out information */
	fprintf(grub_file, "%s\n", FILE_VERSION);
	fprintf(grub_file, "%d\n", num_known);
	
	for(i=0;i<num_known;i++)
	{
		if(known[i].name != NULL)
			fprintf(grub_file, "%s\n", known[i].name);
		else
			fprintf(grub_file, "NULL\n");
		if(known[i].aka != NULL)
			fprintf(grub_file, "%s\n", known[i].aka);
		else
			fprintf(grub_file, "NULL\n");
		if(known[i].email != NULL)
			fprintf(grub_file, "%s\n", known[i].email);
		else
			fprintf(grub_file, "NULL\n");
		if(known[i].url != NULL)
			fprintf(grub_file, "%s\n", known[i].url);
		else
			fprintf(grub_file, "NULL\n");
		if(known[i].info != NULL)
			fprintf(grub_file, "%s\n", known[i].info);
		else
			fprintf(grub_file, "NULL\n");
		if(known[i].bday != NULL)
			fprintf(grub_file, "%s\n", known[i].bday);
		else
			fprintf(grub_file, "NULL\n");

		fprintf(grub_file, "%d\n", known[i].last_seen);

		if(known[i].last_room != NULL)
			fprintf(grub_file, "%s\n", known[i].last_room);
		else
			fprintf(grub_file, "NULL\n");
	
		fprintf(grub_file, "%d\n", known[i].msg_count);
		for (j=0;j<known[i].msg_count;j++)
		{
			fprintf(grub_file, "%s\n", known[i].msg[j].msg_from);
			fprintf(grub_file, "%s\n", known[i].msg[j].msg);
		}
	}

	fclose(grub_file);
}


void do_greet(char *name)
{
	int i,j,k;

	if(!strcmp(name, bot_name)) {
		free(name);
		return;
	}

	for(i=0; i<num_known; i++)
		if(!strcmp(known[i].name, name))
			break;

	if(i == num_known || i >= MAX_KNOWN) {
		sprintf(out_msg, "Hi %s, I'm %s.", name, bot_name);
		send_chat(out_msg);
		sprintf(out_msg, "I haven't seen you before, please take a second to introduce yourself to me.");
		send_chat(out_msg);
		sprintf(out_msg, "I'll remember your name if you say '%s: my name is <your name>'.", bot_name);
		send_chat(out_msg);
		sprintf(out_msg, "You can always type '%s: help' to see what I can do!", bot_name);
		send_chat(out_msg);
		if(i < MAX_KNOWN) {
			known[num_known].name = name;
			known[num_known].last_seen = time(NULL);
			known[num_known++].last_room = room_list[cur_room];
		} else {
			sprintf(out_msg,
				"Sorry %s, but I can only remember %d people.",
				name, MAX_KNOWN);
			send_chat(out_msg);
			free(name);
		}
	} else {
		known[i].last_seen = time(NULL);
		known[i].last_room = room_list[cur_room];
		if(known[i].aka)
			sprintf(out_msg, "Good to see you again, %s!", known[i].aka);
		else
			sprintf(out_msg, "Good to see you again, %s!", name);
		send_chat(out_msg);
		if((random()%15) == 0) {
			sprintf(out_msg,
				"Remember, you can type '%s: help'",
				bot_name);
			send_chat(out_msg);
		}
		if((random()%15) == 0) {
			k=(int) (total_greating*rand()/(RAND_MAX+1.0));
			send_chat(great_strings[k]);
		}
		for(j=0;j<known[i].msg_count;j++)
		{
			sprintf(out_msg, "%s wanted me to tell you:",
				known[i].msg[j].msg_from);
			send_chat(out_msg);
			sprintf(out_msg, "    '%s'", known[i].msg[j].msg);
			send_chat(out_msg);
			known[i].msg[j].msg_from = NULL;
			free(known[i].msg[j].msg);
			known[i].msg[j].msg = NULL;
		}
		known[i].msg_count=0;
		free(name);
	}
}


void store_aka(char *sender, char *aka)
{
	int i;
	char *newstr;

	for(i=0; i<num_known; i++)
		if(!strcmp(sender, known[i].name))
			break;
	if(i >= num_known) {
		sprintf(out_msg, "Sorry %s, I'd like to remember you as %s, ",
			sender, aka);
		sprintf(out_msg+strlen(out_msg),
			"but I already know %d people.", MAX_KNOWN);
		send_chat(out_msg);
		return;
	}

	if(known[i].aka) {
		sprintf(out_msg, "OK %s, changing your name to %s.",
			known[i].aka, aka);
		free(known[i].aka);
	} else
		sprintf(out_msg, "OK %s, I'll remember that your name is %s.",
			sender, aka);
	send_chat(out_msg);

	newstr = malloc(strlen(aka)+1);
	strcpy(newstr, aka);
	known[i].aka = newstr;
}


void log_on(char *sender, char *command)
{
	if(log_file != NULL)
	{
		send_chat("I'm already logging this room.");
		return;
	}
	log_file = fopen(command, "w");
	if (log_file == NULL)
	{
		send_chat("Error opening file.");
		return;
	}
	fprintf(log_file,"<center>\n");
	fprintf(log_file,"<table border=0 width=540 cellpadding=\"1\" cellspacing=\"0\">\n");
	send_chat("Logging on.");
}


void log_off(char *sender, char *command)
{
	if (log_file == NULL)
	{
		send_chat("I'm not logging anything");
		return;
	}
	send_chat("Logging off.");
	fclose(log_file);
	log_file=NULL;
}


void store_email(char *sender, char *email)
{
	int i;
	char *newstr;

	for(i=0; i<num_known; i++)
		if(!strcmp(sender, known[i].name))
			break;
	if(i >= num_known) {
		sprintf(out_msg, "Sorry %s, I'd like to remember your email as %s, ",
			sender, email);
		sprintf(out_msg+strlen(out_msg),
			"but I already know %d people.", MAX_KNOWN);
		send_chat(out_msg);
		return;
	}

	if(known[i].email) {
		sprintf(out_msg, "OK %s, changing your email to %s.",
			known[i].aka, email);
		free(known[i].email);
	} else
		sprintf(out_msg, "OK %s, I'll remember that your email is %s.",
			sender, email);
	send_chat(out_msg);

	newstr = malloc(strlen(email)+1);
	strcpy(newstr, email);
	known[i].email = newstr;
}


void store_url(char *sender, char *url)
{
	int i;
	char *newstr;

	for(i=0; i<num_known; i++)
		if(!strcmp(sender, known[i].name))
			break;
	if(i >= num_known) {
		sprintf(out_msg, "Sorry %s, I'd like to remember your url as %s, ",
			sender, url);
		sprintf(out_msg+strlen(out_msg),
			"but I already know %d people.", MAX_KNOWN);
		send_chat(out_msg);
		return;
	}

	if(known[i].url) {
		sprintf(out_msg, "OK %s, changing your url to %s.",
			known[i].aka, url);
		free(known[i].url);
	} else
		sprintf(out_msg, "OK %s, I'll remember that your url is %s.",
			sender, url);
	send_chat(out_msg);

	newstr = malloc(strlen(url)+1);
	strcpy(newstr, url);
	known[i].url = newstr;
}


void store_info(char *sender, char *info)
{
	int i;
	char *newstr;

	for(i=0; i<num_known; i++)
		if(!strcmp(sender, known[i].name))
			break;
	if(i >= num_known) {
		sprintf(out_msg, "Sorry %s, I'd like to remember your info as %s, ",
			sender, info);
		sprintf(out_msg+strlen(out_msg),
			"but I already know %d people.", MAX_KNOWN);
		send_chat(out_msg);
		return;
	}

	if(known[i].aka) {
		sprintf(out_msg, "OK %s, changing your info to %s.",
			known[i].aka, info);
		free(known[i].info);
	} else
		sprintf(out_msg, "OK %s, I'll remember that your info as %s.",
			sender, info);
	send_chat(out_msg);

	newstr = malloc(strlen(info)+1);
	strcpy(newstr, info);
	known[i].info = newstr;
}

void store_bday(char *sender, char *bday)
{
	int i;
	char *newstr;

	for(i=0; i<num_known; i++)
		if(!strcmp(sender, known[i].name))
			break;
	if(i >= num_known) {
		sprintf(out_msg, "Sorry %s, I'd like to remember your brithday as %s, ",
			sender, bday);
		sprintf(out_msg+strlen(out_msg),
			"but I already know %d people.", MAX_KNOWN);
		send_chat(out_msg);
		return;
	}

	if(known[i].aka) {
		sprintf(out_msg, "OK %s, changing your birthday to %s.",
			known[i].aka, bday);
		free(known[i].bday);
	} else
		sprintf(out_msg, "OK %s, I'll remember that your birthday as %s.",
			sender, bday);
	send_chat(out_msg);

	newstr = malloc(strlen(bday)+1);
	strcpy(newstr, bday);
	known[i].bday = newstr;
}


void check_seen(char *sender, char *name)
{
	int i, j;
	time_t now;
	char *disp_sender, *disp_name;
	int secs, mins, hours, days, sz;
	char tstr[128];

	now = time(NULL);

	for(i=0; i<num_known; i++)
		if(!strcasecmp(known[i].name, name))
			break;
	for(j=0; j<num_known; j++)
		if(!strcasecmp(known[j].name, sender))
			break;
	disp_sender = sender;
	if(j != num_known && known[j].aka)
		disp_sender = known[j].aka;

	if(i == num_known)
		sprintf(out_msg, "Sorry %s, I've never encountered %s.",
			disp_sender, name);
	else {
		if(known[i].aka)
			disp_name = known[i].aka;
		else
			disp_name = name;

		secs = difftime(now, known[i].last_seen);
		days = secs/86400;
		secs -= days*86400;
		hours = secs/3600;
		secs -= hours*3600;
		mins = secs/60;
		secs -= mins*60;

		sz = 0;
		if(days > 0)
			if(days > 1)
				sz = sprintf(tstr, "%d days, ", days);
			else
				sz = sprintf(tstr, "1 day, ");
		if(hours > 0)
			if(hours > 1)
				sz += sprintf(tstr+sz, "%d hours, ", hours);
			else
				sz += sprintf(tstr+sz, "1 hour, ");
		if(mins == 1)
			sz += sprintf(tstr+sz, "1 minute and ");
		else
			sz += sprintf(tstr+sz, "%d minutes and ", mins);
		if(secs == 1)
			sprintf(tstr+sz, "1 second");
		else
			sprintf(tstr+sz, "%d seconds", secs);

		sprintf(out_msg,
		       "I last greeted %s %s ago in the '%s' room, %s.",
			disp_name, tstr, known[i].last_room, disp_sender);
	}

	send_chat(out_msg);
}

void whois(char *sender, char *name)
{
	int i, j;
	time_t now;
	char *disp_sender, *disp_name, *email, *url, *info;

	now = time(NULL);

	for(i=0; i<num_known; i++)
		if(!strcasecmp(known[i].name, name))
			break;
	for(j=0; j<num_known; j++)
		if(!strcasecmp(known[j].name, sender))
			break;
	disp_sender = sender;
	if(j != num_known && known[j].aka)
	{
		disp_sender = known[j].aka;
	}

	if(i == num_known)
	{
		sprintf(out_msg, "Sorry %s, I've never encountered %s.",
			disp_sender, name);
		send_chat(out_msg);
	} else {
		if(known[i].aka)
			disp_name = known[i].aka;
		else
			disp_name = name;
		email = known[i].email;
		url = known[i].url;
		info = known[i].info;
		sprintf(out_msg, "%s, %s's name is %s.", disp_sender, name, disp_name);
		send_chat(out_msg);
		if(email != NULL)
		{
			sprintf(out_msg, "%s, %s's email is %s.", disp_sender, disp_name, email);
			send_chat(out_msg);
		}
		if(url != NULL)
		{
			sprintf(out_msg, "%s, %s's URL is %s.", disp_sender, disp_name, url);
			send_chat(out_msg);
		}
		if(info != NULL)
		{
			sprintf(out_msg, "%s, %s's info is \"%s\".", disp_sender, disp_name, info);
			send_chat(out_msg);
		}
	}

}


void queue_message(char *sender, char *command)
{
	int i;
	char *p, *newstr, *disp_sender, *disp_name;

	for(p=command; *p!='\0' && *p!=' '; p++)
		;
	if(*p == '\0') {
		send_chat("Huh?");
		sprintf(out_msg,
			"Type '%s: Help' to see what commands I know!",
			 bot_name);
		send_chat(out_msg);
		return;
	}

	*p = '\0';
	p++;
	if(*p == '\0') {
		send_chat("Huh?");
		sprintf(out_msg,
			"Type '%s: Help' to see what commands I know!",
			 bot_name);
		send_chat(out_msg);
		return;
	}

	for(i=0; i<num_known; i++)
		if(!strcasecmp(sender, known[i].name))
			break;
	if(i == num_known) {
		sprintf(out_msg, "Sorry %s, I can't remember your name.",
			sender);
		send_chat(out_msg);
		return;
	}
	disp_sender = known[i].name;
	if(known[i].aka)
		disp_sender = known[i].aka;

	for(i=0; i<num_known; i++)
		if(!strcasecmp(command, known[i].name))
			break;
	if(i == num_known) {
		sprintf(out_msg, "Sorry %s, I don't know %s.",
			disp_sender, command);
		send_chat(out_msg);
		return;
	}

	if(known[i].msg_count == MAX_MESSAGES) {
		sprintf(out_msg, "Sorry %s, I already %d messages for %s.",
			disp_sender, MAX_MESSAGES, command);
		send_chat(out_msg);
		return;
	}

	newstr = malloc(strlen(p)+1);
	strcpy(newstr, p);
	known[i].msg[known[i].msg_count].msg_from = disp_sender;
	known[i].msg[known[i].msg_count].msg = newstr;
	known[i].msg_count++;

	disp_name = known[i].name;
	if(known[i].aka)
		disp_name = known[i].aka;

	sprintf(out_msg, "OK %s, I'll deliver that message next time I see %s.",
		disp_sender, disp_name);
	send_chat(out_msg);
}


log_it(char *left, char *right)
{
	if (log_file != NULL)
	{
		if(!strcmp("-->",left) || !strcmp("<--",left) || !strcmp("---",left) || !strcmp("*",left))
			fprintf(log_file, "<tr><td valign=top align=right>%s</td><td bgcolor=\"#777777\">&nbsp</td><td valign=top>%s</td></td>\n",left, right);
		else
			fprintf(log_file, "<tr><td valign=top align=right>[%s]</td><td bgcolor=\"#777777\">&nbsp</td><td valign=top>%s</td></td>\n",left, right);
	}
}
