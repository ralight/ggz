/* loop.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include "datatypes.h"
#include "net.h"

extern struct Grubby grubby;
extern int sig;


void main_loop( void )
{

	fd_set	select_mux;
struct 	timeval	timeout;

	while(1)
	{
		FD_ZERO(&select_mux);
		FD_SET(grubby.socket, &select_mux);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		if(select(grubby.socket+1, &select_mux, NULL, NULL, &timeout)) {
			if(FD_ISSET(grubby.socket, &select_mux)) {
				/* Read from socket */
				get_op_code();
			}
		} else {
			switch(sig)
			{
				case SIG_NORMAL:
					/* Nothing should ever happen here */
					break;
				case SIG_LOGIN:
					login();
					break;
				case SIG_LOGOUT:
					break;
				case SIG_IDLE:
					/* Chit-Chat, Parsing, everything else*/
					break;
				case SIG_CHANGEROOM:
					change_room(0);
					sig=SIG_NORMAL;
					break;
				case SIG_CLEANUP:
					break;
			}
		}
	}
}
