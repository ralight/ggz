#include <gtk/gtk.h>

GtkWidget* create_dlg_options (void);
GtkWidget* create_dlg_save (void);
void dlg_options_update(GtkWidget *);


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

#define OPEN 0
#define LAKE 1
#define BLACK 2
#define PLAYER_1 3
#define PLAYER_2 4
