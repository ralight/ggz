#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "support.h"
#include "dlg_options.h"

GdkPixmap *mini_buf;
extern GdkColor lake_color;
extern GdkColor open_color;
extern GdkColor *player_colors;


GtkWidget*
create_dlg_options (void)
{
  GtkWidget *dlg_options;
  GtkWidget *dialog_vbox2;
  GtkWidget *notebook1;
  GtkWidget *hbox3;
  GtkWidget *mini_board;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label7;
  GtkObject *width_adj;
  GtkWidget *width;
  GtkObject *height_adj;
  GtkWidget *height;
  GtkWidget *label6;
  GtkWidget *hseparator1;
  GSList *terrain_types_group = NULL;
  GtkWidget *open;
  GtkWidget *lake;
  GtkWidget *black;
  GtkWidget *player_1;
  GtkWidget *player_2;
  GtkWidget *label3;
  GtkWidget *hbox4;
  GtkWidget *label8;
  GtkWidget *label4;
  GtkWidget *sorry;
  GtkWidget *label5;
  GtkWidget *dialog_action_area2;
  GtkWidget *hbuttonbox2;
  GtkWidget *ok_button;
  GtkWidget *cancel_button;

  dlg_options = gtk_dialog_new ();
  gtk_widget_set_name (dlg_options, "dlg_options");
  gtk_object_set_data (GTK_OBJECT (dlg_options), "dlg_options", dlg_options);
  gtk_window_set_title (GTK_WINDOW (dlg_options), _("Game options"));
  gtk_window_set_modal (GTK_WINDOW (dlg_options), TRUE);
  gtk_window_set_policy (GTK_WINDOW (dlg_options), TRUE, TRUE, FALSE);

  dialog_vbox2 = GTK_DIALOG (dlg_options)->vbox;
  gtk_widget_set_name (dialog_vbox2, "dialog_vbox2");
  gtk_object_set_data (GTK_OBJECT (dlg_options), "dialog_vbox2", dialog_vbox2);
  gtk_widget_show (dialog_vbox2);

  notebook1 = gtk_notebook_new ();
  gtk_widget_set_name (notebook1, "notebook1");
  gtk_widget_ref (notebook1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "notebook1", notebook1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox2), notebook1, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox3, "hbox3");
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (notebook1), hbox3);

  mini_board = gtk_drawing_area_new ();
  gtk_widget_set_name (mini_board, "mini_board");
  gtk_widget_ref (mini_board);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "mini_board", mini_board,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mini_board);
  gtk_box_pack_start (GTK_BOX (hbox3), mini_board, FALSE, TRUE, 0);
  gtk_widget_set_usize (mini_board, 240, 240);
  gtk_widget_set_events (mini_board, GDK_BUTTON_PRESS_MASK);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox1, "vbox1");
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (hbox3), vbox1, FALSE, TRUE, 0);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_set_name (table1, "table1");
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, FALSE, TRUE, 0);

  label7 = gtk_label_new (_("Height: "));
  gtk_widget_set_name (label7, "label7");
  gtk_widget_ref (label7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  width_adj = gtk_adjustment_new (10, 1, 100, 1, 10, 10);
  width = gtk_spin_button_new (GTK_ADJUSTMENT (width_adj), 1, 0);
  gtk_widget_set_name (width, "width");
  gtk_widget_ref (width);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "width", width,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (width);
  gtk_table_attach (GTK_TABLE (table1), width, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  height_adj = gtk_adjustment_new (10, 1, 100, 1, 10, 10);
  height = gtk_spin_button_new (GTK_ADJUSTMENT (height_adj), 1, 0);
  gtk_widget_set_name (height, "height");
  gtk_widget_ref (height);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "height", height,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (height);
  gtk_table_attach (GTK_TABLE (table1), height, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label6 = gtk_label_new (_("Width: "));
  gtk_widget_set_name (label6, "label6");
  gtk_widget_ref (label6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_set_name (hseparator1, "hseparator1");
  gtk_widget_ref (hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hseparator1", hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, FALSE, TRUE, 7);

  open = gtk_radio_button_new_with_label (terrain_types_group, _("Open"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (open));
  gtk_widget_set_name (open, "open");
	gtk_object_set_data (GTK_OBJECT(open), "type", GINT_TO_POINTER(OPEN));
  gtk_widget_ref (open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "open", open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (open);
  gtk_box_pack_start (GTK_BOX (vbox1), open, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (open), TRUE);

  lake = gtk_radio_button_new_with_label (terrain_types_group, _("Lake"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (lake));
  gtk_widget_set_name (lake, "lake");
	gtk_object_set_data (GTK_OBJECT(lake), "type", GINT_TO_POINTER(LAKE));
  gtk_widget_ref (lake);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "lake", lake,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lake);
  gtk_box_pack_start (GTK_BOX (vbox1), lake, FALSE, FALSE, 0);

  black = gtk_radio_button_new_with_label (terrain_types_group, _("Black"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (black));
  gtk_widget_set_name (black, "black");
	gtk_object_set_data (GTK_OBJECT(black), "type", GINT_TO_POINTER(BLACK));
  gtk_widget_ref (black);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "black", black,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (black);
  gtk_box_pack_start (GTK_BOX (vbox1), black, FALSE, FALSE, 0);

  player_1 = gtk_radio_button_new_with_label (terrain_types_group, _("Player 1"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (player_1));
  gtk_widget_set_name (player_1, "player_1");
	gtk_object_set_data (GTK_OBJECT(player_1), "type", GINT_TO_POINTER(PLAYER_1));
  gtk_widget_ref (player_1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "player_1", player_1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_1);
  gtk_box_pack_start (GTK_BOX (vbox1), player_1, FALSE, FALSE, 0);

  player_2 = gtk_radio_button_new_with_label (terrain_types_group, _("Player 2"));
  terrain_types_group = gtk_radio_button_group (GTK_RADIO_BUTTON (player_2));
  gtk_widget_set_name (player_2, "player_2");
	gtk_object_set_data (GTK_OBJECT(player_2), "type", GINT_TO_POINTER(PLAYER_2));
  gtk_widget_ref (player_2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "player_2", player_2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_2);
  gtk_box_pack_start (GTK_BOX (vbox1), player_2, FALSE, FALSE, 0);

  label3 = gtk_label_new (_("Terrain"));
  gtk_widget_set_name (label3, "label3");
  gtk_widget_ref (label3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label3);

  hbox4 = gtk_hbox_new (TRUE, 0);
  gtk_widget_set_name (hbox4, "hbox4");
  gtk_widget_ref (hbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hbox4", hbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (notebook1), hbox4);

  label8 = gtk_label_new (_("No army options yet. sorry"));
  gtk_widget_set_name (label8, "label8");
  gtk_widget_ref (label8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox4), label8, FALSE, FALSE, 0);

  label4 = gtk_label_new (_("Army"));
  gtk_widget_set_name (label4, "label4");
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label4);

  sorry = gtk_label_new (_("No options yet! Sorry..."));
  gtk_widget_set_name (sorry, "sorry");
  gtk_widget_ref (sorry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "sorry", sorry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (sorry);
  gtk_container_add (GTK_CONTAINER (notebook1), sorry);

  label5 = gtk_label_new (_("Options"));
  gtk_widget_set_name (label5, "label5");
  gtk_widget_ref (label5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label5);

  dialog_action_area2 = GTK_DIALOG (dlg_options)->action_area;
  gtk_widget_set_name (dialog_action_area2, "dialog_action_area2");
  gtk_object_set_data (GTK_OBJECT (dlg_options), "dialog_action_area2", dialog_action_area2);
  gtk_widget_show (dialog_action_area2);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area2), 10);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_set_name (hbuttonbox2, "hbuttonbox2");
  gtk_widget_ref (hbuttonbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "hbuttonbox2", hbuttonbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (dialog_action_area2), hbuttonbox2, TRUE, TRUE, 0);

  ok_button = gtk_button_new_with_label (_("OK"));
  gtk_widget_set_name (ok_button, "ok_button");
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), ok_button);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_with_label (_("Cancel"));
  gtk_widget_set_name (cancel_button, "cancel_button");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_options), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), cancel_button);
  gtk_widget_set_sensitive (cancel_button, FALSE);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT (mini_board), "expose_event",
										 GTK_SIGNAL_FUNC (mini_board_expose), dlg_options);
	gtk_signal_connect(GTK_OBJECT (mini_board), "configure_event",
										 GTK_SIGNAL_FUNC (mini_board_configure), dlg_options);
	gtk_signal_connect(GTK_OBJECT (mini_board), "button_press_event",
									   GTK_SIGNAL_FUNC (mini_board_click), dlg_options);
	gtk_signal_connect_object(GTK_OBJECT (width), "changed",
										 GTK_SIGNAL_FUNC (dlg_options_update), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object_after(GTK_OBJECT (width), "changed",
										 GTK_SIGNAL_FUNC (init_mini_board), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object(GTK_OBJECT (height), "changed",
										 GTK_SIGNAL_FUNC (dlg_options_update), GTK_OBJECT (dlg_options));
	gtk_signal_connect_object_after(GTK_OBJECT (height), "changed",
										 GTK_SIGNAL_FUNC (init_mini_board), GTK_OBJECT (dlg_options));

	dlg_options_update(dlg_options);

  gtk_widget_grab_default (ok_button);
  return dlg_options;
}

void dlg_options_update(GtkWidget *dlg_options) {
	GtkWidget *width, *height;
	int width_v, height_v;

	// Gets width / height
	width = lookup_widget(dlg_options, "width");
	height = lookup_widget(dlg_options, "height");
	width_v = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(width));
	height_v = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(height));
	
	// Sets data
	gtk_object_set_data(GTK_OBJECT(dlg_options), "width_v", GINT_TO_POINTER(width_v));
	gtk_object_set_data(GTK_OBJECT(dlg_options), "height_v", GINT_TO_POINTER(height_v));

}

