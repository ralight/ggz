/*
 * File: support.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: support.c 6272 2004-11-05 21:19:52Z jdorje $
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

/* This is an internally used function to check if a pixmap file exists. */
static gchar* check_file_exists        (const gchar     *directory,
                                        const gchar     *filename);

/* This is an internally used function to create pixmaps. */
static GtkWidget* create_dummy_pixmap  (GtkWidget       *widget);

/* This is an internally used parse commands */
static int my_poptParseArgvString(const char * s, int * argcPtr, char *** argvPtr);

GtkWidget*
lookup_widget                          (GtkWidget       *widget,
                                        const gchar     *widget_name)
{
  GtkWidget *parent, *found_widget;

  for (;;)
    {
      if (GTK_IS_MENU (widget))
        parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
      else
        parent = widget->parent;
      if (parent == NULL)
        break;
      widget = parent;
    }

  found_widget = g_object_get_data(G_OBJECT(widget), widget_name);
  if (!found_widget)
    g_warning ("Widget not found: %s", widget_name);
  return found_widget;
}

/* XPM  --  From GNOME */
static char * unknown_xpm[] = {
"48 48 73 1",
" 	c None",
".	c #000000",
"+	c #A27F3B",
"@	c #B89243",
"#	c #D8AB4E",
"$	c #9D7D3A",
"%	c #A6823C",
"&	c #7C622D",
"*	c #F7D899",
"=	c #F7E2B8",
"-	c #D8BA7B",
";	c #BE9B53",
">	c #F7C35A",
",	c #F7DFAF",
"'	c #F7E5C0",
")	c #F7D48C",
"!	c #F7CF7E",
"~	c #D8B15F",
"{	c #6E5526",
"]	c #F7DCA5",
"^	c #A8843D",
"/	c #937435",
"(	c #AA863E",
"_	c #D8B66E",
":	c #BEA363",
"<	c #947536",
"[	c #927335",
"}	c #F7C96D",
"|	c #80632D",
"1	c #C39F57",
"2	c #A38E55",
"3	c #A48B5A",
"4	c #D2A64D",
"5	c #82662F",
"6	c #654F24",
"7	c #DCB463",
"8	c #BE9544",
"9	c #C3A466",
"0	c #8C6D31",
"a	c #D2B06C",
"b	c #D8BE86",
"c	c #715929",
"d	c #B99344",
"e	c #B38D40",
"f	c #D8B56E",
"g	c #D8AC58",
"h	c #997A3B",
"i	c #6B5527",
"j	c #C39F55",
"k	c #DCC494",
"l	c #DAB872",
"m	c #C9AC73",
"n	c #BFA16A",
"o	c #19150D",
"p	c #876A31",
"q	c #C9A45B",
"r	c #C6A76A",
"s	c #7F6941",
"t	c #977737",
"u	c #D8B165",
"v	c #D8AC5B",
"w	c #BEA272",
"x	c #C09745",
"y	c #D8B163",
"z	c #927C52",
"A	c #735F3B",
"B	c #A68748",
"C	c #1A1A1A",
"D	c #393939",
"E	c #221C12",
"F	c #DDBC74",
"G	c #D2A64C",
"H	c #705627",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                   ........                     ",
"                 ...+@#$%&...                   ",
"                ..#**=*---;%.                   ",
"               ..>,'*)>>>!-~%{..                ",
"               .>,]>^//(>>!_~%..                ",
"              .:*=><....[>>}~~|..               ",
"              .}'!<..   .1>!#~%..               ",
"             .2),#..    .3>}~#%..               ",
"             .4!>%..    .3}}_#5..               ",
"             .>!>%..    .3>__#6..               ",
"             .7~85..    .9}-#%...               ",
"             .......    0ab~#c..                ",
"              .....    .d)b#8...                ",
"                      .e}b##...                 ",
"                     ..>)fgh..                  ",
"                     ijklmno.                   ",
"                    .pafqrs.                    ",
"                    .t-uvw..                    ",
"                    .x-yvz.                     ",
"                    .#b~#A.                     ",
"                    .;-#8..                     ",
"                    .BB%|..                     ",
"                    .CD...                      ",
"                     ....                       ",
"                                                ",
"                     E....                      ",
"                    .F!)8..                     ",
"                    .!**#..                     ",
"                    .))~8..                     ",
"                    .)_#8..                     ",
"                    .G~8H..                     ",
"                     ......                     ",
"                      ....                      ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                ",
"                                                "};

