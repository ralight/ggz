#include "shadowclient.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

ShadowClient::ShadowClient(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label, *label2;
	QPushButton *ok, *cancel;

	label = new QLabel("Please enter the command line:", this);
	label2 = new QLabel("Socket app name:", this);

	sockname = new KLineEdit(this);
	input = new KLineEdit(this);

	ok = new QPushButton("OK", this);
	cancel = new QPushButton("Cancel", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(input);
	vbox->add(label2);
	vbox->add(sockname);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(ok, SIGNAL(clicked()), SLOT(slotPressed()));

	show();
}

ShadowClient::~ShadowClient()
{
}

void ShadowClient::slotPressed()
{
	emit signalClient(sockname->text().latin1(), input->text().latin1());
	close();
}