gboolean mini_board_expose               (GtkWidget       *widget, GdkEventExpose  *event, gpointer         user_data) {
	gdk_draw_pixmap( widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], mini_buf, event->area.x, event->area.y, event->area.x, event->area.y,
			event->area.width, event->area.height);
	return 1;
}

gboolean mini_board_configure            (GtkWidget       *widget, GdkEventConfigure *event, gpointer         user_data) {
	init_mini_board(user_data);
	return 1;
}

gboolean mini_board_click         (GtkWidget       *widget, GdkEventButton  *event, gpointer         user_data) {
	int width, height, width_v, height_v, pix_width, pix_height, x, y;
	char *map_data = gtk_object_get_data(GTK_OBJECT(user_data), "map_data");
	GSList *radio_button;
	int current = OPEN;

	gdk_window_get_size(widget->window, &width, &height);

	width_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(user_data), "width_v"));
	height_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(user_data), "height_v"));

	pix_width = width/width_v;
	pix_height = width/height_v;

	x = event->x/(pix_width);
	y = event->y/(pix_height);

	radio_button = gtk_radio_button_group(GTK_RADIO_BUTTON(lookup_widget(user_data, "open")));

	while (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_button->data))) {
		radio_button = radio_button->next;
		current++;
		if (radio_button == NULL) {
			game_status("Big bad error!");
			return 0;
		}
	}

	current = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(radio_button->data), "type"));

	map_data[y*width_v+x] = current;

	draw_mini_board(user_data);

	return 1;
}

