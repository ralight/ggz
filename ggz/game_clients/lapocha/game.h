struct game_t {
	/* Basic info about connection */
	int fd;
	int me;
	int seats[4];
	int score[4];
	char names[4][17];
	char state;
	char move;
	char got_players;
};
extern struct game_t game;

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

/* Play errors */
#define LP_ERR_STATE   -1
#define LP_ERR_TURN    -2
#define LP_ERR_INVALID -3

/* Messages from client */
#define LP_SND_PLAY     0
#define LP_SND_BID      1
#define LP_REQ_SYNC     2
#define LP_REQ_NEWGAME  3

/* LaPocha game states */
#define LP_STATE_INIT		0
#define LP_STATE_WAIT		1
#define LP_STATE_PLAY		2
#define LP_STATE_ANIM		3
#define LP_STATE_DONE		4
#define LP_STATE_OPPONENT	5
#define LP_STATE_CHOOSE		6

/* LaPocha game events */
#define LP_EVENT_LAUNCH      0
#define LP_EVENT_JOIN        1
#define LP_EVENT_LEAVE       2
#define LP_EVENT_MOVE_H      3
#define LP_EVENT_MOVE_V      4

/* GGZ define */
#define GGZ_SEAT_OPEN	       -1
