#ifndef QEXTPAINTER_H
#define QEXTPAINTER_H

#include <qpainter.h>
#include <qweb.h>

class QExtPainter : public QPainter
{
	public:
		QExtPainter();
		~QExtPainter();
		void drawWeb(QWeb w);
};

#endif