/* This is an internally used function to create pixmaps. */
static GtkWidget*
create_dummy_pixmap                    (GtkWidget       *widget)
{
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask,
                                                     NULL, unknown_xpm);
  if (gdkpixmap == NULL)
    g_error ("Couldn't create replacement pixmap.");
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
  gdk_pixmap_unref (gdkpixmap);
  gdk_bitmap_unref (mask);
  return pixmap;
}

static GList *pixmaps_directories = NULL;

/* Use this function to set the directory containing installed pixmaps. */
void
add_pixmap_directory                   (const gchar     *directory)
{
  pixmaps_directories = g_list_prepend (pixmaps_directories,
                                        g_strdup (directory));
}

/* This is an internally used function to create pixmaps. */
GtkWidget*
create_pixmap                          (GtkWidget       *widget,
                                        const gchar     *filename)
{
  gchar *found_filename = NULL;
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;
  GList *elem;

  /* Check for a NULL */
  if (filename == NULL)
  {
      return create_dummy_pixmap (widget);
  }

  /* We first try any pixmaps directories set by the application. */
  elem = pixmaps_directories;
  while (elem)
    {
      found_filename = check_file_exists ((gchar*)elem->data, filename);
      if (found_filename)
        break;
      elem = elem->next;
    }

  /* If we haven't found the pixmap, try the source directory. */
  if (!found_filename)
    {
      found_filename = check_file_exists ("../pixmaps", filename);
    }

  if (!found_filename)
    {
      g_warning (_("Couldn't find pixmap file: %s"), filename);
      return create_dummy_pixmap (widget);
    }

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm (NULL, colormap, &mask,
                                                   NULL, found_filename);
  if (gdkpixmap == NULL)
    {
      g_warning (_("Error loading pixmap file: %s"), found_filename);
      g_free (found_filename);
      return create_dummy_pixmap (widget);
    }
  g_free (found_filename);
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
  gdk_pixmap_unref (gdkpixmap);
  gdk_bitmap_unref (mask);
  return pixmap;
}

GdkPixbuf *load_pixbuf(const char *name)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/%s.png", GGZDATADIR, name);
	image = gdk_pixbuf_new_from_file(fullpath, &error);
	if(image == NULL) {
		ggz_error_msg("Can't load pixmap %s", fullpath);
		printf("Can't load pixmap %s.\n", fullpath);
	}
	g_free(fullpath);

	return image;
}

GdkPixmap *load_pixmap(const char *name, GdkBitmap **mask)
{
	GdkPixbuf *image = load_pixbuf(name);
	GdkPixmap *pixmap;

	*mask = NULL;
	if (!image) {
		return NULL;
	}

	gdk_pixbuf_render_pixmap_and_mask(image, &pixmap, mask, 1);
	g_object_unref(image);

	return pixmap;
}

/* This is an internally used function to check if a pixmap file exists. */
gchar*
check_file_exists                      (const gchar     *directory,
                                        const gchar     *filename)
{
  gchar *full_filename;
  struct stat s;
  gint status;

  full_filename = (gchar*) g_malloc (strlen (directory) + 1
                                     + strlen (filename) + 1);
  strcpy (full_filename, directory);
  strcat (full_filename, G_DIR_SEPARATOR_S);
  strcat (full_filename, filename);

  status = stat (full_filename, &s);
  if (status == 0 && S_ISREG (s.st_mode))
    return full_filename;
  g_free (full_filename);
  return NULL;
}

char *nocasestrstr (char *text, char *tofind)
{
   char *ret = text, *find = tofind;

   while (1)
   {
      if (*find == 0)
         return ret;
      if (*text == 0)
         return 0;
      if (toupper (*find) != toupper (*text))
      {
         ret = text + 1;
         find = tofind;
      } else
         find++;
      text++;
   }
}

