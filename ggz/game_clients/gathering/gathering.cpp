#include "gathering.h"
#include "qnetmapsample.h"
#include <qlayout.h>
#include <qlabel.h>

Gathering::Gathering(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QNetmapSample *qnetmapsample;

	qnetmapsample = new QNetmapSample(this, "QNetmapSample");

	vbox = new QVBoxLayout(this, 5);
	//vbox->add(labelwidget);
	vbox->add(qnetmapsample);

	resize(500, 400);

	qnetmapsample->setFocus();
	setCaption("GGZ Gathering");
	show();
}

Gathering::~Gathering()
{

}
