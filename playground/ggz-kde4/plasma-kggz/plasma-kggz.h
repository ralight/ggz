#ifndef PLASMA_KGGZ_HEADER
#define PLASMA_KGGZ_HEADER

#include <kicon.h>

#include <plasma/applet.h>
#include <plasma/svg.h>

#include <kggzcore/coreclient.h>
#include <kggzcore/room.h>
 
class ConfigWidget;

class PlasmaKGGZ : public Plasma::Applet
{
	Q_OBJECT
	public:
		PlasmaKGGZ(QObject *parent, const QVariantList &args);
		~PlasmaKGGZ();
 
		void paintInterface(QPainter *painter,
			const QStyleOptionGraphicsItem *option,
			const QRect& contentsRect);
		void init();

	public slots:
		void createConfigurationInterface(KConfigDialog *parent);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *event);

	private slots:
		void slotConfiguration();
		void slotFeedback(KGGZCore::CoreClient::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::CoreClient::AnswerMessage message);
		void slotEvent(KGGZCore::CoreClient::EventMessage message);
		void slotFeedback(KGGZCore::Room::FeedbackMessage message, KGGZCore::Error::ErrorCode error);
		void slotAnswer(KGGZCore::Room::AnswerMessage message);
		void slotEvent(KGGZCore::Room::EventMessage message);
 
	private:
		Plasma::Svg m_svg;
		KIcon m_icon;
		ConfigWidget *m_config;
		KGGZCore::CoreClient *m_core;
		QString m_activity;
};
 
K_EXPORT_PLASMA_APPLET(kggz, PlasmaKGGZ)

#endif
