// Connection dialog includes
#include "connectiondialog.h"

// KGGZ includes
# include "qasyncpixmap.h"

// Qt includes
#include <qlistview.h>
#include <qitemdelegate.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>

class ItemDelegate : public QItemDelegate
{
public:
	ItemDelegate()
	: QItemDelegate()
	{
	}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		if(option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		QMap<QString, QVariant> datamap = index.data(Qt::UserRole + 1).toMap();
		QString url = datamap["url"].toString();
		QString name = datamap["name"].toString();
		QString logintype = datamap["logintype"].toString();

		//QPixmap pix = index.data(Qt::UserRole + 2).value<QPixmap>();
		QPixmap pix = datamap["pixmap"].value<QPixmap>();
		if(pix.isNull())
		{
			//QString icon = datamap["icon"].toString();
			//pix = QPixmap(icon);
			////index.setData(pix, Qt::UserRole + 2);
			//datamap["pixmap"] = pix.scaled(QSize(32, 32), Qt::KeepAspectRatio);
		}

		int x = option.rect.left();
		int y = option.rect.top();

		QString loginstring;
		if(logintype == "guest")
			loginstring = "You'll play as a guest.";
		else if(logintype == "registered")
			loginstring = "You're already registered.";
		else if(logintype == "firsttime")
			loginstring = "You'll apply for an account.";

		painter->save();
		painter->drawPixmap(x + 10, y + 29, pix);
		painter->setFont(QFont(QString(), 15));
		painter->drawText(x + 50, y + 30, name);
		painter->setFont(QFont(QString(), 8));
		painter->drawText(x + 50, y + 50, url);
		painter->drawText(x + 50, y + 70, loginstring);
		painter->restore();
	}

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		Q_UNUSED(option);
		Q_UNUSED(index);

		return QSize(100, 90);
	}
};

ConnectionDialog::ConnectionDialog()
: QWidget()
{
	QListView *listview = new QListView();

	QPushButton *manage_button = new QPushButton("Manage sites...");
	QPushButton *connect_button = new QPushButton("Connect");
	QPushButton *cancel_button = new QPushButton("Cancel");

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(manage_button);
	hbox->addStretch();
	hbox->addWidget(connect_button);
	hbox->addWidget(cancel_button);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(listview);
	vbox->addLayout(hbox);
	setLayout(vbox);

	QStandardItemModel *model = new QStandardItemModel();

	QMap<QString, QVariant> mymap;
	mymap["url"] = "ggz://localhost:5688";
	mymap["name"] = "Honest Harry's Server";
	mymap["logintype"] = "guest";
	mymap["icon"] = "honestharry.png";
	QStandardItem *item = new QStandardItem();
	item->setData(mymap);
	model->appendRow(item);

	QMap<QString, QVariant> mymap2;
	mymap2["url"] = "ggz://live.ggzgamingzone.org";
	mymap2["name"] = "GGZ Live server";
	mymap2["logintype"] = "registered";
	//mymap2["icon"] = "ggzlogo.png";
	mymap2["icon"] = "/home/josef/home.checkout/projects/ggz-trunk/playground/ggz-kde4/connection-dialog/ggzlogo.png";
	QStandardItem *item2 = new QStandardItem();
	item2->setData(mymap2);
	model->appendRow(item2);

	QMap<QString, QVariant> mymap3;
	mymap3["url"] = "ggz://live.ggzgamingzone.org";
	mymap3["name"] = "Josef's server";
	mymap3["logintype"] = "guest";
	mymap3["icon"] = "http://us.ggzgamingzone.org/~josef/cestmoi2.png";
	QStandardItem *item3 = new QStandardItem();
	item3->setData(mymap3);
	model->appendRow(item3);

	for(int i = 0; i < model->rowCount(); i++)
	{
		QStandardItem *item = model->item(i);
		QMap<QString, QVariant> map = item->data().toMap();
		QString url = map["icon"].toString();

		m_apixmaps[url] = item;

		QAsyncPixmap *apixmap = new QAsyncPixmap(url, this);
		if(!apixmap->isNull())
		{
			QMap<QString, QVariant> map = item->data().toMap();
			map["pixmap"] = apixmap->scaled(QSize(32, 32), Qt::KeepAspectRatio);
			item->setData(map);
		}

		connect(apixmap,
			SIGNAL(signalLoaded(const QString&, const QPixmap&)),
			SLOT(slotLoaded(const QString&, const QPixmap&)));
	}

	ItemDelegate *delegate = new ItemDelegate();

	listview->setModel(model);
	listview->setItemDelegate(delegate);
	listview->setEditTriggers(QAbstractItemView::NoEditTriggers);

	connect(cancel_button, SIGNAL(clicked()), SLOT(close()));

	setWindowTitle("Connect to Gaming Zone");
	resize(320, 350);
	show();
}

void ConnectionDialog::slotLoaded(const QString& url, const QPixmap& pixmap)
{
	QStandardItem *item = m_apixmaps[url];
	if(!item)
		return;

	QMap<QString, QVariant> map = item->data().toMap();
	map["pixmap"] = pixmap.scaled(QSize(32, 32), Qt::KeepAspectRatio);
	item->setData(map);
}

