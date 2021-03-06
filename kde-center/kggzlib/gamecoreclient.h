#ifndef GAME_CORE_CLIENT_H
#define GAME_CORE_CLIENT_H

#include <QMainWindow>

#include "kggzlib_export.h"

class EmbeddedCoreClient;
class KGGZCoreLayer;

namespace KGGZCore
{
	class CoreClient;
}

class KGGZLIB_EXPORT GameCoreClient : public QMainWindow
{
	Q_OBJECT
	public:
		GameCoreClient(KGGZCore::CoreClient *core, KGGZCoreLayer *layer);

	private:
		EmbeddedCoreClient *m_ecc;
};

#endif

