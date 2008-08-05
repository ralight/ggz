// Server list includes
#include "serverlist.h"

// KGGZ includes
#include "qasyncpixmap.h"

// Qt includes
#include <qlistview.h>
#include <qitemdelegate.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>

class ItemDelegate : public QItemDelegate
{
public:
	ItemDelegate(QWidget *parent = NULL)
	: QItemDelegate(parent)
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
		else
			loginstring = "Unconfigured login profile.";

		painter->save();
		painter->drawPixmap(x + 10, y + 29, pix);
		painter->setFont(QFont(QString(), 15));
		painter->drawText(x + 50, y + 30, name);
		painter->setFont(QFont(QString(), 8));
		painter->drawText(x + 50, y + 50, url);
		painter->drawText(x + 50, y + 70, loginstring);
		painter->restore();

		if(option.state & QStyle::State_Selected)
		{
			// FIXME: might be better to use a separate selection method from Qt
			GGZServer server;
			server.setUri(datamap["url"].toString());
			server.setName(datamap["name"].toString());
			server.setLoginType(datamap["logintype"].toString());
			server.setIcon(datamap["icon"].toString());
			server.setApi(datamap["api"].toString());
			dynamic_cast<ServerList*>(parent())->selected(server);
		}
	}

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		Q_UNUSED(option);
		Q_UNUSED(index);

		return QSize(100, 90);
	}
};

ServerList::ServerList()
: QWidget()
{
	QListView *listview = new QListView();

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(listview);
	setLayout(vbox);

	m_model = new QStandardItemModel();

	ItemDelegate *delegate = new ItemDelegate(this);

	listview->setModel(m_model);
	listview->setItemDelegate(delegate);
	listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ServerList::addServer(const GGZServer& server)
{
	QMap<QString, QVariant> map;
	map["url"] = server.uri();
	map["name"] = server.name();
	map["logintype"] = server.loginType();
	map["icon"] = server.icon();
	map["api"] = server.api();
	QStandardItem *item = new QStandardItem();
	item->setData(map);
	m_model->appendRow(item);

	//QMap<QString, QVariant> map = item->data().toMap();
	//QString url = map["icon"].toString();

	m_apixmaps[server.icon()] = item;

	QAsyncPixmap *apixmap = new QAsyncPixmap(server.icon(), this);
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

void ServerList::slotLoaded(const QString& url, const QPixmap& pixmap)
{
	QStandardItem *item = m_apixmaps[url];
	if(!item)
		return;

	QMap<QString, QVariant> map = item->data().toMap();
	map["pixmap"] = pixmap.scaled(QSize(32, 32), Qt::KeepAspectRatio);
	item->setData(map);
}

// FIXME: This method shouldn't be necessary if we got the signal right
void ServerList::selected(const GGZServer& server)
{
	emit signalSelected(server);
}
