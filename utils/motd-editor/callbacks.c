#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

void motd_print_line(gchar *line);

extern GtkWidget *window1;
GtkWidget *opendlg;
gint action;

/* Aray of GdkColors currently used for chat and MOTD */
/* They are all non-ditherable and as such should look the same everywhere */
/* These must corrispond with chat.c */
GdkColor colors[] =
{
        {0, 0x0000, 0x0000, 0x0000},          /* 0   Black                      */
        {0, 0xFFFF, 0xFFFF, 0x3333},          /* 1   Dark Goldenrod             */
        {0, 0xCCCC, 0x0000, 0x0000},          /* 2   Orange Red 3               */
        {0, 0x6666, 0x9999, 0x0000},          /* 3   Olive Drab                 */
        {0, 0xCCCC, 0x3333, 0xCCCC},          /* 4   Medium Orchid              */
        {0, 0x9999, 0x3333, 0x3333},          /* 5   Indian Red 4               */
        {0, 0x0000, 0x6666, 0xFFFF},          /* 6   Royal Blue 2               */
        {0, 0xFFFF, 0x9999, 0x3333},          /* 7   Tan 1                      */
        {0, 0x6666, 0xCCCC, 0xCCCC},          /* 8   Dark Slate Grey 3          */
        {0, 0x6666, 0xCCCC, 0xFFFF},          /* 9   Cadet Blue                 */
        {0, 0x9999, 0x3333, 0xFFFF},          /* 10  Purple 2                   */
        {0, 0x9999, 0x0000, 0x6666},          /* 11  Violet Red 4               */
        {0, 0x3333, 0x0000, 0x6666},          /* 12  Dark Blue                  */
        {0, 0x9999, 0x3333, 0x3333},          /* 13  Indian Red                 */
        {0, 0x3333, 0x6666, 0xFFFF},          /* 14  Blue                       */
        {0, 0xFFFF, 0x6666, 0x9999},          /* 15  Pale Violet Red            */
        {0, 0xCCCC, 0xCCCC, 0x3333},          /* 16  Yellow 3                   */
        {0, 0x6666, 0xFFFF, 0xCCCC},          /* 17  Aquamarine 2               */
        {0, 0x0000, 0x0000, 0x0000},          /* 18  foreground (Black)         */
        {0, 0xFFFF, 0xFFFF, 0xFFFF}           /* 19  background (White)         */
};

void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *temp_widget;
	gchar *text;

        temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text1");
	gtk_text_set_point(GTK_TEXT(temp_widget), 0);
	gtk_text_forward_delete(GTK_TEXT(temp_widget),
		gtk_text_get_length(GTK_TEXT(temp_widget)));
	text = gtk_editable_get_chars(GTK_EDITABLE(temp_widget), 0,
				      gtk_text_get_length(GTK_TEXT(temp_widget)));
	motd_print_line(text);
}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	action = 1;
	opendlg = create_fileselection1();
	gtk_widget_show(opendlg);
}

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	action = 0;
	opendlg = create_fileselection1();
	gtk_widget_show(opendlg);
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_revert1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_select_all1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_undo2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_redo2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_text1_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkWidget *temp_widget;
	gchar *text;

	temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text1");

	/* Get whats there */
	text = gtk_editable_get_chars(GTK_EDITABLE(temp_widget), 0,
				      gtk_text_get_length(GTK_TEXT(temp_widget)));

	/* Draw new */
	motd_print_line(text);
}


#if 0
void
on_text1_move_cursor                   (GtkEditable     *editable,
                                        gint             x,
                                        gint             y,
                                        gpointer         user_data)
{

}
#endif


void
on_cut2_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copy2_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_paste3_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_black1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_dark_goldenrod1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_orange_red_1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_olive_drab1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_medium_orchid1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_indian_red_1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_light_blue1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_yellow_brown1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_dark_slate_grey_1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_cadet_blue1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_window1_realize                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
	GtkWidget *temp_widget;
	GtkRcStyle *style;
        gint i;

        /* Allocate standared colors */
        if (!colors[0].pixel)        /* don't do it again */
        {
                for (i = 0; i < 20; i++)
                {
                        colors[i].pixel = (gulong) ((colors[i].red & 0xff00) * 256 +
                                        (colors[i].green & 0xff00) +
                                        (colors[i].blue & 0xff00) / 256);
                        if (!gdk_color_alloc (gdk_colormap_get_system(),
                            &colors[i]))
                                g_error("*** GGZ: Couldn't alloc color\n");
                }
        }

	temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text1");

	style = gtk_rc_style_new();
	style->font_name = g_strdup("-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1");
	gtk_widget_modify_style(GTK_WIDGET(temp_widget), style);
	gtk_rc_style_unref(style);
	gtk_text_set_word_wrap(GTK_TEXT(temp_widget), TRUE);


	temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text2");
	gtk_text_set_word_wrap(GTK_TEXT(temp_widget), TRUE);

}

