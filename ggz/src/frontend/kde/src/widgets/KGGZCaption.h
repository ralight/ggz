#ifndef KGGZ_CAPTION_H
#define KGGZ_CAPTION_H

#include <qframe.h>

class KGGZCaption : public QFrame
{
	Q_OBJECT

	public:
		KGGZCaption(QString caption, QString explanation, QWidget *parent = NULL, const char *name = NULL);
		~KGGZCaption();
};

#endif

