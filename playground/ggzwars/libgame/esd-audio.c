#include "libgame.h"

void esd_playwav (char *filename)
{
	esd_play_file (NULL, filename, 0);
}
