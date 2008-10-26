#ifndef VENCEDOR_PREFS_H
#define VENCEDOR_PREFS_H

#include <kconfigskeleton.h>

class Prefs : public KConfigSkeleton
{
	Q_OBJECT
	public:
		Prefs();
		static Prefs *self();

	private:
		QString m_syncurl;
};

#endif
