#ifndef KRYDS_H
#define KRYDS_H

#include <QObject>

#include <kggzdmod/event.h>

namespace KGGZdMod
{
	class Module;
};

class Kryds : public QObject
{
	Q_OBJECT
	public:
		Kryds(QObject *parent = NULL);
	private slots:
 		void slotEvent(const KGGZdMod::Event& event);
	private:
		KGGZdMod::Module *m_module;
};

#endif
