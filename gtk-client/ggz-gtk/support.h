/*
 * File: support.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: support.h 10274 2008-07-10 21:38:34Z jdorje $
 *
 * Support code
 *
 * Copyright (C) 2004 GGZ Development Team
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include <gtk/gtk.h>

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext ("ggz-gtk", String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#define INTERNAL __attribute__ ((visibility ("internal")))


/*
 * Public Functions.
 */

/*
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
GtkWidget INTERNAL *ggz_lookup_widget              (GtkWidget       *widget,
                                        const gchar     *widget_name);

/*
 * Use this function to load a URL in netscape
 */
int INTERNAL support_goto_url(gchar *url);

/*
 * Use this function to fork a command
 */
void INTERNAL support_exec(char *cmd);

/* get_widget() is deprecated. Use ggz_lookup_widget instead. */
#define get_widget ggz_lookup_widget

/* Use this function to set the directory containing installed pixmaps. */
void INTERNAL add_pixmap_directory       (const gchar     *directory);


/*
 * Private Functions.
 */

/* This is used to create the pixmaps in the interface. */
GtkWidget INTERNAL *create_pixmap              (GtkWidget       *widget,
                                        const gchar     *filename);


GdkPixbuf INTERNAL *load_pixbuf(const char *name);
GdkPixbuf INTERNAL *load_svg_pixbuf(const char *name, int width, int height);

#endif
