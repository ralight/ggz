#include "metaserver_add.h"
#include "metaserver_add.moc"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>

MetaserverAdd::MetaserverAdd(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *ok, *cancel;
	QLabel *lauri, *laproto;

	ok = new QPushButton("OK", this);
	cancel = new QPushButton("Cancel", this);

	lauri = new QLabel("URI:", this);
	laproto = new QLabel("Supported classes:", this);

	eduri = new QLineEdit(this);
	edproto = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(lauri);
	vbox->add(eduri);
	vbox->add(laproto);
	vbox->add(edproto);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));

	resize(300, 150);
	setCaption("Add a metaserver");
	show();
}

MetaserverAdd::~MetaserverAdd()
{
}

void MetaserverAdd::slotAccept()
{
	emit signalAdd(eduri->text(), edproto->text());
	close();
}

