#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <easysock.h>

#include "game.h"
#include "table.h"
#include "hand.h"

struct hand_t hand;

/* hand_read_hand()
 *   Sets the cards in our hand based on a server message
 */
int hand_read_hand(void)
{
	int i;

	/* Zap our hand */
	for(i=0; i<10; i++)
		hand.card[i] = -1;
	hand.selected_card = -1;

	/* First find out how many cards in this hand */
	if(es_read_char(game.fd, &hand.hand_size) < 0)
		return -1;

	/* Read in all the card values */
	for(i=0; i<hand.hand_size; i++)
		if(es_read_char(game.fd, &hand.card[i]) < 0)
			return -1;

	/* Everyone got dealt this many cards */
	for(i=0; i<4; i++)
		game.num_cards[i] = hand.hand_size;

	table_display_hand();

	return 0;
}
