#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "main.h"
#include "game.h"
#include "easysock.h"

struct game_t game;


void game_send_bid(int bid)
{
	es_write_int(game.fd, LP_SND_BID);
	es_write_char(game.fd, bid);

	game.bid[game.me] = bid;

	statusbar_message("Sending bid to server");
}


void game_play_card(int card)
{
	es_write_int(game.fd, LP_SND_PLAY);
	es_write_char(game.fd, (char)card);

	statusbar_message("Sending play to server");
}


void game_handle_table_click(char card)
{
	char trump;

	/* Do we care if they clicked a card? */
	if(game.state != LP_STATE_TRUMP)
		return;

	/* Since we drew the cards out of order to make them look
	 * better on the screen, switch 2 and 3 */
	if((trump = card) == 3)
		trump = 2;
	else if(trump == 2)
		trump = 3;

	/* Send out our choice of trump */
	es_write_int(game.fd, LP_SND_TRUMP);
	es_write_char(game.fd, trump);

	statusbar_message("Sending trump selection to server");
}
