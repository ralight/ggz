#ifndef KGGZLIB_UTIL_HEADER
#define KGGZLIB_UTIL_HEADER

#include <ksharedconfig.h>

class GGZProfile;
class KConfigGroup;

class Util
{
	public:
		void saveprofile(const GGZProfile& profile, KConfigGroup& cg, KSharedConfig::Ptr conf);
		GGZProfile loadprofile(const KConfigGroup& cg) const;
};

#endif
