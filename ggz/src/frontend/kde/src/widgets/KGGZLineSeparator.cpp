#include "KGGZLineSeparator.h"

KGGZLineSeparator::KGGZLineSeparator(QWidget *parent, const char *name)
: QFrame(parent, name)
{
	setFrameStyle(Panel | Sunken);
	setBackgroundColor(QColor(150, 0, 0));
	setDirection(horizontal);
	show();
}

KGGZLineSeparator::~KGGZLineSeparator()
{
}

void KGGZLineSeparator::setDirection(Directions direction)
{
	if(direction == horizontal)
	{
		setFixedHeight(3);
		setMinimumWidth(parentWidget()->minimumWidth());
		setMaximumWidth(parentWidget()->maximumWidth());
	}
	else
	{
		setFixedWidth(3);
		setMinimumHeight(parentWidget()->minimumHeight());
		setMaximumHeight(parentWidget()->maximumHeight());
	}
}

