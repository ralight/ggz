// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Header file
#include "ggzap_game.h"

// System includes
#include <string.h>
#ifndef strdup
#define strdup(x) strcpy(((char*)malloc(strlen(x) + 1)), x)
#endif
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <fnmatch.h>

GGZapGame::GGZapGame()
{
	gamelist = NULL;
	m_count = 0;
}

GGZapGame::~GGZapGame()
{
	clear();
}

void GGZapGame::addGame(const char *name, const char *frontend)
{
	gamelist = (char***)realloc(gamelist, sizeof(char**) * (m_count + 1));
	gamelist[m_count] = (char**)malloc(sizeof(char*) * 2);
	gamelist[m_count][0] = (char*)malloc(strlen(name) + 1);
	gamelist[m_count][1] = (char*)malloc(strlen(frontend) + 1);
	strcpy(gamelist[m_count][0], name);
	strcpy(gamelist[m_count][1], frontend);
	m_count++;
}

int GGZapGame::count()
{
	return m_count;
}

char *GGZapGame::name(int id)
{
	if((id < 0) || (id >= m_count)) return NULL;
	return gamelist[id][0];
}

char *GGZapGame::frontend(int id)
{
	if((id < 0) || (id >= m_count)) return NULL;
	return gamelist[id][1];
}

void GGZapGame::clear()
{
	for(int i = 0; i < m_count; i++)
	{
		free(gamelist[i][0]);
		free(gamelist[i][1]);
		free(gamelist[i]);
	}
	if(gamelist) free(gamelist);
	m_count = 0;
	gamelist = NULL;
}

void GGZapGame::autoscan()
{
	char dir[1024];
	char filename[1024];
	char buffer[512];
	char *token;
	FILE *f;
	DIR *dp;
	struct dirent *ep;
	char *module, *frontend;

	strcpy(dir, getenv("HOME"));
	strcat(dir, "/.kde/share/applnk/Games/ggz/games");
	dp = opendir(dir);
	if(dp)
	{
		while((ep = readdir(dp)) != 0)
		{
			if(!fnmatch("*.desktop", ep->d_name, FNM_NOESCAPE))
			{
				module = NULL;
				frontend = NULL;
				sprintf(filename, "%s/%s", dir, ep->d_name);
				f = fopen(filename, "r");
				if(f)
				{
					while(fgets(buffer, sizeof(buffer), f))
					{
						buffer[strlen(buffer) - 1] = 0;
						if(strncmp(buffer, "Exec", 4) == 0)
						{
							token = strtok(buffer, " ");
							while(token)
							{
								if(!strcmp(token, "--module") && (token = strtok(NULL, " "))) module = strdup(token);
								if(!strcmp(token, "--frontend") && (token = strtok(NULL, " "))) frontend = strdup(token);
								if(token) token = strtok(NULL, " ");
							}
						}
					}
					fclose(f);
				}
				if((module) && (frontend)) addGame(module, frontend);
				if(module) free(module);
				if(frontend) free(frontend);
			}
		}
		closedir(dp);
	}
}