void init_mini_board(GtkWidget *dlg_options) {
	int width, height, a;
	GtkWidget *widget = lookup_widget(dlg_options, "mini_board");
	char *map_data;

	if (widget == NULL) {
		game_status("Can't find widget!");
		return;
	}

	// Init the pixmap
	gdk_window_get_size(widget->window, &width, &height);
	printf("W: %d, H: %d\n", width, height);
	if (mini_buf)
		gdk_pixmap_unref(mini_buf);
	mini_buf = gdk_pixmap_new( widget->window, width, height, -1 );

	// Now init the data
	width = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "width_v"));
	height = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "height_v"));
	map_data = (char *)malloc(sizeof(char) * width * height + 1);
	for (a = 0; a < width * height; a++)
		map_data[a] = OPEN;
	gtk_object_set_data(GTK_OBJECT(dlg_options), "map_data", map_data);

	draw_mini_board(dlg_options);

}

void draw_mini_board(GtkWidget *dlg_options) {
	GtkWidget *widget = lookup_widget(dlg_options, "mini_board");
	int width, height, width_v, height_v, pix_width, pix_height, i, j;
	char *map_data = gtk_object_get_data(GTK_OBJECT(dlg_options), "map_data");
	GdkGC *solid_gc;

	solid_gc = gdk_gc_new(widget->window);

	gdk_window_get_size(widget->window, &width, &height);

	width_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "width_v"));
	height_v = GPOINTER_TO_INT (gtk_object_get_data(GTK_OBJECT(dlg_options), "height_v"));

	// Draw background
	gdk_draw_rectangle (mini_buf,
			dlg_options->style->white_gc, TRUE, 0, 0, width, height);

	// Gets the size of each square
	pix_width = width/width_v;
	pix_height = height/height_v;

	// Draw terrain
	for (j = 0; j < height_v; j++) {
		for (i = 0; i < width_v; i++) {
			switch (map_data[j*width_v+i]) {
				case (OPEN):
					gdk_gc_set_foreground(solid_gc, &open_color);
					break;
				case (LAKE):
					gdk_gc_set_foreground(solid_gc, &lake_color);
					break;
				case (BLACK):
					gdk_gc_set_foreground(solid_gc, &widget->style->black);
					break;
				case (PLAYER_1):
					gdk_gc_set_foreground(solid_gc, &player_colors[0]);
					break;
				case (PLAYER_2):
					gdk_gc_set_foreground(solid_gc, &player_colors[1]);
					break;
			}
			gdk_draw_rectangle( mini_buf,
					solid_gc,
					TRUE,
					i*pix_width, j*pix_height,
					pix_width, pix_height);
		}
	}


	// Draw lines
	for (i = 0; i < width_v; i++) {
		gdk_draw_line(mini_buf, widget->style->black_gc,
				i*pix_width, 0, i*pix_width, height);
	}

	for (i = 0; i < height_v; i++) {
		gdk_draw_line(mini_buf, widget->style->black_gc,
				0, i*pix_height, width, i*pix_height);
	}

	gtk_widget_draw(widget, NULL);


}
	

