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

void main_loop( void )
{
        for (;;) {
				if(ggzcore_server_data_is_pending)
					ggzcore_server_read_data(activeserver);
		ggzcore_conf_commit();
		chat_chatter();
        }
}