void motd_print_line(gchar *line)
{
        gchar *out;
        gint lindex=0;
        gint oindex=0;
        GtkWidget *temp_widget;
        GdkColormap *cmap;
        GdkFont *fixed_font;
        gint color_index=0; /* Black */
        gint letter;

	out = malloc(strlen(line) * sizeof(char*));

        cmap = gdk_colormap_get_system();
        if (!gdk_color_alloc(cmap, &colors[color_index])) {
                g_error("couldn't allocate color");
        }

        temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text2");
        gtk_text_freeze(GTK_TEXT(temp_widget));

	/* Clear out what is currently there */
	gtk_text_set_point(GTK_TEXT(temp_widget), 0);
	gtk_text_forward_delete(GTK_TEXT(temp_widget),
		gtk_text_get_length(GTK_TEXT(temp_widget)));

        fixed_font = gdk_font_load ("-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1");
        while(line[lindex] != '\0')
        {
                if (line[lindex] == '%')
                {
                        lindex++;
                        if (line[lindex] == 'c')
                        {
                                lindex++;
                                letter = atoi(&line[lindex]);
                                if ((letter>=0) && (letter<=9))
                                {
                                        out[oindex]='\0';
                                        gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                                                                &colors[color_index], NULL, out, -1);
                                        color_index=atoi(&line[lindex]);
                                        cmap = gdk_colormap_get_system();
                                        if (!gdk_color_alloc(cmap, &colors[color_index])) {
                                                g_error("couldn't allocate color");
                                        }
                                        oindex=0;
                                        lindex++;
                                }else {
                                        lindex--;
                                        lindex--;
                                }
                        }else if (line[lindex] == 'p') {
				out[oindex] = '5';
				out[oindex + 1] = '6';
				out[oindex + 2] = '8';
				out[oindex + 3] = '8';
				out[oindex + 4] = '\0';
                                gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                                                        &colors[color_index], NULL, out, -1);
                                oindex=0;
                                lindex++;
                        }else if (line[lindex] == 'h') {
				out[oindex] = 'f';
				out[oindex + 1] = 'o';
				out[oindex + 2] = 'o';
				out[oindex + 3] = '.';
				out[oindex + 4] = 'c';
				out[oindex + 5] = 'o';
				out[oindex + 6] = 'm';
				out[oindex + 7] = '\0';
                                gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                                                        &colors[color_index], NULL, out, -1);
                                oindex=0;
                                lindex++;
                        }else if (line[lindex] == 't') {
				out[oindex] = 'T';
				out[oindex + 1] = 'I';
				out[oindex + 2] = 'M';
				out[oindex + 3] = 'E';
				out[oindex + 4] = '\0';
                                gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                                                        &colors[color_index], NULL, out, -1);
                                oindex=0;
                                lindex++;
                        }else if (line[lindex] == 'd') {
				out[oindex] = 'D';
				out[oindex + 1] = 'A';
				out[oindex + 2] = 'T';
				out[oindex + 3] = 'E';
				out[oindex + 4] = '\0';
                                gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                                                        &colors[color_index], NULL, out, -1);
                                oindex=0;
                                lindex++;
                        }else if (line[lindex] == 'a') {
				out[oindex] = 'J';
				out[oindex + 1] = 'o';
				out[oindex + 2] = 'h';
				out[oindex + 3] = 'n';
				out[oindex + 4] = ' ';
				out[oindex + 5] = 'D';
				out[oindex + 6] = 'o';
				out[oindex + 7] = 'e';
				out[oindex + 8] = '\0';
                                gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                                                        &colors[color_index], NULL, out, -1);
                                oindex=0;
                                lindex++;
                        }else if (line[lindex] == 'e') {
				out[oindex] = 'J';
				out[oindex + 1] = 'D';
				out[oindex + 2] = 'o';
				out[oindex + 3] = 'e';
				out[oindex + 4] = '@';
				out[oindex + 5] = 'f';
				out[oindex + 6] = 'o';
				out[oindex + 7] = 'o';
				out[oindex + 8] = '.';
				out[oindex + 9] = 'c';
				out[oindex + 10] = 'o';
				out[oindex + 11] = 'm';
				out[oindex + 12] = '\0';
                                gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                                                        &colors[color_index], NULL, out, -1);
                                oindex=0;
                                lindex++;
                        }else{
                                lindex--;
                        }
                }
                out[oindex]=line[lindex];
                lindex++;
                oindex++;
        }
        out[oindex]='\0';
        gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                        &colors[color_index], NULL, out, -1);

        gtk_text_thaw(GTK_TEXT(temp_widget));  
	free(out);
}


void
on_file_ok_button1_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	gchar *filename;
	FILE *fd;
	gchar in[1024];
	GdkFont *fixed_font;
	GtkWidget *temp_widget;
	gchar *text;
	
	if(action == 1)
	{
	        temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text1");
        	fixed_font = gdk_font_load ("-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1");
		gtk_text_set_point(GTK_TEXT(temp_widget), 0);
		gtk_text_forward_delete(GTK_TEXT(temp_widget),
			gtk_text_get_length(GTK_TEXT(temp_widget)));

		filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(opendlg));
		fd = fopen(filename, "r");

		while(fgets(in, 1024, fd))
		{
        		gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
                		        &colors[0], NULL, in, -1);
		}
		fclose(fd);
		gtk_widget_destroy(opendlg);
	
		text = gtk_editable_get_chars(GTK_EDITABLE(temp_widget), 0,
					      gtk_text_get_length(GTK_TEXT(temp_widget)));
		motd_print_line(text);

	        temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text1");
	} else if(action == 0) {
	        temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text1");
		filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(opendlg));
		fd = fopen(filename, "w");
		text = gtk_editable_get_chars(GTK_EDITABLE(temp_widget), 0,
					      gtk_text_get_length(GTK_TEXT(temp_widget)));

		fputs(text, fd);
		fclose(fd);
		gtk_widget_destroy(opendlg);
	}
}


void
on_file_cancel_button1_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(opendlg);
}

