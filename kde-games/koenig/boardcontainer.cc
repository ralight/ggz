#include <qlayout.h>
#include <kdebug.h>

#include "boardcontainer.h"

ChessBoardContainer::ChessBoardContainer(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;

	kdDebug(12101) << "ChessBoardContainer::ChessBoardContainer()" << endl;

	setMinimumSize(240, 240);

	m_root = new ChessBoard(this);
	vbox = new QVBoxLayout(this, 10);
	vbox->add(m_root);

	setBackgroundColor(QColor(255, 127, 0)); // FIXME: nice pixmap with wood :-)
	setCaption("Chess Board - inactive");
}

ChessBoardContainer::~ChessBoardContainer(void)
{
}

void ChessBoardContainer::resizeEvent(QResizeEvent *event)
{
	int size = (event->size().width() + event->size().height()) / 2 / 8;

	resize(size * 8, size * 8);
	//root->setGeometry(BOARD_BORDER, BOARD_BORDER, size * 8 - BOARD_BORDER * 2, size * 8 - BOARD_BORDER * 2);
}

ChessBoard *ChessBoardContainer::root()
{
	return m_root;
}

