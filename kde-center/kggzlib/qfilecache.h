#ifndef Q_FILECACHE_H
#define Q_FILECACHE_H

#include <QString>
#include <QPixmap>

#include "kggzlib_export.h"

// FIXME: At the moment this API is inherently linked to pixmaps
// and also to QAsyncPixmap. A more generic API would be better.

class KGGZLIB_EXPORT QFileCache
{
	public:
		static QFileCache *instance();

		QString cachepath(QString url);

		//QPixmap load(QString url);
		void save(QString url, QPixmap pixmap);

	private:
		QFileCache();
};

#endif
