/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Wed Mar  7 19:16:57 /etc/localtime 2001
    copyright            : (C) 2001 by Ismael Orenstein
    email                : perdig@linuxbr.com.br
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <qstring.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <config.h>
#include <stdlib.h>

#include "kreversi.h"

static const char *REV_VERSION = I18N_NOOP("0.0.1");

static const char *description =
	I18N_NOOP("KDE client for GGZ's Reversi game module");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { "o", I18N_NOOP("This game will use GGZ"), 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

  KAboutData aboutData( "ggz.kreversi", I18N_NOOP("KGGZReversi"),
    REV_VERSION, description, KAboutData::License_GPL,
    "(c) 2001, Ismael Orenstein", 0, 0, "perdig@linuxbr.com.br");
  aboutData.addAuthor("Ismael Orenstein",0, "perdig@linuxbr.com.br");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication a;
  KReversi *kreversi = new KReversi();
  a.setMainWidget(kreversi);
  if (kreversi->initAll() < 0)
    return -5;
  else
    return a.exec();
}

int ggz_connect(char *name) {
  char fd_name[30];
  int sock;
  struct sockaddr_un addr;
 
  /* Connect to Unix domain socket */
  sprintf(fd_name, "/tmp/%s.%d", name, getpid());
 
  if ( (sock = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
    exit(-1);
 
  bzero(&addr, sizeof(addr));
  addr.sun_family = AF_LOCAL;
  strcpy(addr.sun_path, fd_name);
 
  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    exit(-1);
 
  return sock;
}

int select_dirs(const struct dirent *d)
{
  int ok=0;
  struct stat buf;
  QString pathname(GGZDATADIR "/kreversi/pixmaps/");
  pathname += d->d_name;
  if(stat(pathname, &buf) == -1)
    perror(pathname);
  if(S_ISDIR(buf.st_mode))
    ok = 1;
 
  if(ok && d->d_name[0] != '.')
    return 1;
  return 0;
}
