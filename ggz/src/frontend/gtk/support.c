/*
 * File: support.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: support.c 6391 2004-11-16 07:15:24Z jdorje $
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

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#include <ggzcore.h>
#include <ggz.h>

#include "props.h"
#include "support.h"

/* This is an internally used parse commands */
static int my_poptParseArgvString(const char *s, int *argcPtr,
				  char ***argvPtr);

GtkWidget *lookup_widget(GtkWidget * widget, const gchar * widget_name)
{
	GtkWidget *parent, *found_widget;

	for (;;) {
		if (GTK_IS_MENU(widget))
			parent =
			    gtk_menu_get_attach_widget(GTK_MENU(widget));
		else
			parent = widget->parent;
		if (parent == NULL)
			break;
		widget = parent;
	}

	found_widget = g_object_get_data(G_OBJECT(widget), widget_name);
	if (!found_widget)
		g_warning("Widget not found: %s", widget_name);
	return found_widget;
}

GdkPixbuf *load_pixbuf(const char *name)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/%s.png", GGZDATADIR, name);
	image = gdk_pixbuf_new_from_file(fullpath, &error);
	if (image == NULL) {
		ggz_error_msg("Can't load pixmap %s", fullpath);
		printf("Can't load pixmap %s.\n", fullpath);
	}
	g_free(fullpath);

	return image;
}

GdkPixbuf *load_svg_pixbuf(const char *name, int width, int height)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/%s.svg", GGZDATADIR, name);
	image = gdk_pixbuf_new_from_file_at_size(fullpath,
						 width, height,
						 &error);
	if (image == NULL) {
		ggz_error_msg("Can't load pixmap %s", fullpath);
		printf("Can't load SVG %s.\n", fullpath);
		g_free(fullpath);
		return load_pixbuf(name);
	}
	g_free(fullpath);

	return image;
}

char *nocasestrstr(char *text, char *tofind)
{
	char *ret = text, *find = tofind;

	while (1) {
		if (*find == 0)
			return ret;
		if (*text == 0)
			return 0;
		if (toupper(*find) != toupper(*text)) {
			ret = text + 1;
			find = tofind;
		} else
			find++;
		text++;
	}
}

void support_goto_url(gchar * url)
{
	char *command = NULL;
	char *browser_opt;
	char *lynx_opt;

	browser_opt =
	    ggzcore_conf_read_string("OPTIONS", "BROWSER", "None");

	if (!strcmp(browser_opt, "None")) {
		command = g_strdup(" ");
		props_create_or_raise();
	} else if (!strcmp(browser_opt, "Galeon - New")) {
		command = g_strdup_printf("galeon %s", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Galeon - Existing")) {
		command = g_strdup_printf("galeon -w %s", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Gnome URL Handler")) {
		command = g_strdup_printf("gnome-moz-remote %s", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Konqueror - New")) {
		command = g_strdup_printf("konqueror %s", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Konqueror - Existing")) {
		command =
		    g_strdup_printf("dcop konqueror default getWindows");
		support_exec(command);
		g_free(command);
		command =
		    g_strdup_printf
		    ("dcop konqueror konqueror-mainwindow#1 openURL %s",
		     url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Lynx")) {
		lynx_opt =
		    ggzcore_conf_read_string("OPTIONS", "LYNX", "xterm");
		command = g_strdup_printf("%s -e lynx %s", lynx_opt, url);
		ggz_free(lynx_opt);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Mozilla - New")) {
		command = g_strdup_printf("mozilla %s", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Mozilla - Existing")) {
		command =
		    g_strdup_printf("mozilla -remote 'openURL(%s)'", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Netscape - New")) {
		command = g_strdup_printf("netscape %s", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Netscape - Existing")) {
		command =
		    g_strdup_printf("netscape -remote 'openURL(%s)'", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Opera - New")) {
		command = g_strdup_printf("opera %s", url);
		support_exec(command);
	} else if (!strcmp(browser_opt, "Opera - Existing")) {
		command =
		    g_strdup_printf
		    ("opera -remote 'openURL(%s,new-window)'", url);
		support_exec(command);
	}

	ggz_free(browser_opt);

	g_free(command);
}


void support_exec(char *cmd)
{
	int pid;
	char **argv;
	int argc;

	if (my_poptParseArgvString(cmd, &argc, &argv) != 0)
		return;

	if ((pid = fork()) < 0) {
		return;
	} else if (pid == 0) {
		execvp(argv[0], argv);
		_exit(0);
	}
	free(argv);
}

/* Taken from XChat */

/* I think gnome1.0.x isn't necessarily linked against popt, ah well! */
/* !!! For now use this inlined function, or it would break fe-text building */
/* .... will find a better solution later. */
/*#ifndef USE_GNOME*/

/* this is taken from gnome-libs 1.2.4 */
#define POPT_ARGV_ARRAY_GROW_DELTA 5

static int my_poptParseArgvString(const char *s, int *argcPtr,
				  char ***argvPtr)
{
	char *buf, *bufStart, *dst;
	const char *src;
	char quote = '\0';
	int argvAlloced = POPT_ARGV_ARRAY_GROW_DELTA;
	char **argv = malloc(sizeof(*argv) * argvAlloced);
	const char **argv2;
	int argc = 0;
	int i, buflen;

	buflen = strlen(s) + 1;
/*    bufStart = buf = alloca(buflen);*/
	bufStart = buf = malloc(buflen);
	memset(buf, '\0', buflen);

	src = s;
	argv[argc] = buf;

	while (*src) {
		if (quote == *src) {
			quote = '\0';
		} else if (quote) {
			if (*src == '\\') {
				src++;
				if (!*src) {
					free(argv);
					free(bufStart);
					return 1;
				}
				if (*src != quote)
					*buf++ = '\\';
			}
			*buf++ = *src;
		} else if (isspace(*src)) {
			if (*argv[argc]) {
				buf++, argc++;
				if (argc == argvAlloced) {
					argvAlloced +=
					    POPT_ARGV_ARRAY_GROW_DELTA;
					argv =
					    realloc(argv,
						    sizeof(*argv) *
						    argvAlloced);
				}
				argv[argc] = buf;
			}
		} else
			switch (*src) {
			case '"':
			case '\'':
				quote = *src;
				break;
			case '\\':
				src++;
				if (!*src) {
					free(argv);
					free(bufStart);
					return 1;
				}
				/* fallthrough */
			default:
				*buf++ = *src;
			}

		src++;
	}

	if (strlen(argv[argc])) {
		argc++, buf++;
	}

	dst = malloc((argc + 1) * sizeof(*argv) + (buf - bufStart));
	argv2 = (void *)dst;
	dst += (argc + 1) * sizeof(*argv);
	memcpy(argv2, argv, argc * sizeof(*argv));
	argv2[argc] = NULL;
	memcpy(dst, bufStart, buf - bufStart);

	for (i = 0; i < argc; i++) {
		argv2[i] = dst + (argv[i] - bufStart);
	}

	free(argv);

	*argvPtr = (char **)argv2;	/* XXX don't change the API */
	*argcPtr = argc;

	free(bufStart);

	return 0;
}
