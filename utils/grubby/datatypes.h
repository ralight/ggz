#ifndef _AGRUB_DATATYPES_
#define _AGRUB_DATATYPES_

#include <stdio.h>

/*
 *  Server Limites
 */

#define MAX_USER_NAME_LEN	16		/* Max length of a username, this is imposed by	*/
						/* the server					*/


/*
 *  Grubby limites
 */

#define MAX_PEOPLE	50			/* Total number of people Grubby can remember	*/
#define MAX_MESSAGES	5			/* Total number of messages Grubby can remember */
						/* for a person					*/
#define MAX_CHATTER	5			/* Total number or random chatter		*/

/*
 *  Simple little type def for stored messages
 */

typedef struct {
	char	from[MAX_USER_NAME_LEN];	/* Who's this message from?		 	*/
	char	*text;				/* The text of the message.			*/
	int	timestamp;			/* The time the message was recorded		*/
} Message;


/*
 *  This is basicaly grubbies memmory,
 *  all information Grubby "knows" about 
 *  a person is stored here
 */

typedef struct {
	char	*username;			/* Username of someone Grubby has seen		*/
	char	*realname;			/* Real name of that person			*/
	char	*email;				/* Email address for person			*/
	char	*url;				/* URL of person's web site			*/
	char	*info;				/* Comment a person can make			*/
	int	seen;				/* Time when first Grubby first met person	*/
	int	lastseen;			/* Time that Grubby last saw the person		*/
	char	*lastroom;			/* Last room Grubby saw the person in		*/
	int	msgcount;			/* Total number of message this person has	*/
	Message	msg[MAX_MESSAGES];		/* The individual message this person has	*/
} People;

/*
 * Little memmory space for random chatter
 */
typedef struct {
	char *text;
	int type;
} Chatter;

/*
 *  This is information about Grubby
 */

struct Grubby{
	char	*name;				/* Gurbbies user name				*/
	char	*owner;				/* Gurbbies owner's user name			*/
	int	port;				/* Server port to connect to			*/
	char	*host;				/* Server to connect to				*/
	int	socket;				/* Socket of the GGZ server connection		*/
	int	chat_length;			/* The max length of a chat message		*/
	int	stay;				/* 0 => Move around, 1 => Stay put		*/
	int	chatter;			/* 0 => Don't talk unless spoken to		*/
						/* 1 => Chatter every now and again		*/
	int	log;				/* Log whats being said to file			*/
	char	*logtype;			/* HTML or TEXT					*/
	char	*logname;			/* Full path to log file			*/
	FILE	*logfile;			/* Log file pointer				*/
	int	lang_check;			/* Check for fowl words	0 => Yes, 1 => No	*/
};

/* This holds all of AGRUB's memmory */
struct Memmory {
	int	num_people;
	People	people[MAX_PEOPLE];
	int	num_chatter;
	Chatter	chatter[MAX_CHATTER];
};

/* Internal status signals (sig)	*/
#define SIG_NORMAL	0
#define SIG_LOGIN	1
#define SIG_LOGOUT	2
#define SIG_IDLE	3
#define SIG_CHANGEROOM	4
#define SIG_CLEANUP	5

#endif

