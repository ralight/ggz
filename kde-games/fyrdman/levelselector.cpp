#include "levelselector.h"

#include <klocale.h>

#include <qcombobox.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qpushbutton.h>

LevelSelector::LevelSelector(QWidget *parent, const char *name)
: QDialog(parent, name, true)
{
	QVBoxLayout *lay;
	QHBoxLayout *lay2;
	QPushButton *ok, *cancel;

	combo = new QComboBox(this);
	combo->insertItem("Level1");
	combo->insertItem("Level2");

	desc = new QTextEdit(this);
	desc->setReadOnly(true);

	ok = new QPushButton(i18n("Play"), this);
	cancel = new QPushButton(i18n("Cancel"), this);

	lay = new QVBoxLayout(this, 5);
	lay->add(combo);
	lay->add(desc);
	lay2 = new QHBoxLayout(lay, 5);
	lay2->add(ok);
	lay2->add(cancel);

	connect(ok, SIGNAL(clicked()), SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(combo, SIGNAL(activated(int)), SLOT(slotActivated(int)));

	setCaption(i18n("Select a level"));

	slotActivated(0);
}

LevelSelector::~LevelSelector()
{
}

void LevelSelector::accept()
{
	// ...
	QDialog::accept();
}

void LevelSelector::slotActivated(int id)
{
	QString level, author, version;
	int width, height, players;
	QString text;

	level = combo->currentText();
	author = "anonymous";
	version = "0.1";
	width = 20;
	height = 20;
	players = 8;

	text = i18n("Level: <i>%1</i><br>Author: <i>%2</i><br>Version: <i>%3</i><br>").arg(level).arg(author).arg(version);
	text += i18n("Width: <i>%1</i><br>Height: <i>%2</i><br>Players: <i>%3</i>").arg(width).arg(height).arg(players);

	desc->setText(text);
}

void LevelSelector::addLevel(QString title)
{
	combo->insertItem(title);
}

