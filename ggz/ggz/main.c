/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 2/18/00
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 1998 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include <config.h>

#include <gtk/gtk.h>

#include "client.h"
#include "interface.h"
#include "support.h"
#include "options.h"
#include "parse_opt.h"


Options opt;
GtkWidget *main_win;
GtkWidget *dlg_login;

int main (int argc, char *argv[])
{
  parse_args(argc, argv);

  gtk_init (&argc, &argv);

  main_win = create_main_win();
  gtk_widget_show(main_win);
  dlg_login = create_dlg_login();
  gtk_widget_show(dlg_login);
  
  gtk_main();
  return 0;
}


