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

void motd_print_line(gchar *line);

extern GtkWidget *window1;
extern GtkTextBuffer *buffer, *motdbuffer;
GtkWidget *opendlg;
gint action;

/* Aray of GdkColors currently used for chat and MOTD */
/* They are all non-ditherable and as such should look the same everywhere */
/* These must corrispond with chat.c */
typedef struct
{
	char *name;
	GdkColor gdk;
} MotdColor;

MotdColor colors[] =
{
	{"black",          {0, 0x0000, 0x0000, 0x0000}},          /* 0   Black                      */
	{"dark goldenrod", {0, 0xFFFF, 0xFFFF, 0x3333}},          /* 1   Dark Goldenrod             */
	{"OrangeRed3",     {0, 0xCCCC, 0x0000, 0x0000}},          /* 2   Orange Red 3               */
	{"OliveDrab",      {0, 0x6666, 0x9999, 0x0000}},          /* 3   Olive Drab                 */
	{"MediumOrchid",   {0, 0xCCCC, 0x3333, 0xCCCC}},          /* 4   Medium Orchid              */
	{"IndianRed4",     {0, 0x9999, 0x3333, 0x3333}},          /* 5   Indian Red 4               */
	{"RoyalBlue2",     {0, 0x0000, 0x6666, 0xFFFF}},          /* 6   Royal Blue 2               */
	{"tan1",           {0, 0xFFFF, 0x9999, 0x3333}},          /* 7   Tan 1                      */
	{"DarkSlateGray3", {0, 0x6666, 0xCCCC, 0xCCCC}},          /* 8   Dark Slate Grey 3          */
	{"CadetBlue",      {0, 0x6666, 0xCCCC, 0xFFFF}},          /* 9   Cadet Blue                 */
	{"purple2",        {0, 0x9999, 0x3333, 0xFFFF}},          /* 10  Purple 2                   */
	{"VioletRed4",     {0, 0x9999, 0x0000, 0x6666}},          /* 11  Violet Red 4               */
	{"dark blue",      {0, 0x3333, 0x0000, 0x6666}},          /* 12  Dark Blue                  */
	{"IndianRed",      {0, 0x9999, 0x3333, 0x3333}},          /* 13  Indian Red                 */
	{"blue",           {0, 0x3333, 0x6666, 0xFFFF}},          /* 14  Blue                       */
	{"PaleVioletRed",  {0, 0xFFFF, 0x6666, 0x9999}},          /* 15  Pale Violet Red            */
	{"yellow3",        {0, 0xCCCC, 0xCCCC, 0x3333}},          /* 16  Yellow 3                   */
	{"aquamarine",     {0, 0x6666, 0xFFFF, 0xCCCC}},          /* 17  Aquamarine 2               */
	{"black",          {0, 0x0000, 0x0000, 0x0000}},          /* 18  foreground (Black)         */
	{"white",          {0, 0xFFFF, 0xFFFF, 0xFFFF}}           /* 19  background (White)         */
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
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(buffer, &end, -1);

	gtk_text_buffer_delete(buffer, &start, &end);

    motd_print_line(gtk_text_buffer_get_text(buffer, &start, &end, TRUE));
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
on_text1_changed                       (GtkTextBuffer    *editable,
                                        gpointer         user_data)
{
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
    gtk_text_buffer_get_iter_at_offset(buffer, &end, -1);

    motd_print_line(gtk_text_buffer_get_text(buffer, &start, &end, TRUE));
}


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

	temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text1");

	style = gtk_rc_style_new();
	gtk_widget_modify_style(GTK_WIDGET(temp_widget), style);
	gtk_rc_style_unref(style);
}

