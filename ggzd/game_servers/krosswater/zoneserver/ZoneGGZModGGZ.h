#define MAX_USER_NAME_LEN 16
#define GGZ_SEAT_OPEN   -1
#define GGZ_SEAT_BOT    -2
#define GGZ_SEAT_RESV   -3
#define GGZ_SEAT_NONE   -4
#define GGZ_SEAT_PLAYER -5
#define GGZ_DBG_CONFIGURATION	(unsigned) 0x00000001
#define GGZ_DBG_PROCESS		(unsigned) 0x00000002
#define GGZ_DBG_CONNECTION	(unsigned) 0x00000004
#define GGZ_DBG_CHAT		(unsigned) 0x00000008
#define GGZ_DBG_TABLE		(unsigned) 0x00000010
#define GGZ_DBG_PROTOCOL	(unsigned) 0x00000020
#define GGZ_DBG_UPDATE		(unsigned) 0x00000040
#define GGZ_DBG_MISC		(unsigned) 0x00000080
#define GGZ_DBG_ROOM		(unsigned) 0x00000100
#define GGZ_DBG_LISTS		(unsigned) 0x00000200
#define GGZ_DBG_GAME_MSG	(unsigned) 0x00000400
#define GGZ_DBG_ALL		(unsigned) 0xFFFFFFFF

struct ggz_seat_t {
	int assign;
	char name[MAX_USER_NAME_LEN +1];
	int fd;
};

class ZoneGGZModGGZ
{
public:
	static int ggz_init(char* game_name);
	static int ggz_connect(void);
	static int ggz_done(void);
	static void ggz_quit(void);
	static int ggz_game_launch(void);
	static int ggz_game_over(void);
	static int ggz_player_join(int* seat, int *fd);
	static int ggz_player_leave(int* seat, int *fd);
	static void ggz_debug(const char *fmt, ...);
	static int ggz_seats_open(void);
	static int ggz_seats_num(void);
	static int ggz_seats_bot(void);
	static int ggz_seats_reserved(void);
	static int ggz_seats_human(void);
	static int ggz_fd_max(void);

	static struct ggz_seat_t* ggz_seats;

	static char* name;
	static int fd;
	static int seats;
};
