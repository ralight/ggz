struct hand_t {
	char hand_size;
	char card[13];
	char selected_card;
	char in_play_card_num;
	char in_play_card_val;
};

extern struct hand_t hand;

extern int hand_read_hand(void);
