#ifndef _AGRUB_DATATYPES_
#define _AGRUB_DATATYPES_

#include <stdio.h>
#include <time.h>

/*
 *  Server Limites
 */

#define MAX_USER_NAME_LEN	16		/* Max length of a username, this is imposed by	*/
						/* the server					*/


/*
 *  Grubby limites
 */

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
	char	*logtype;			/* HTML or TEXT					*/
	char	*logname;			/* Full path to log file			*/
	FILE	*logfile;			/* Log file pointer				*/
};


/* Internal status signals (sig)	*/
#define SIG_NORMAL	0
#define SIG_LOGIN	1
#define SIG_LOGOUT	2
#define SIG_IDLE	3
#define SIG_CHANGEROOM	4
#define SIG_CLEANUP	5

#endif

