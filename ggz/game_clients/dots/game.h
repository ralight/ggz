#define MAX_BOARD_WIDTH 25
#define MAX_BOARD_HEIGHT 25

extern void board_init(guint8, guint8);
extern void board_handle_expose_event(GtkWidget *, GdkEventExpose *);
extern void board_handle_click(GtkWidget *, GdkEventButton *);
extern void board_handle_pxb_expose(void);
