#ifndef KGGZ_LINESEPARATOR_H
#define KGGZ_LINESEPARATOR_H

#include <qframe.h>

class KGGZLineSeparator : public QFrame
{
	Q_OBJECT

	public:
		KGGZLineSeparator(QWidget *parent = NULL, const char *name = NULL);
		~KGGZLineSeparator();

		enum Directions
		{
			horizontal,
			vertical
		};

		void setDirection(Directions direction);
};

#endif

