#ifndef PLASMA_KGGZ_HEADER
#define PLASMA_KGGZ_HEADER

#include <KIcon>
#include <plasma/applet.h>
#include <plasma/svg.h>
 
class QSizeF;
 
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
 
    private:
        Plasma::Svg m_svg;
        KIcon m_icon;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(kggz, PlasmaKGGZ)
#endif