void support_goto_url(gchar *url)
{
	char *command = NULL;
	char *browser_opt;
	char *lynx_opt;

	browser_opt = ggzcore_conf_read_string("OPTIONS", "BROWSER", "None");

        if(!strcmp(browser_opt, "None")) {
		command = g_strdup(" ");
		props_create_or_raise();
	}
	else if(!strcmp(browser_opt, "Galeon - New")) {
		command = g_strdup_printf("galeon %s", url);
		support_exec(command);
	}
	else if(!strcmp(browser_opt, "Galeon - Existing")) {
		command = g_strdup_printf("galeon -w %s", url);
		support_exec(command);
	}
	else if(!strcmp(browser_opt, "Gnome URL Handler")) {
		command = g_strdup_printf("gnome-moz-remote %s", url);
		support_exec(command);
	}
	else if(!strcmp(browser_opt, "Konqueror - New")) {
		command = g_strdup_printf("konqueror %s", url);
		support_exec(command);
	}
	else if(!strcmp(browser_opt, "Konqueror - Existing")) {
		command = g_strdup_printf("dcop konqueror default getWindows");
		support_exec(command);
		g_free(command);
		command = g_strdup_printf("dcop konqueror konqueror-mainwindow#1 openURL %s", url);
		support_exec(command);
	}
	else if(!strcmp(browser_opt, "Lynx")) {
		lynx_opt = ggzcore_conf_read_string("OPTIONS", "LYNX", "xterm");
		command = g_strdup_printf("%s -e lynx %s", lynx_opt, url);
		ggz_free(lynx_opt);
		support_exec(command);
	}
	else if(!strcmp(browser_opt, "Mozilla - New")) {
		command = g_strdup_printf("mozilla %s", url);
		support_exec(command);
	}
	else if(!strcmp(browser_opt, "Mozilla - Existing")) {
		command = g_strdup_printf("mozilla -remote 'openURL(%s)'", url);
		support_exec(command);
	}
        else if(!strcmp(browser_opt, "Netscape - New")) {
		command = g_strdup_printf("netscape %s", url);
		support_exec(command);
	}
        else if(!strcmp(browser_opt, "Netscape - Existing")) {
		command = g_strdup_printf("netscape -remote 'openURL(%s)'", url);
		support_exec(command);
	}
        else if(!strcmp(browser_opt, "Opera - New")) {
		command = g_strdup_printf("opera %s", url);
		support_exec(command);
	}
        else if(!strcmp(browser_opt, "Opera - Existing")) {
		command = g_strdup_printf("opera -remote 'openURL(%s,new-window)'", url);
		support_exec(command);
	}

	ggz_free(browser_opt);

	g_free(command);
}


void support_exec (char *cmd)
{
	int pid;
	char **argv;
	int argc;

	if (my_poptParseArgvString (cmd, &argc, &argv) != 0)
		return;

        if ( (pid = fork()) < 0) {
                return;
        } else if(pid == 0) {
		execvp (argv[0], argv);
                _exit(0);
	} 
	free (argv);
}

/* Taken from XChat */

/* I think gnome1.0.x isn't necessarily linked against popt, ah well! */
/* !!! For now use this inlined function, or it would break fe-text building */
/* .... will find a better solution later. */
/*#ifndef USE_GNOME*/

/* this is taken from gnome-libs 1.2.4 */
#define POPT_ARGV_ARRAY_GROW_DELTA 5

static int my_poptParseArgvString(const char * s, int * argcPtr, char *** argvPtr) {
    char * buf, * bufStart, * dst;
    const char * src;
    char quote = '\0';
    int argvAlloced = POPT_ARGV_ARRAY_GROW_DELTA;
    char ** argv = malloc(sizeof(*argv) * argvAlloced);
    const char ** argv2;
    int argc = 0;
    int i, buflen;

    buflen = strlen(s) + 1;
/*    bufStart = buf = alloca(buflen);*/
	 bufStart = buf = malloc (buflen);
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
		if (*src != quote) *buf++ = '\\';
	    }
	    *buf++ = *src;
	} else if (isspace(*src)) {
	    if (*argv[argc]) {
		buf++, argc++;
		if (argc == argvAlloced) {
		    argvAlloced += POPT_ARGV_ARRAY_GROW_DELTA;
		    argv = realloc(argv, sizeof(*argv) * argvAlloced);
		}
		argv[argc] = buf;
	    }
	} else switch (*src) {
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
    argv2 = (void *) dst;
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

	 free (bufStart);

    return 0;
}

