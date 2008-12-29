/*
 * GGZ Starterpack for C - Sample Client
 * Copyright (C) 2008 GGZ Development Team
 *
 * This code is made available as public domain; you can use it as a base
 * for your own game, as long as its licence is compatible with the libraries
 * you use.
 */

#ifndef SAMPLE_GAME_H
#define SAMPLE__GAME_H

#include <ggz_dio.h>
#include <ggzmod.h>

/* Data structure for sample game; more members can be added easily */
struct game_state_t {
	GGZMod *ggzmod;
};

/* Setup functions */
void game_init(void);

#endif
