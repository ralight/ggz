#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include "main.h"
#include "game.h"
#include "table.h"
#include "hand.h"

#include "cards.xpm"
#include "cards-b1.xpm"
#include "cards-b2.xpm"
#include "cards-b3.xpm"
#include "cards-b4.xpm"
/* The following would be hell to change */
#define CARDWIDTH	71
#define CARDHEIGHT	96

static GdkPixmap *table_buf, *table_buf_backup;
static GdkPixmap *cards, *cards_b1, *cards_b2, *cards_b3, *cards_b4;
static GtkStyle *f1_style;
static GtkWidget *f1;
static struct {
	int	card;
	int	dest_x, dest_y;
	int	card_x, card_y;
	float	cur_x, cur_y;
	float	step_x, step_y;
	gint	cb_tag;
} anim;

static void table_card_clicked(int);
static void table_card_select(int);
static void table_card_play(int);
static void table_animation_trigger(int, int, int, int, int);
static gint table_animation_callback(gpointer);


/* table_initialize()
 *   Setup and draw the table areas on the fixed1 dialog item
 */
void table_initialize(void)
{
	GdkBitmap *mask;
	int i, x1, y1, x2, y2;

	/* This starts our drawing code */
	f1 = gtk_object_get_data(GTK_OBJECT(dlg_main), "fixed1");
	f1_style = gtk_widget_get_style(f1);
	cards = gdk_pixmap_create_from_xpm_d(f1->window, &mask,
					     &f1_style->bg[GTK_STATE_NORMAL],
					     (gchar **) cards_xpm);
	cards_b1 = gdk_pixmap_create_from_xpm_d(f1->window, &mask,
					    &f1_style->bg[GTK_STATE_NORMAL],
					    (gchar **) cards_b1_xpm);
	cards_b2 = gdk_pixmap_create_from_xpm_d(f1->window, &mask,
					    &f1_style->bg[GTK_STATE_NORMAL],
					    (gchar **) cards_b2_xpm);
	cards_b3 = gdk_pixmap_create_from_xpm_d(f1->window, &mask,
					    &f1_style->bg[GTK_STATE_NORMAL],
					    (gchar **) cards_b3_xpm);
	cards_b4 = gdk_pixmap_create_from_xpm_d(f1->window, &mask,
					    &f1_style->bg[GTK_STATE_NORMAL],
					    (gchar **) cards_b4_xpm);
	table_buf = gdk_pixmap_new(f1->window,
			     f1->allocation.width,
			     f1->allocation.height,
			     -1);
	table_buf_backup = gdk_pixmap_new(f1->window,
			     f1->allocation.width,
			     f1->allocation.height,
			     -1);

	/* Clear our buffer to the style's background color */
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   0, 0,
			   f1->allocation.width, f1->allocation.height);

	/* Draw card areas */
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 5,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 111,
			   106, 241);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 357,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   111, 357,
			   241, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 357,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 116,
			   106, 241);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 5,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   116, 5,
			   241, 106);

/* JUST TESTING FOR NOW */
	/* Fill hand with random cards */
	hand.hand_size = 10;
	hand.selected_card = -1;
	for(i=0; i<10; i++)
		hand.card[i] = random() % 52;
	game.state = LP_STATE_MOVE;

	/* Put some cards in the buffer */
	for(i=0; i<10; i++) {
		x1 = (hand.card[i] / 13) * CARDWIDTH;
		y1 = (hand.card[i] % 13) * CARDHEIGHT;
		x2 = 116.5 + (i * CARDWIDTH/4.0);
		y2 = 363;
		gdk_draw_pixmap(table_buf,
			        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				x1, y1,
				x2, y2,
				CARDWIDTH, CARDHEIGHT);
	}
	for(i=0; i<10; i++) {
		x1 = 10;
		y1 = 116.5 + (i * CARDWIDTH/4.0);
		gdk_draw_pixmap(table_buf,
			        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards_b2,
				0, 142,
				x1, y1,
				CARDHEIGHT, CARDWIDTH);
	}
	for(i=9; i>=0; i--) {
		x1 = 121.5 + (i * CARDWIDTH/4.0);
		y1 = 10;
		gdk_draw_pixmap(table_buf,
			        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards_b3,
				71, 0,
				x1, y1,
				CARDWIDTH, CARDHEIGHT);
	}
	for(i=9; i>=0; i--) {
		x1 = 363;
		y1 = 121.5 + (i * CARDWIDTH/4.0);
		gdk_draw_pixmap(table_buf,
			        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards_b4,
				0, 71,
				x1, y1,
				CARDHEIGHT, CARDWIDTH);
	}
