#include <gtk/gtk.h>

GtkWidget* create_dlg_options (void);
GtkWidget* create_dlg_save (void);
void dlg_options_update(GtkWidget *);
void dlg_options_list_maps(GtkWidget *);

void load_button_clicked(GtkButton *, gpointer dialog);
void save_button_clicked(GtkButton *, gpointer dialog);
void save_map(GtkButton *, GtkWidget *);

void delete_button_clicked(GtkButton *button, gpointer dialog);

void delete_map(GtkButton *button, gpointer dialog);

void load_map(char *filename, GtkWidget *dialog);

combat_game *quick_load_map_on_struct(char *filename);

void maps_list_selected (GtkCList *clist, gint row, gint column,
	 											 GdkEventButton *event, gpointer user_data);


gboolean
mini_board_expose             (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
mini_board_configure           (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
mini_board_click         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void init_mini_board(GtkWidget *);
void draw_mini_board(GtkWidget *);

#define OPEN T_OPEN
#define LAKE T_LAKE
#define BLACK T_NULL
#define PLAYER_1 OWNER(0) + T_OPEN
#define PLAYER_2 OWNER(1) + T_OPEN
