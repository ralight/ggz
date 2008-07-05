#include "plasma-kggz.h"

#include "configwidget.h"

#include <qpainter.h>
#include <qfontmetrics.h>
//#include <qsizef.h>

#include <kconfigdialog.h>
 
#include <plasma/svg.h>
#include <plasma/theme.h>
 
PlasmaKGGZ::PlasmaKGGZ(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args),
	m_svg(this),
	m_icon("ggz"),
	m_config(0)
{
	setBackgroundHints(DefaultBackground);
	setHasConfigurationInterface(true);

	m_svg.setImagePath("widgets/plot-background");

	resize(300, 600);
}
 
PlasmaKGGZ::~PlasmaKGGZ()
{
	if (hasFailedToLaunch())
	{
	}
	else
	{
	}

	delete m_config;
}
 
void PlasmaKGGZ::init()
{
	if (m_icon.isNull())
	{
		setFailedToLaunch(true, "GGZ icon not found.");
	}
} 
 
void PlasmaKGGZ::paintInterface(QPainter *p,
	const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);

	m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
	m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());

	p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(), (int)contentsRect.width() - 14));
	p->save();
	p->setPen(Qt::white);
	p->drawText(contentsRect, Qt::AlignBottom | Qt::AlignHCenter, "GGZ Gaming Zone");
	p->restore();
}

void PlasmaKGGZ::mousePressEvent(QGraphicsSceneMouseEvent*)
{
	kDebug() << "mousePressEvent!";
	// ...
}

void PlasmaKGGZ::createConfigurationInterface(KConfigDialog *parent)
{
	kDebug() << "createConfigurationInterface!";

	m_config = new ConfigWidget(parent);

	parent->addPage(m_config, parent->windowTitle(), icon());

	connect(parent, SIGNAL(applyClicked()), SLOT(slotConfiguration()));
	connect(parent, SIGNAL(okClicked()), SLOT(slotConfiguration()));
}

void PlasmaKGGZ::slotConfiguration()
{
	kDebug() << "slotConfiguration!";
}
 
#include "plasma-kggz.moc"
