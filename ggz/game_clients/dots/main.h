/* Maximums */
#define MAX_BOARD_WIDTH  25
#define MAX_BOARD_HEIGHT 25

/* Exposed globals */
extern GtkWidget *main_win, *opt_dialog;
extern char game_state;

struct game_t {
	/* Basic info about connection */
	int fd;
	int me;
	int opponent;
	int seats[2];
	int score[2];
	char names[2][9];
	char state;
	char move;
	char got_players;
};
extern struct game_t game;

/* Exposed functions */
extern int send_options(void);
extern void game_init(void);
extern void handle_req_newgame(void);
