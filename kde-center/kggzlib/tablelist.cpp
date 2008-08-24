// Table configuration list includes
#include "tablelist.h"

// KGGZ includes
#include "qasyncpixmap.h"
//#include "ggzprofile.h"
#include <kggzcore/player.h>

// Qt includes
#include <qlistview.h>
#include <qitemdelegate.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>

class ItemDelegate : public QItemDelegate
{
public:
	enum Roles
	{
		TableRole = Qt::UserRole + 1,
		PixmapRole = Qt::UserRole + 2
	};

	ItemDelegate(QWidget *parent = NULL)
	: QItemDelegate(parent)
	{
	}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		if(option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		//GGZProfile *profile = reinterpret_cast<GGZProfile*>(index.data(TableRole).value<void*>());
		QPixmap pix = index.data(PixmapRole).value<QPixmap>();

		int x = option.rect.left();
		int y = option.rect.top();

		QString loginstring;
		/*GGZProfile::LoginType logintype = profile->loginType();
		if(logintype == GGZProfile::guest)
			loginstring = "You'll play as a guest.";
		else if(logintype == GGZProfile::registered)
			loginstring = "You're already registered.";
		else if(logintype == GGZProfile::firsttime)
			loginstring = "You'll apply for an account.";
		else
			loginstring = "Unconfigured login profile.";*/

		QString username;
		/*QString username = profile->username();
		if(!username.isEmpty())
			loginstring += " [" + username + "]";*/

		painter->save();
		painter->drawPixmap(x + 10, y + 29, pix);
		painter->setFont(QFont(QString(), 15));
		painter->drawText(x + 50, y + 30, "XXXXXXXXXXXXXXXXXXXXXXXXX");
		painter->setFont(QFont(QString(), 8));
		painter->drawText(x + 50, y + 50, "XXXXXXXXXXXXXXXXXXXXXXXXX");
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

TableList::TableList()
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

	QItemSelectionModel *ism = listview->selectionModel();
	connect(ism, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(slotActivated(const QItemSelection&, const QItemSelection&)));

	m_deletionmode = false;
}

void TableList::addConfiguration(QList<KGGZCore::Player> seats)
{
	m_seats.append(seats);

	//GGZProfile *profptr = new GGZProfile(profile);
	//profptr->assign(profile);

	//m_profptrs.append(profptr);

	QStandardItem *item = new QStandardItem();
	//item->setData(QVariant::fromValue<void*>(profptr), ItemDelegate::TableRole);
	item->setData(QPixmap(), ItemDelegate::PixmapRole);
	m_model->appendRow(item);

	//GGZServer server = profile.ggzServer();
	//m_apixmaps[server.icon()] = item;

	/*QAsyncPixmap *apixmap = new QAsyncPixmap(server.icon(), this);
	if(!apixmap->isNull())
	{
		QPixmap pix = apixmap->scaled(QSize(32, 32), Qt::KeepAspectRatio);
		item->setData(pix, ItemDelegate::PixmapRole);
	}

	connect(apixmap,
		SIGNAL(signalLoaded(const QString&, const QPixmap&)),
		SLOT(slotLoaded(const QString&, const QPixmap&)));*/
}

/*void TableList::slotLoaded(const QString& url, const QPixmap& pixmap)
{
	QStandardItem *item = m_apixmaps[url];
	if(!item)
		return;

	QPixmap pix = pixmap.scaled(QSize(32, 32), Qt::KeepAspectRatio);
	item->setData(pix, ItemDelegate::PixmapRole);
}*/

void TableList::removeConfiguration(int pos)
{
	if(pos == -1)
		return;

	// FIXME: This whole method is very complicated but apparently
	// there are some bugs in QStandardItemModel which do not work
	// intuitive enough.

	m_deletionmode = true;

	m_seats.removeAt(pos);
	QStandardItem *item = m_model->takeItem(pos);
	delete item;

	m_model->removeRow(pos);

	//GGZProfile *profptr = m_profptrs.takeAt(pos);
	//delete profptr;

	int newpos = pos - 1;
	if(newpos == -1)
		newpos = 0;
	if(newpos < m_seats.size())
	{
		QList<KGGZCore::Player> seats = m_seats.at(newpos);
		//emit signalSelected(seats, newpos);
	}
	else
	{
		//emit signalSelected(GGZProfile(), -1);
	}

	m_deletionmode = false;
}

void TableList::updateConfiguration(QList<KGGZCore::Player> seats, int pos)
{
	if(m_deletionmode)
		return;

	if(pos == -1)
		return;
	if(pos >= m_seats.size())
		return;

	m_seats[pos] = seats;

	QStandardItem *item = m_model->item(pos);

	//GGZProfile *oldprofile = reinterpret_cast<GGZProfile*>(item->data(ItemDelegate::TableRole).value<void*>());
	//oldprofile->assign(profile);
	//item->setData(QVariant::fromValue<void*>(oldprofile), ItemDelegate::TableRole);
}

void TableList::slotActivated(const QItemSelection& selected, const QItemSelection& deselected)
{
	if(m_deletionmode)
		return;

	QModelIndexList desel_indexes = deselected.indexes();
	QModelIndexList sel_indexes = selected.indexes();
	if(sel_indexes.size() == 0)
	{
		for(int i = 0; i < desel_indexes.size(); i++)
		{
			//GGZProfile profile;
			//emit signalSelected(profile, -1);
		}
	}
	for(int i = 0; i < sel_indexes.size(); i++)
	{
		QModelIndex index = sel_indexes.at(i);
		QStandardItem *item = m_model->itemFromIndex(index);

		//GGZProfile *profile = reinterpret_cast<GGZProfile*>(item->data(ItemDelegate::TableRole).value<void*>());
		//emit signalSelected(*profile, index.row());
	}
}

QList<KGGZCore::Player> TableList::configuration()
{
	if(m_seats.size() > 0)
		return m_seats.at(0);
	QList<KGGZCore::Player> seats;
	return seats;
}

void TableList::clear()
{
	m_seats.clear();
	//m_apixmaps.clear();
	m_model->clear();
	//qDeleteAll(m_profptrs);
	//m_profptrs.clear();
}
