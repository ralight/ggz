#include <gtk/gtk.h>
#include <dlg_exit.h>


void
InputOptions                           (GtkButton       *button,
                                        gpointer         user_data);

void
StartSession                           (GtkButton       *button,
                                        gpointer         user_data);

void
show_details                           (GtkButton       *button,
                                        gpointer         user_data);

void
join_game                              (GtkButton       *button,
                                        gpointer         user_data);

void
get_game_options                       (GtkButton       *button,
                                        gpointer         user_data);

void
cancel_details                         (GtkButton       *button,
                                        gpointer         user_data);

void
logout                                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
get_types                              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
get_players                            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
get_tables                             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
fill_defaults                          (GtkWidget       *widget,
                                        gpointer         user_data);
