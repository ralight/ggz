#include "../config.h"
#include "game.h"
#include "ships.h"

char *ship_file[] = {"naboo.gif", "xwing.gif", "awing.gif", "tie.gif",
	"shoot.gif", "outrider.gif", "advanced.gif", "bomber.gif",
	"deathstar.gif", "rocket.gif", "interceptor.gif", "enshoot.gif",
	"dshoot.gif"};
int ship_width[] = {76, 45, 45, 28, 6, 50, 40, 39, 300, 6, 37, 6, 60};
int ship_height[] = {18, 18, 24, 21, 3, 35, 27, 22, 300, 3, 23, 3, 30};
int ship_speed[] = {8, 6, 10, 8, 15, 5, 9, 5, 4, 15, 10, 15, 13};
int ship_hp[] = {2, 2, 1, 1, 0, 3, 3, 3, 20, 0, 2, 0, 0};

char *def_file (int type)
{
	return ship_file[type];
}

int def_width (int type)
{
	return ship_width[type];
}

int def_height (int type)
{
	return ship_height[type];
}

int def_speed (int type)
{
	return ship_speed[type];
}

int def_hp (int type)
{
	return ship_hp[type];
}
