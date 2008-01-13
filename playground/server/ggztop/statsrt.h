#ifndef GGZ_STATS_RT_H
#define GGZ_STATS_RT_H

#include <semaphore.h>

#define STATS_RT_SHMID            0x5688
#define STATS_RT_VERSION          1

#define STATS_RT_MAX_ROOMS        100
#define STATS_RT_MAX_ROOMNAME_LEN 32

struct stats_rt_t
{
	int version;
	int magicversion;
	sem_t semlock;

	int uptime;
	int num_rooms;
	int num_players;
	int num_tables;
	char rooms[STATS_RT_MAX_ROOMS][STATS_RT_MAX_ROOMNAME_LEN];
	int players[STATS_RT_MAX_ROOMS];
	int tables[STATS_RT_MAX_ROOMS];
	int chat[STATS_RT_MAX_ROOMS];
};
typedef struct stats_rt_t stats_rt;

#endif
