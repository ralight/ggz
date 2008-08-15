#ifndef KGGZLIB_KGGZACTION_H
#define KGGZLIB_KGGZACTION_H

#include "kggzlib_export.h"

class QObject;
class KAction;
class KActionCollection;

KGGZLIB_EXPORT KAction* kggzAction(const QString& what,
	const QObject *recvr,
	const char *slot,
	KActionCollection* parent,
	const char *name = 0);

#endif
