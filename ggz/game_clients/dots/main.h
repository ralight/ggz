/* Maximums */
#define MAX_BOARD_WIDTH  25
#define MAX_BOARD_HEIGHT 25

/* Exposed globals */
extern GtkWidget *main_win;
extern char game_state;

struct game_t {
	/* Basic info about connection */
	int fd;
	int num;
	int seats[2];
	char names[2][9];
	char state;
	int move;
};
extern struct game_t game;
