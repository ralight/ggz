#ifndef KGGZLIB_UTIL_HEADER
#define KGGZLIB_UTIL_HEADER

#include <QPixmap>

#include <ksharedconfig.h>

#include "kggzlib_export.h"

class GGZProfile;
class KConfigGroup;

class KGGZLIB_EXPORT Util
{
	public:
		void saveprofile(const GGZProfile& profile, KConfigGroup& cg, KSharedConfig::Ptr conf);
		GGZProfile loadprofile(const KConfigGroup& cg) const;

		static QPixmap composite(QPixmap bottom, QPixmap top);
		static QPixmap greyscale(QPixmap orig);

		static GGZProfile uriToProfile(const QString& uri);
};

#endif
