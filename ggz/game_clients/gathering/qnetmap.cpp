// Include files
#include "qnetmap.h"

/** Constructor */
QNetmap::QNetmap(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name)
{
	setBackgroundColor(QColor(0.0, 0.0, 70.0));
}

/** Destructor */
QNetmap::~QNetmap()
{
	spritelist.clear();
}

/** The map to be used by the widget. */
void QNetmap::setMap(NetmapMap *map)
{
	m_map = map;
}

/** Substitute given tile value with a certain sprite */
void QNetmap::setSprite(Ntile tile, QPixmap sprite)
{
	Sprite tmp;

	tmp.tile = tile;
	tmp.sprite = sprite;

	spritelist.insert(spritelist.end(), tmp);
}

/** Return a sprite */
QPixmap QNetmap::sprite(Ntile tile)
{
	list<Sprite>::iterator it;

	if(!tile) return QPixmap();

	for(it = spritelist.begin(); it != spritelist.end(); it++)
		if(it->tile == tile) return it->sprite;

	return QPixmap();
}
