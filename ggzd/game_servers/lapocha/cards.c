#include <stdlib.h>

#include "cards.h"

char deck[GGZ_MAX_DECKSIZE];
int deck_size;


/* cards_shuffle_deck()
 *   Setup and shuffle a deck of type
 *
 *   This is more complex than might seem necessary, but allows a game-designer
 *   to very easily invent new card deck types by merely setting the params
 */
void cards_shuffle_deck(char type)
{
	char *deck_cards, *deck_suits;
	char *c, *s;
	int card, card_value, suit_value;
	char *std_cards = "A234567890JQK";
	char *std_suits = "CDHS";
	int i, j;
	char temp;

	/* First set the deck parameters up */
	switch(type) {
		case GGZ_DECK_LAPOCHA:
			/* La Pocha doesn't uses 7,8,9 */
			deck_cards = "A234560JQK";
			deck_suits = "CDHS";
			break;
		case GGZ_DECK_STANDARD:
		default:
			deck_cards = "A234567890JQK";
			deck_suits = "CDHS";
			break;
	}

	/* Now generate an in-order deck */
	card = 0;
	s = deck_suits;
	while(*s != '\0') {
		for(suit_value=0;
		    std_suits[suit_value] != *s;
		    suit_value++)
			;
		c = deck_cards;
		while(*c != '\0') {
			for(card_value=0;
			    std_cards[card_value] != *c;
			    card_value++)
				;
			deck[card] = card_value + (13 * suit_value);
			c++;
			card++;
		}
		s++;
	}
	deck_size = card;

	/* Now we can randomize the deck order */
	/* Go through the deck, card by card */
	for(i=0; i<deck_size; i++) {
		/* Pick any OTHER card */
		do {
			j = random() % deck_size;
		} while(j == i);
		/* And swap positions */
		temp = deck[i];
		deck[i] = deck[j];
		deck[j] = temp;
	}
}


/* cards_deal_hands
 *   This deals the cards out from a pre-shuffled deck into hand structures
 */
void cards_deal_hands(int num_players, int hand_size, struct hand_t *hand)
{
	int i, j, player;
	char c1, c2;
	int card=0;
	char temp;

	/* Deal the cards out */
	for(i=0; i<hand_size; i++)
		for(player=0; player<num_players; player++)
			hand[player].card[i] = deck[card++];

	/* Store the handsize and sort cards */
	for(player=0; player<num_players; player++) {
		hand[player].hand_size = hand_size;
		for(i=0; i<hand_size-1; i++)
			for(j=i+1; j<hand_size; j++) {
				/* Mangle so ace comes out high */
				c1 = hand[player].card[i]-1;
				c2 = hand[player].card[j]-1;
				if((c1+1) % 13 == 0)
					c1 += 13;
				if((c2+1) % 13 == 0)
					c2 += 13;
				if(c1 > c2){
					temp = hand[player].card[i];
					hand[player].card[i] =
					                   hand[player].card[j];
					hand[player].card[j] = temp;
				}
			}
	}
}
