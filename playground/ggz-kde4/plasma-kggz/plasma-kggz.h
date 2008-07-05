#ifndef PLASMA_KGGZ_HEADER
#define PLASMA_KGGZ_HEADER

#include <kicon.h>
#include <plasma/applet.h>
#include <plasma/svg.h>
 
//class QSizeF;
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
		void mousePressEvent(QGraphicsSceneMouseEvent*);

	private slots:
		void slotConfiguration();
 
	private:
		Plasma::Svg m_svg;
		KIcon m_icon;
		ConfigWidget *m_config;
};
 
K_EXPORT_PLASMA_APPLET(kggz, PlasmaKGGZ)

#endif