/* END OF JUST TESTING */

	/* Display the buffer */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			0, 0,
			0, 0,
			f1->allocation.width, f1->allocation.height);
}


/* table_handle_expose_event()
 *   Redraw our table areas that just got exposed, including
 *   all dependent widgets.
 */
void table_handle_expose_event(GdkEventExpose *event)
{
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);
}


/* table_handle_click_event()
 *   Check for what card has been clicked and process it
 */
void table_handle_click_event(GdkEventButton *event)
{
	int target;
	int x;

	/* Real quick, see if we even care */
	if(event->x < 116 || event->x > 400
	   || event->y < 363 || event->y > 512
	   || game.state != LP_STATE_MOVE)
		return;

	/* Calculate our card target */
	for(target=0; target<9; target++) {
		x = 116.5 + ((target+1) * CARDWIDTH/4.0);
		if(event->x <= x)
			break;
	}
	/* This seemed more efficient than a loop */
	if(hand.card[target] == -1)
		if(hand.card[--target] == -1)
			if(hand.card[--target] == -1)
				if(hand.card[--target] == -1)
					return;

	table_card_clicked(target);
}


/* table_card_clicked()
 *   Handle a card that has been clicked by either popping it forward
 *   or playing it if it is already selected.
 */
static void table_card_clicked(int card)
{
	if(card == hand.selected_card) {
		/* Play this card */
		table_card_play(card);
		hand.in_play_card = card;
		hand.card[card] = -1;
		hand.selected_card = -1;
	} else {
		/* Pop the card forward and select it */
		table_card_select(card);
		hand.selected_card = card;
	}
}


/* table_card_select()
 *   Popout the newly selected card, possibly restoring table state if
 *   we are popping an old card back into the hand.
 */
static void table_card_select(int card)
{
	int i, x1, y1, x2, y2;

	if(hand.selected_card >= 0)
		/* Restore the table image */
		gdk_draw_pixmap(table_buf,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf_backup,
				112, 348,
				112, 348,
				239, 114);
	else
		/* Backup the table image */
		gdk_draw_pixmap(table_buf_backup,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf,
				112, 348,
				112, 348,
				239, 114);

	/* Clean the card area */
	gdk_draw_rectangle(table_buf,
		   	f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
		   	TRUE,
		   	112, 358,
		   	239, 104);

	/* Draw the cards, with our selected card popped forward */
	for(i=0; i<10; i++) {
		if(hand.card[i] < 0)
			continue;
		x1 = (hand.card[i] / 13) * CARDWIDTH;
		y1 = (hand.card[i] % 13) * CARDHEIGHT;
		x2 = 116.5 + (i * CARDWIDTH/4.0);
		y2 = 363;
		if(i == card)
			y2 -= 10;
		gdk_draw_pixmap(table_buf,
		        	f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				x1, y1,
				x2, y2,
				CARDWIDTH, CARDHEIGHT);
	}

	/* Refresh the on-screen image */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			112, 348,
			112, 348,
			239, 114);
}


/* table_card_play()
 *   Move the selected card out onto the playing area.
 */
