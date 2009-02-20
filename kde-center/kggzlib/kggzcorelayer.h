#ifndef KGGZLIB_KGGZCORELAYER_HEADER
#define KGGZLIB_KGGZCORELAYER_HEADER

#include <kggzcore/coreclient.h>
#include <kggzcore/room.h>

#include "kggzlib_export.h"

namespace KGGZCore
{
	class Player;
}

class KGGZLIB_EXPORT KGGZCoreLayer : public QObject
{
	Q_OBJECT

	public:
		KGGZCoreLayer(QObject *parent = NULL, QString protengine = QString(), QString protversion = QString());
		~KGGZCoreLayer();

		void ggzcore(QString uri);
		void configureTable(QList<KGGZCore::Player> seats);
		void launch();

		KGGZCore::CoreClient *core();
		void setCore(KGGZCore::CoreClient *core);

	signals:
		void signalReady(bool ready);
		void signalRoomReady(bool ready);

	protected:
		void activity(QString activity);
		void checkTables();

	protected slots:
		void slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::CoreClient::AnswerMessage message);
		void slotEvent(KGGZCore::CoreClient::EventMessage message);
		void slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::Room::AnswerMessage message);
		void slotEvent(KGGZCore::Room::EventMessage message);

	private slots:
		void slotModuleReady();
		void slotTableReady();

	private:
		void launchmodule();
		void switchroom();
		void notready();

		KGGZCore::CoreClient *m_core;
		QList<KGGZCore::Player> m_seats;
		QString m_protengine;
		QString m_protversion;
		bool m_typedrooms;
};

#endif
