/* loop.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <stdlib.h>
#include <poll.h>
#include <unistd.h>   
#include <signal.h>
        
#include <ggzcore.h>
#include "datatypes.h"
#include "net.h"
#include "chat.h"

extern struct Grubby grubby;
extern int sig;

#define TIMEOUT 500

void main_loop( void )
{
	struct pollfd fd[1] = {{STDIN_FILENO, POLLIN, 0}};

        for (;;) {
                if (ggzcore_event_poll(fd, 1, TIMEOUT)) {
                }
		ggzcore_conf_commit();
		chat_chatter();
        }
}
