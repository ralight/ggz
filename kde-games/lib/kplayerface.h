#ifndef KPLAYERFACE_H
#define KPLAYERFACE_H

#include <qwidget.h>

#include <kurl.h>

class QLabel;
class QFrame;

namespace KIO
{
	class Job;
}

class KPlayerFace : public QWidget
{
	Q_OBJECT
	public:
		KPlayerFace(QWidget *parent = NULL, const char *name = NULL);
		~KPlayerFace();
		void setName(QString name);
		void setText(QString text);
		void setImage(QString url);

	public slots:
		void slotResult(KIO::Job *job);
		void slotImage(KIO::Job *job, const KURL& from, const KURL& dest, bool dir, bool ren);

	private:
		QLabel *m_name, *m_text;
		QFrame *m_image;
};

#endif

