#include "kggz_tables.h"

QString instdir2 = "/usr/local/share/kggz/";

KGGZ_Tables::KGGZ_Tables(QWidget *parent, char *name)
: QIconView(parent, name)
{
// only fake, so why show it...
//	QIconViewItem *item1 = new QIconViewItem(this, "(TicTacToe)\nClassic\n(2 seats, 0 open)", QPixmap(instdir2 + "images/tictactoe.png"));
//	QIconViewItem *item2 = new QIconViewItem(this, "(TicTacToe)\nRoom of Doom\n(2 seats, 1 open)", QPixmap(instdir2 + "images/tictactoe.png"));
//	QIconViewItem *item3 = new QIconViewItem(this, "(Hastings1066)\nExample game\n(6 seats, 5 open)", QPixmap(instdir2 + "images/hastings.png"));
}

KGGZ_Tables::~KGGZ_Tables()
{
}

