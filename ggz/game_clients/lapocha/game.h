struct game_t {
	int fd;
	int me;
	int seats[4];
	int score[4];
	int bid[4];
	int tricks[4];
	int num_cards[4];
	char names[4][17];
	char state;
	char move;
	int trump_suit;
	char got_players;
};
extern struct game_t game;

extern void game_send_bid(int);
extern void game_play_card(int);

/* LaPocha protocol */
/* Messages from server */
#define LP_MSG_SEAT     0
#define LP_MSG_PLAYERS  1
#define LP_MSG_GAMEOVER 2
#define LP_MSG_HAND     3
#define LP_REQ_BID	4
#define LP_RSP_BID	5
#define LP_MSG_BID	6
#define LP_REQ_PLAY     7
#define LP_RSP_PLAY     8
#define LP_MSG_PLAY	9
#define LP_SND_SYNC     10
#define LP_MSG_TRUMP	11
#define LP_REQ_TRUMP	12
#define LP_MSG_TRICK	13
#define LP_MSG_SCORES	14

/* Play errors */
#define LP_ERR_STATE       -2
#define LP_ERR_TURN        -3
#define LP_ERR_INVALID     -4
#define LP_ERR_FOLLOW_SUIT -5
#define LP_ERR_MUST_TRUMP  -6

/* Messages from client */
#define LP_SND_PLAY     0
#define LP_SND_BID      1
#define LP_REQ_SYNC     2
#define LP_REQ_NEWGAME  3
#define LP_SET_TRUMP	4

/* LaPocha game states */
#define LP_STATE_INIT		0
#define LP_STATE_WAIT		1
#define LP_STATE_BID		2
#define LP_STATE_PLAY		3
#define LP_STATE_ANIM		4
#define LP_STATE_DONE		5
#define LP_STATE_OPPONENT	6
#define LP_STATE_CHOOSE		7

/* LaPocha game events */
#define LP_EVENT_LAUNCH      0
#define LP_EVENT_JOIN        1
#define LP_EVENT_LEAVE       2
#define LP_EVENT_MOVE_H      3
#define LP_EVENT_MOVE_V      4

/* GGZ define */
#define GGZ_SEAT_OPEN	       -1
