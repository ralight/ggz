#include "kcm_ggz_default.h"
#include "kcm_ggz_default.moc"

#include <klocale.h>
#include <ksimpleconfig.h>
#include <kfiledialog.h>

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qpixmap.h>
#include <qpushbutton.h>

#include <stdlib.h>

#include "config.h"

KCMGGZDefault::KCMGGZDefault(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QLabel *label_server, *label_username;
	QLabel *label_explanation, *label_personalization;
	QVBoxLayout *vbox, *vbox2, *vbox3;
	QHBoxLayout *hbox;
	QGroupBox *box, *mebox;
	QWidget *gear;

	box = new QGroupBox(i18n("General"), this);
	box->setFixedHeight(150);
	box->setMinimumWidth(400);
	box->move(10, 10);
	box->setEraseColor(QColor(255, 200, 0));

	mebox = new QGroupBox(i18n("Personalization"), this);
	mebox->setEraseColor(QColor(255, 200, 0));

	m_server = new QLineEdit(box);
	m_username = new QLineEdit(box);

	label_server = new QLabel(i18n("Preferred connection server"), box);
	label_server->setEraseColor(QColor(255, 200, 0));
	label_username = new QLabel(i18n("Your login username"), box);
	label_username->setEraseColor(QColor(255, 200, 0));

	label_personalization = new QLabel(i18n("Upload an image of yourself to be used in GGZ games."), mebox);
	label_personalization->setEraseColor(QColor(255, 200, 0));
	m_button_personalization = new QPushButton(mebox);
	m_button_personalization->setFixedSize(140, 140);

	label_explanation = new QLabel(i18n("The GGZ Gaming Zone needs some information\n"
		" to be configured properly. The default values for KGGZ, GGZap\n"
		" the IO Slave and other applications can be specified here.\n"),
		this);
	label_explanation->setEraseColor(QColor(255, 200, 0));

	gear = new QWidget(this);
	gear->setFixedSize(200, 212);
	gear->setErasePixmap(QPixmap(QString(KGGZ_DIRECTORY) + "/kcmggz/icons/gear.png"));

	vbox = new QVBoxLayout(box, 15);
	vbox->add(label_server);
	vbox->add(m_server);
	vbox->add(label_username);
	vbox->add(m_username);

	vbox3 = new QVBoxLayout(mebox, 15);
	vbox3->add(label_personalization);
	vbox3->add(m_button_personalization);

	vbox2 = new QVBoxLayout(this, 15);
	hbox = new QHBoxLayout(vbox2, 5);
	hbox->add(gear);
	hbox->addStretch(1);
	hbox->add(mebox);
	vbox2->addStretch(1);
	vbox2->add(label_explanation);
	vbox2->add(box);

	connect(m_server, SIGNAL(textChanged(const QString &)), SIGNAL(signalChanged()));
	connect(m_username, SIGNAL(textChanged(const QString &)), SIGNAL(signalChanged()));
	connect(m_button_personalization, SIGNAL(clicked()), SLOT(slotPersonalization()));

	setEraseColor(QColor(255, 200, 0));
}

KCMGGZDefault::~KCMGGZDefault()
{
}

void KCMGGZDefault::slotPersonalization()
{
	QString picname = KFileDialog::getOpenFileName();
	if(!picname.isNull())
	{
		m_picture = picname;
		loadPicture();
		emit signalChanged();
	}
}

void KCMGGZDefault::loadPicture()
{
	QPixmap pix = QPixmap(m_picture);
	if(!pix.isNull())
	{
		m_button_personalization->setIconSet(QIconSet(pix, pix));
	}
}

void KCMGGZDefault::load()
{
	KSimpleConfig conf(QString("%1/.ggz/ggzap.rc").arg(getenv("HOME")));
	conf.setGroup("Global");
	m_server->setText(conf.readEntry("Server"));
	m_username->setText(conf.readEntry("Username"));
	m_picture = conf.readEntry("Picture");
	loadPicture();
}

void KCMGGZDefault::save()
{
	KSimpleConfig conf(QString("%1/.ggz/ggzap.rc").arg(getenv("HOME")));
	conf.setGroup("Global");
	conf.writeEntry("Server", m_server->text());
	conf.writeEntry("Username", m_username->text());
	conf.writeEntry("Picture", m_picture);

	KSimpleConfig conf2(QString("%1/.ggz/personalization").arg(getenv("HOME")));
	conf2.setGroup("Personalization");
	conf2.writeEntry("picture", m_picture);
}

QString KCMGGZDefault::caption()
{
	return i18n("Default Settings");
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZDefault(parent, name);
		//return NULL;
	}
}

