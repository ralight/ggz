#ifndef KPLAYERFACE_GALLERY_H
#define KPLAYERFACE_GALLERY_H

#include <qwidget.h>
#include <qmap.h>

class KPlayerFace;
class QGridLayout;

class KPlayerFaceGallery : public QWidget
{
	Q_OBJECT
	public:
		KPlayerFaceGallery(QWidget *parent = NULL, const char *name = NULL);
		~KPlayerFaceGallery();
		void setMainWidget(QWidget *w);
		void add(QWidget *w);
		void setLayout(int layout);

		enum Layout
		{
			leftrow   = 0x01,
			rightrow  = 0x02,
			toprow    = 0x04,
			bottomrow = 0x08,
			edges     = 0x10
		};

	private:
		QGridLayout *m_layout;
		QMap<int, QWidget*> m_map;
		int m_lay;
};

#endif

