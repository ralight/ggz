#ifndef GAME_CORE_CLIENT_H
#define GAME_CORE_CLIENT_H

#include <QMainWindow>

#include "kggzlib_export.h"

class EmbeddedCoreClient;

namespace KGGZCore
{
	class CoreClient;
}

class KGGZLIB_EXPORT GameCoreClient : public QMainWindow
{
	Q_OBJECT
	public:
		GameCoreClient(KGGZCore::CoreClient *core);
	private:
		EmbeddedCoreClient *m_ecc;
};

#endif

