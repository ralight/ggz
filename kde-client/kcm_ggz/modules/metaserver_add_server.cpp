#include "metaserver_add_server.h"
#include "metaserver_add_server.moc"

#include <klocale.h>

#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>

MetaserverAddServer::MetaserverAddServer(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, QString::null, KDialogBase::Ok | KDialogBase::Cancel)
{
	QVBoxLayout *vbox;
	QLabel *lauri, *latype, *laspeed, *lalocation, *lacomment;
	QFrame *root;

	root = makeMainWidget();

	lauri = new QLabel(i18n("URI:"), root);
	latype = new QLabel(i18n("Type of server:"), root);
	lacomment = new QLabel(i18n("Comment:"), root);
	laspeed = new QLabel(i18n("Speed:"), root);
	lalocation = new QLabel(i18n("Location:"), root);

	eduri = new QLineEdit(root);
	edtype = new QLineEdit(root);
	edcomment = new QLineEdit(root);
	edspeed = new QLineEdit(root);
	edlocation = new QLineEdit(root);

	vbox = new QVBoxLayout(root, 5);
	vbox->add(lauri);
	vbox->add(eduri);
	vbox->add(latype);
	vbox->add(edtype);
	vbox->add(lalocation);
	vbox->add(edlocation);
	vbox->add(laspeed);
	vbox->add(edspeed);
	vbox->add(lacomment);
	vbox->add(edcomment);

	connect(this, SIGNAL(okClicked()), SLOT(slotAccept()));
	connect(this, SIGNAL(cancelClicked()), SLOT(close()));

	//resize(300, 200);
	setCaption(i18n("Add a server"));
	show();
}

MetaserverAddServer::~MetaserverAddServer()
{
}

void MetaserverAddServer::slotAccept()
{
	emit signalAdd(eduri->text(), edtype->text(), edlocation->text(), edspeed->text(), edcomment->text());
	close();
}

