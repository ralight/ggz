#ifndef GGZ_CARDS_INCLUDED

/* A hand structure */
struct hand_t {
	char hand_size;
	char card[13];
};

/* Deck types */
#define GGZ_DECK_STANDARD	1
#define GGZ_DECK_LAPOCHA	2
#define GGZ_MAX_DECKSIZE	52

/* Exported functions */
extern void cards_shuffle_deck(char);
extern void cards_deal_hands(int, int, struct hand_t *);
extern char cards_cut_for_trump(void);
extern char cards_suit_in_hand(struct hand_t *, char);
extern char cards_highest_in_suit(struct hand_t *, char);
extern char card_value(char);

#define GGZ_CARDS_INCLUDED
#endif /*GGZ_CARDS_INCLUDED*/
