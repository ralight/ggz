#include "../config.h"
#include "game.h"
#include "ships.h"

#ifndef NOGTK
GtkWidget *gwindow;
GtkWidget *ship_combo;
GtkWidget *cheat_entry;

void end_gtk ()
{
	gtk_widget_destroy (gwindow);
	gtk_main_quit ();
}

void cheat_ok ()
{
	char *cheat = gtk_entry_get_text (GTK_ENTRY(cheat_entry));
	
	if (strncmp (cheat, "ahnowiremember", 14) == 0)
	{
		#ifdef CHEAT_DEBUG
		fprintf (stderr, "Credit cheat activated !\n");
		#endif
		show_credits = TRUE;
	}
	if (strncmp (cheat, "twinklelittlestar", 17) == 0)
	{
		#ifdef CHEAT_DEBUG
		fprintf (stderr, "Deathstar cheat activated !\n");
		#endif
		littlestar = TRUE;
	}
	if (strncmp (cheat, "whatapieceofjunk", 16) == 0)
	{
		#ifdef CHEAT_DEBUG
		fprintf (stderr, "Dash Rendar's Outrider is yours !\n");
		#endif
		outride = TRUE;
	}
	if (strncmp (cheat, "lockpick", 8) == 0)
	{
		#ifdef CHEAT_DEBUG
		fprintf (stderr, "This isn't a good idea...\n");
		#endif
		invul = TRUE;
	}

	if (invul == TRUE)
		printf (cheat);
	gtk_entry_set_text (GTK_ENTRY(cheat_entry), "");
}

void opts_ok ()
{
	char *which = gtk_entry_get_text (GTK_ENTRY(GTK_COMBO
				(ship_combo)->entry));

	if (strncmp (which, "X-Wing", 6) == 0)
		pdef = XWING;
	else 
		if (strncmp (which, "A-Wing", 6) == 0)
	     		pdef = AWING;
	     else 
		if (strncmp (which, "Naboo Fighter", 13) == 0)
			pdef = NABOO;
		else
			pdef = XWING;

	set_ship (pdef);
	end_gtk ();
}

