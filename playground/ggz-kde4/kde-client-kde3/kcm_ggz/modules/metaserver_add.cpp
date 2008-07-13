#include "metaserver_add.h"
#include "metaserver_add.moc"

#include <klocale.h>

#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>

MetaserverAdd::MetaserverAdd(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, QString::null, KDialogBase::Ok | KDialogBase::Cancel)
{
	QVBoxLayout *vbox;
	QLabel *lauri, *laproto;
	QFrame *root;
	
	root = makeMainWidget();

	lauri = new QLabel(i18n("URI:"), root);
	laproto = new QLabel(i18n("Supported classes:"), root);

	eduri = new QLineEdit(root);
	edproto = new QLineEdit(root);

	vbox = new QVBoxLayout(root, 5);
	vbox->add(lauri);
	vbox->add(eduri);
	vbox->add(laproto);
	vbox->add(edproto);

	connect(this, SIGNAL(okClicked()), SLOT(slotAccept()));
	connect(this, SIGNAL(cancelClicked()), SLOT(close()));

	//resize(300, 150);
	setCaption(i18n("Add a metaserver"));
	show();
}

MetaserverAdd::~MetaserverAdd()
{
}

void MetaserverAdd::slotAccept()
{
	emit signalAdd(eduri->text(), edproto->text());
	accept();
}

