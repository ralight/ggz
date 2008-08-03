#include "qfilecache.h"

#include <kstandarddirs.h>

#include <QtCore/QDir>

static QFileCache *s_object = NULL;

QFileCache::QFileCache()
{
}

QFileCache *QFileCache::instance()
{
	if(!s_object)
		s_object = new QFileCache();
	return s_object;
}

/*QPixmap QFileCache::load(QString url)
{
}*/

void QFileCache::save(QString url, QPixmap pixmap)
{
	pixmap.save(QFileCache::cachepath(url));
}

QString QFileCache::cachepath(QString url)
{
	KStandardDirs d;
	QString cachedir = d.saveLocation("cache") + "/asyncpixmaps/";
	QString cachename = url.toUtf8().toBase64();
	QString path = cachedir + cachename + ".png";
	return path;
}
