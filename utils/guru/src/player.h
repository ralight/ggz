#ifndef PLAYER_H
#define PLAYER_H

enum Status
{
	STATUS_GUEST,
	STATUS_REGISTERED,
	STATUS_ADMIN,
	STATUS_OWNER
};

struct player_t
{
	char *name;
	char *realname;
	int firstseen, lastseen, lastactive;
	char *language;
	char *publicinfo;
	int status;
	struct player_t *origin;
};

typedef struct player_t Player;

void guru_player_policy(int duplication);
Player *guru_player_lookup(const char *name);
void guru_player_save(Player *p);
void guru_player_free(Player *p);

#endif

