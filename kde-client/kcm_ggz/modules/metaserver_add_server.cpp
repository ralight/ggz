#include "metaserver_add_server.h"
#include "metaserver_add_server.moc"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>

MetaserverAddServer::MetaserverAddServer(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *ok, *cancel;
	QLabel *lauri, *latype, *lacomment;

	ok = new QPushButton("OK", this);
	cancel = new QPushButton("Cancel", this);

	lauri = new QLabel("URI:", this);
	latype = new QLabel("Type of server:", this);
	lacomment = new QLabel("Comment:", this);

	eduri = new QLineEdit(this);
	edtype = new QLineEdit(this);
	edcomment = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(lauri);
	vbox->add(eduri);
	vbox->add(latype);
	vbox->add(edtype);
	vbox->add(lacomment);
	vbox->add(edcomment);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));

	resize(300, 200);
	setCaption("Add a server");
	show();
}

MetaserverAddServer::~MetaserverAddServer()
{
}

void MetaserverAddServer::slotAccept()
{
	emit signalAdd(eduri->text(), edtype->text(), edcomment->text());
	close();
}