void optionscreen ()
{
	int n;
	char *stuff = (char *) malloc (200);
	GList *ships = NULL;
	GtkWidget *ok_btn;
	GtkWidget *nok_btn;
	GtkWidget *box1;
	GtkWidget *box2;
	GtkWidget *ship_label;
	GtkWidget *ship_box;
	GtkWidget *cheat_label;
	GtkWidget *cheat_box;
	GtkWidget *cheat_button;
	GtkWidget *hai_text;
	GtkWidget *hai_label;
	
	gwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	ok_btn = gtk_button_new_with_label ("OK");
	nok_btn = gtk_button_new_with_label ("Cancel");
	box1 = gtk_hbox_new (FALSE, 0);
	box2 = gtk_vbox_new (FALSE, 0);
	ship_label = gtk_label_new ("Select your ship:");
	ship_combo = gtk_combo_new ();
	ship_box = gtk_hbox_new (FALSE, 0);
	cheat_label = gtk_label_new ("Enter cheat code:");
	cheat_entry = gtk_entry_new ();
	cheat_button = gtk_button_new_with_label ("OK");
	cheat_box = gtk_hbox_new (FALSE, 0);
	hai_text = gtk_text_new (NULL, NULL);
	hai_label = gtk_label_new ("hIgHsc0rEZ");
	
	ships = g_list_append (ships, "X-Wing");
	ships = g_list_append (ships, "A-Wing");
	ships = g_list_append (ships, "Naboo Fighter");
	gtk_combo_set_popdown_strings (GTK_COMBO(ship_combo), ships);
	gtk_editable_set_editable (GTK_EDITABLE (hai_text), FALSE);
	
	for (n = 1; n < 11; n++)
	{
		if (n < 10)
			sprintf (stuff, "0%d.\t%s\t\t\t%d\n", n, 
					scorelist[n-1].name,  
					scorelist[n-1].score);
		else
			sprintf (stuff, "%d.\t%s\t\t\t%d\n", n, 
					scorelist[n-1].name, 
					scorelist[n-1].score);

		gtk_text_insert (GTK_TEXT(hai_text), NULL, NULL, NULL,
				stuff, strlen (stuff));
	}
	
	gtk_widget_set_usize (gwindow, 640, 480);
	gtk_widget_set_uposition (gwindow, 210, 135);
	
	gtk_signal_connect (GTK_OBJECT(gwindow), "delete_event", 
			GTK_SIGNAL_FUNC(end_gtk), NULL);
	gtk_signal_connect (GTK_OBJECT(ok_btn), "clicked",
			GTK_SIGNAL_FUNC(opts_ok), NULL);
	gtk_signal_connect (GTK_OBJECT(nok_btn), "clicked",
			GTK_SIGNAL_FUNC(end_gtk), NULL);
	gtk_signal_connect (GTK_OBJECT(cheat_button), "clicked",
			GTK_SIGNAL_FUNC(cheat_ok), NULL);

	gtk_box_pack_start (GTK_BOX(ship_box), ship_label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(ship_box), ship_combo, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(cheat_box), cheat_label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(cheat_box), cheat_entry, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(cheat_box), cheat_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box1), ok_btn, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(box1), nok_btn, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(box2), ship_box, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box2), cheat_box, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box2), hai_label, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box2), hai_text, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box2), box1, TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER(gwindow), box2);

	gtk_widget_show (ship_label);
	gtk_widget_show (ship_combo);
	gtk_widget_show (ship_box);
	gtk_widget_show (cheat_label);
	gtk_widget_show (cheat_entry);
	gtk_widget_show (cheat_button);
	gtk_widget_show (cheat_box);
	gtk_widget_show (ok_btn);
	gtk_widget_show (nok_btn);
	gtk_widget_show (box1);
	gtk_widget_show (hai_label);
	gtk_widget_show (hai_text);
	gtk_widget_show (box2);
	gtk_widget_show (gwindow);

	gtk_main ();
}

void show_text (char *filename)
{
	char buffer;
	char ch[] = {'\0', '\0'};
	FILE *textfile;
	GtkWidget *text;
	GtkWidget *button;
	GtkWidget *box;
	GtkWidget *foo;
	GtkWidget *bar;

	gwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	text = gtk_text_new (NULL, NULL);
	foo = gtk_vscrollbar_new (GTK_TEXT(text)->vadj);
	button = gtk_button_new_with_label ("Exit");
	bar = gtk_hbox_new (FALSE, 0);
	box = gtk_vbox_new (FALSE, 0);

	gtk_widget_set_usize (gwindow, 640, 480);
	gtk_widget_set_uposition (gwindow, 220, 130);

	gtk_text_freeze (GTK_TEXT(text));
	textfile = fopen (stralloc (TEXTDIR, filename), "r");
	buffer = fgetc (textfile);
	while (buffer != EOF)
	{
		ch[0] = buffer;
		gtk_text_insert (GTK_TEXT(text), NULL, NULL, NULL, ch, 1);
		buffer = fgetc (textfile);
	}
	fclose (textfile);
	gtk_text_thaw (GTK_TEXT(text));
	
	gtk_signal_connect (GTK_OBJECT(gwindow), "delete_event",
			GTK_SIGNAL_FUNC(end_gtk), NULL);
	gtk_signal_connect (GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(
				end_gtk), NULL);
	
	gtk_box_pack_start (GTK_BOX(bar), text, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(bar), foo, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box), bar, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(box), button, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(gwindow), box);

	gtk_widget_show (text);
	gtk_widget_show (foo);
	gtk_widget_show (bar);
	gtk_widget_show (button);
	gtk_widget_show (box);
	gtk_widget_show (gwindow);

	gtk_main ();
}

#endif
