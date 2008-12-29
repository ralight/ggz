/*
 * GGZ Starterpack for C - Sample Server
 * Copyright (C) 2008 GGZ Development Team
 *
 * This code is made available as public domain; you can use it as a base
 * for your own game, as long as its licence is compatible with the libraries
 * you use.
 */

#include <ggzdmod.h>

#include "game.h"

int main(void)
{
	/* ggzdmod initializations */
	GGZdMod *ggz = ggzdmod_new(GGZDMOD_GAME);
	game_init(ggz);

	/* Connect to GGZ server; main loop */
	if (ggzdmod_connect(ggz) < 0)
		return -1;
	(void) ggzdmod_loop(ggz);
	(void) ggzdmod_disconnect(ggz);
	ggzdmod_free(ggz);

	return 0;
}

