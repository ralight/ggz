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