void motd_print_line(gchar *line)
{
        gchar *out;
        gint lindex=0;
        gint oindex=0;
		gint xindex=0;
        GtkWidget *temp_widget;
        /*GdkColormap *cmap;*/
        GdkFont *fixed_font;
        gint color_index=0; /* Black */
        gint letter;
		GtkTextIter iter;
		GtkTextIter start;
		GtkTextIter end;
		GtkTextTag *tag;

		gtk_text_buffer_get_iter_at_offset(motdbuffer, &start, 0);
		gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);

		gtk_text_buffer_delete(motdbuffer, &start, &end);

		gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, 0);

		out = malloc(strlen(line) * sizeof(char*));

		tag = gtk_text_buffer_create_tag(motdbuffer, NULL, "foreground",
			colors[0].name, NULL);

        temp_widget = gtk_object_get_data(GTK_OBJECT(window1), "text2");

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
										tag = gtk_text_buffer_create_tag(motdbuffer, NULL, "foreground",
											colors[color_index].name, NULL);
										gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
										gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
										gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
										gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
										iter = end;
                                        color_index=atoi(&line[lindex]);
                                        oindex=0;
                                        lindex++;
										xindex += strlen(out);
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
								gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
								gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
								iter = end;
                                oindex=0;
                                lindex++;
								xindex += strlen(out);
                        }else if (line[lindex] == 'h') {
				out[oindex] = 'f';
				out[oindex + 1] = 'o';
				out[oindex + 2] = 'o';
				out[oindex + 3] = '.';
				out[oindex + 4] = 'c';
				out[oindex + 5] = 'o';
				out[oindex + 6] = 'm';
				out[oindex + 7] = '\0';
								gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
								gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
								iter = end;
                                oindex=0;
                                lindex++;
								xindex += strlen(out);
                        }else if (line[lindex] == 't') {
				out[oindex] = 'T';
				out[oindex + 1] = 'I';
				out[oindex + 2] = 'M';
				out[oindex + 3] = 'E';
				out[oindex + 4] = '\0';
								gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
								gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
								iter = end;
                                oindex=0;
                                lindex++;
								xindex += strlen(out);
                        }else if (line[lindex] == 'd') {
				out[oindex] = 'D';
				out[oindex + 1] = 'A';
				out[oindex + 2] = 'T';
				out[oindex + 3] = 'E';
				out[oindex + 4] = '\0';
								gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
								gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
								iter = end;
                                oindex=0;
                                lindex++;
								xindex += strlen(out);
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
								gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
								gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
								iter = end;
                                oindex=0;
                                lindex++;
								xindex += strlen(out);
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
								gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
								gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
								gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
								iter = end;
                                oindex=0;
                                lindex++;
								xindex += strlen(out);
                        }else{
                                lindex--;
                        }
                }
                out[oindex]=line[lindex];
                lindex++;
                oindex++;
        }
        out[oindex]='\0';
		gtk_text_buffer_insert(motdbuffer, &iter, out, -1);
		gtk_text_buffer_get_iter_at_offset(motdbuffer, &iter, xindex);
		gtk_text_buffer_get_iter_at_offset(motdbuffer, &end, -1);
		gtk_text_buffer_apply_tag(motdbuffer, tag, &iter, &end);
		iter = end;
		xindex += strlen(out);

	free(out);
}


void
on_file_ok_button1_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	const gchar *filename;
	FILE *fd;
	gchar in[1024];
	gchar *text;
	GtkTextIter iter;
    GtkTextIter start;
    GtkTextIter end;

	gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
	gtk_text_buffer_get_iter_at_offset(buffer, &end, -1);

	if(action == 1)
	{
		
		filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(opendlg));
		fd = fopen(filename, "r");
		if(fd)
		{
			gtk_text_buffer_delete(buffer, &start, &end);
			gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);

			while(fgets(in, 1024, fd))
			{
				gtk_text_buffer_insert(buffer, &iter, in, -1);
			}
			fclose(fd);
			gtk_widget_destroy(opendlg);
	
			on_text1_changed(NULL, NULL);
		}
	} else if(action == 0) {
		filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(opendlg));
		fd = fopen(filename, "w");

		text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);

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

