#include "kcm_ggz_default.h"
#include "kcm_ggz_default.moc"

#include <klocale.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>

KCMGGZDefault::KCMGGZDefault(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QLabel *label_server, *label_username;
	QLabel *label_explanation;
	QVBoxLayout *vbox, *vbox2;
	QGroupBox *box;

	box = new QGroupBox("General", this);
	box->setFixedHeight(150);
	box->setMinimumWidth(400);
	box->move(10, 10);
	box->setBackgroundColor(QColor(255, 200, 0));

	m_server = new QLineEdit(box);
	m_username = new QLineEdit(box);

	label_server = new QLabel(i18n("Preferred connection server"), box);
	label_server->setBackgroundColor(QColor(255, 200, 0));
	label_username = new QLabel(i18n("Your login username"), box);
	label_username->setBackgroundColor(QColor(255, 200, 0));

	label_explanation = new QLabel(i18n("The GGZ Gaming Zone needs some information\n"
		" to be configured properly. You can specify here what is needed as\n"
		" default value for KGGZ, GGZap, the IO Slave and other applications.\n"),
		this);
	label_explanation->setBackgroundColor(QColor(255, 200, 0));

	vbox = new QVBoxLayout(box, 15);
	vbox->add(label_server);
	vbox->add(m_server);
	vbox->add(label_username);
	vbox->add(m_username);

	vbox2 = new QVBoxLayout(this, 15);
	vbox2->add(label_explanation);
	vbox2->add(box);

	connect(m_server, SIGNAL(textChanged(const QString &)), SIGNAL(signalChanged()));
	connect(m_username, SIGNAL(textChanged(const QString &)), SIGNAL(signalChanged()));

	setBackgroundColor(QColor(255, 200, 0));

	load();
}

KCMGGZDefault::~KCMGGZDefault()
{
}

void KCMGGZDefault::load()
{
}

void KCMGGZDefault::save()
{
}

const char *KCMGGZDefault::caption()
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

