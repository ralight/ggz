struct hand_t {
	int hand_size;
	int card[13];
	int selected_card;
	int in_play_card;
};

extern struct hand_t hand;