static void table_card_play(int card)
{
	int i, x1, y1, x2, y2;

	/* Start by restoring gfx to a clean state */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf_backup,
			112, 348,
			112, 348,
			239, 114);

	/* Clean the card area */
	gdk_draw_rectangle(table_buf,
		   	f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
		   	TRUE,
		   	112, 358,
		   	239, 104);

	/* Draw the cards, eliminating the card in play */
	for(i=0; i<10; i++) {
		if(hand.card[i] < 0)
			continue;
		x1 = (hand.card[i] / 13) * CARDWIDTH;
		y1 = (hand.card[i] % 13) * CARDHEIGHT;
		x2 = 116.5 + (i * CARDWIDTH/4.0);
		y2 = 363;
		if(i == card)
			continue;
		gdk_draw_pixmap(table_buf,
		        	f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				x1, y1,
				x2, y2,
				CARDWIDTH, CARDHEIGHT);
	}

	/* Backup entire table image */
	gdk_draw_pixmap(table_buf_backup,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			0, 0,
			0, 0,
			f1->allocation.width, f1->allocation.height);

	/* Now draw the card on the table */
	x1 = (hand.card[card] / 13) * CARDWIDTH;
	y1 = (hand.card[card] % 13) * CARDHEIGHT;
	x2 = 116.5 + (card * CARDWIDTH/4.0);
	y2 = 363;
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			cards,
			x1, y1,
			x2, y2,
			CARDWIDTH, CARDHEIGHT);

	/* And refresh the on-screen image */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			112, 348,
			112, 348,
			239, 114);

	/* Setup and trigger the card animation */
	table_animation_trigger(hand.card[card], x2, y2, 199, 242);
}




/* table_animation_trigger()
 *   Setup and trigger the card animation
 */
void table_animation_trigger(int card, int x1, int y1, int x2, int y2)
{
	#define FRAMES		15
	#define DURATION	500	/* In milliseconds */

	/* Store all our info */
	anim.card = card;
	anim.card_x = (card / 13) * CARDWIDTH;
	anim.card_y = (card % 13) * CARDHEIGHT;
	anim.cur_x = x1;
	anim.cur_y = y1;
	anim.dest_x = x2;
	anim.dest_y = y2;

	/* This sets up 15 frames of animation */
	anim.step_x = (x1 - x2) / (float) FRAMES;
	anim.step_y = (y1 - y2) / (float) FRAMES;

	/* This sets up our timeout callback */
	anim.cb_tag = gtk_timeout_add(DURATION / FRAMES,
				      table_animation_callback, NULL);

	game.state = LP_STATE_ANIM;
}


/* table_animation_callback()
 *   Handle one frame of card animation, this is triggered by a GtkTimeout
 *   setup in table_animation_trigger().
 */
gint table_animation_callback(gpointer ignored)
{
	float new_x, new_y;
	int ref_x, ref_y;

	/* Calculate our next move */
	new_x = anim.cur_x - anim.step_x;
	if(abs(new_x - anim.dest_x) < 2)
		new_x = anim.dest_x;
	new_y = anim.cur_y - anim.step_y;
	if(abs(new_y - anim.dest_y) < 2)
		new_y = anim.dest_y;

	/* Restore table area around old card image from backup */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf_backup,
			anim.cur_x, anim.cur_y,
			anim.cur_x, anim.cur_y,
			CARDWIDTH, CARDHEIGHT);

	/* Draw our new card position */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			cards,
			anim.card_x, anim.card_y,
			new_x, new_y,
			CARDWIDTH, CARDHEIGHT);

	/* And refresh the on-screen image */
	if(new_x < anim.cur_x)
		ref_x = new_x;
	else
		ref_x = anim.cur_x;
	if(new_y < anim.cur_y)
		ref_y = new_y;
	else
		ref_y = anim.cur_y;
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			ref_x, ref_y,
			ref_x, ref_y,
			CARDWIDTH + abs(anim.step_x) + 2,
			CARDHEIGHT + abs(anim.step_y) + 2);

	/* If we are there, stop the animation process */
	if(new_x == anim.dest_x && new_y == anim.dest_y) {
		/* We'll go to WAIT state in the real game */
		game.state = LP_STATE_MOVE;
		return FALSE;
	}

	/* Update our information for next time */
	anim.cur_x = new_x;
	anim.cur_y = new_y;

	/* Continue animating */
	return TRUE;
}
