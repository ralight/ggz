#ifndef Q_NETMAP_SAMPLE_H
#define Q_NETMAP_SAMPLE_H

#include "qnetmap.h"
#include <qevent.h>

class QNetmapSample : public QNetmap
{
Q_OBJECT
public:
	QNetmapSample(QWidget *parent, char *name);
	~QNetmapSample();
	void pushSprite(char *spritename);
	void drawMap();
	void QNetmapSample::drawTile(int x, int y, int num);
protected:
	void keyPressEvent(QKeyEvent *e);
	void resizeEvent(QResizeEvent *e);
private:
	NetmapMap *m_map;
	int m_sprite_counter;
	int m_sprites_horizontal, m_sprites_vertical;
};

#endif
