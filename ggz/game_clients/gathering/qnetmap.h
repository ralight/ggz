#ifndef Q_NETMAP_H
#define Q_NETMAP_H

// Just to go sure about Qt ...
#ifdef DEBUG
#undef DEBUG
#endif

// The Netmap map representation
#include <NetmapMap>

// Qt includes
#include <qwidget.h>
#include <qpixmap.h>

// C++ includes
#include <list>

/**
	The structure used to hold sprite information
*/

struct Sprite
{
	/** A tile value */
	Ntile tile;
	/** A Qt Pixmap */
	QPixmap sprite;
};

/**
	This class should be used when creating games with the netmap library
	when using the Qt widget kit.
	It is a simple Qwidget (and should be treated as such), but provides some
	additional useful routines for games programmers.
*/

class QNetmap : public QWidget
{
	Q_OBJECT
	public:
		/** Constructor */
		QNetmap(QWidget *parent = NULL, char *name = NULL);

		/** Destructor */
		~QNetmap();

		/** The map to be used by the widget. */
		void setMap(NetmapMap *map);

		/** Substitute given tile value with a certain sprite */
		void setSprite(Ntile tile, QPixmap sprite);

		/** Return a sprite */
		QPixmap sprite(Ntile tile);

	private:
		/** The map used by the widget */
		NetmapMap *m_map;

		/** A list which contains all sprites */
		list<Sprite> spritelist;
};

#endif

