// Table configuration list includes
#include "tablelist.h"

// KGGZ includes
#include "qasyncpixmap.h"
#include "util.h"
#include "tableconfiguration.h"
#include <kggzcore/player.h>
#include <kggzcore/table.h>

// KDE includes
#include <kstandarddirs.h>
#include <klocale.h>

// Qt includes
#include <qlistview.h>
#include <qitemdelegate.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>

Q_DECLARE_METATYPE(KGGZCore::Table);

class TableItemDelegate : public QItemDelegate
{
public:
	enum Roles
	{
		TableRole = Qt::UserRole + 1,
		PixmapRole = Qt::UserRole + 2
	};

	TableItemDelegate(QWidget *parent = NULL)
	: QItemDelegate(parent)
	{
		size = 64;
	}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		KStandardDirs d;

		if(option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		KGGZCore::Table table = index.data(TableRole).value<KGGZCore::Table>();
		//QPixmap pix = index.data(PixmapRole).value<QPixmap>();

		int x = option.rect.left();
		int y = option.rect.top();

		int numplayers = 0;
		int numbots = 0;

		/*GGZProfile::LoginType logintype = profile->loginType();
		if(logintype == GGZProfile::guest)
			loginstring = "You'll play as a guest.";
		else if(logintype == GGZProfile::registered)
			loginstring = "You're already registered.";
		else if(logintype == GGZProfile::firsttime)
			loginstring = "You'll apply for an account.";
		else
			loginstring = "Unconfigured login profile.";*/

		painter->save();
		for(int i = 0; i < table.players().size(); i++)
		{
			KGGZCore::Player player = table.players().at(i);
			QString pixmap;
			if(player.type() == KGGZCore::Player::open)
			{
				pixmap = "ox-guest.png";
				numplayers++;
			}
			else if(player.type() == KGGZCore::Player::reserved)
			{
				pixmap = "ox-player.png";
				numplayers++;
			}
			else if(player.type() == KGGZCore::Player::bot)
			{
				pixmap = "ox-bot.png";
				numbots++;
			}
			else if(player.type() == KGGZCore::Player::player)
			{
				pixmap = "ox-self.png";
			}

			QPixmap pix(d.findResource("data", "kggzlib/players/" + pixmap));
			pix = pix.scaled(QSize(size, size), Qt::KeepAspectRatio);
			/*if(player.type() == KGGZCore::Player::player)
			{
				pixmap = "you.png";
				QPixmap pixoverlay(d.findResource("data", "kggzlib/players/" + pixmap));
				pixoverlay = pixoverlay.scaled(QSize(size, size), Qt::KeepAspectRatio);
				pix = Util::composite(pix, pixoverlay);
			}*/
			painter->drawPixmap(x + 10 + i * (size + 3), y + 60, pix);
		}

		QString infostring;
		if((numbots > 0) && (numplayers > 0))
			infostring = i18n("Playing against %1 players and %2 computer players.", numplayers, numbots);
		else if(numbots > 0)
			infostring = i18n("Playing alone against %1 computer players.", numbots);
		else if(numplayers > 0)
			infostring = i18n("Playing against %1 players.", numplayers);

		//painter->drawPixmap(x + 10, y + 29, pix);
		painter->setFont(QFont(QString(), 15));
		painter->drawText(x + 10, y + 30, table.description());
		painter->setFont(QFont(QString(), 8));
		//painter->drawText(x + 50, y + 50, "XXXXXXXXXXXXXXXXXXXXXXXXX");
		painter->drawText(x + 10, y + 50, infostring);
		painter->restore();
	}

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		Q_UNUSED(option);
		Q_UNUSED(index);

		return QSize(30 + size * 2, 60 + size);
	}

private:
	int size;
};

TableList::TableList()
: QWidget()
{
	QListView *listview = new QListView();

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(listview);
	setLayout(vbox);

	m_model = new QStandardItemModel();

	TableItemDelegate *delegate = new TableItemDelegate(this);

	listview->setModel(m_model);
	listview->setItemDelegate(delegate);
	listview->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QItemSelectionModel *ism = listview->selectionModel();
	connect(ism,
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		SLOT(slotActivated(const QItemSelection&, const QItemSelection&)));
	connect(listview,
		SIGNAL(doubleClicked(const QModelIndex&)),
		SLOT(slotDetails(const QModelIndex&)));

	m_deletionmode = false;
}

void TableList::addConfiguration(const KGGZCore::Table& table)
{
	QVariant tablevariant = QVariant::fromValue(table);

	QStandardItem *item = new QStandardItem();
	item->setData(tablevariant, TableItemDelegate::TableRole);
	item->setData(QPixmap(), TableItemDelegate::PixmapRole);
	m_model->appendRow(item);

	/*GGZServer server = profile.ggzServer();
	m_apixmaps[server.icon()] = item;

	QAsyncPixmap *apixmap = new QAsyncPixmap(server.icon(), this);
	if(!apixmap->isNull())
	{
		QPixmap pix = apixmap->scaled(QSize(32, 32), Qt::KeepAspectRatio);
		item->setData(pix, TableItemDelegate::PixmapRole);
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
	item->setData(pix, TableItemDelegate::PixmapRole);
}*/

void TableList::removeConfiguration(int pos)
{
	if(pos == -1)
		return;

	// FIXME: See ServerList for QStandardItemMode issues

	m_deletionmode = true;

	QStandardItem *item = m_model->takeItem(pos);
	delete item;

	m_model->removeRow(pos);

	int newpos = pos - 1;
	if(newpos == -1)
		newpos = 0;
	if(newpos < m_model->rowCount())
	{
		QStandardItem *item = m_model->item(pos);
		KGGZCore::Table table = item->data(TableItemDelegate::TableRole).value<KGGZCore::Table>();
		emit signalSelected(table, newpos);
	}
	else
	{
		emit signalSelected(KGGZCore::Table(), -1);
	}

	m_deletionmode = false;
}

void TableList::updateConfiguration(const KGGZCore::Table& table, int pos)
{
	if(m_deletionmode)
		return;

	if(pos == -1)
		return;
	if(pos >= m_model->rowCount())
		return;

	QStandardItem *item = m_model->item(pos);

	QVariant tablevariant = QVariant::fromValue(table);
	item->setData(tablevariant, TableItemDelegate::TableRole);
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
			emit signalSelected(KGGZCore::Table(), -1);
		}
	}
	for(int i = 0; i < sel_indexes.size(); i++)
	{
		QModelIndex index = sel_indexes.at(i);
		QStandardItem *item = m_model->itemFromIndex(index);

		KGGZCore::Table table = item->data(TableItemDelegate::TableRole).value<KGGZCore::Table>();
		emit signalSelected(table, index.row());
	}
}

void TableList::slotDetails(const QModelIndex& index)
{
	QStandardItem *item = m_model->itemFromIndex(index);
	KGGZCore::Table table = item->data(TableItemDelegate::TableRole).value<KGGZCore::Table>();

	TableConfiguration tableconf(this, true);
	QList<KGGZCore::Player> players = table.players();
	// FIXME: get player's identity here so we can highlight the player's own seat
	tableconf.initLauncher(QString(), players.size(), 0);
	for(int i = 0; i < players.size(); i++)
	{
		KGGZCore::Player player = players.at(i);
		TableConfiguration::SeatTypes type = TableConfiguration::seatunknown;
		if(player.type() == KGGZCore::Player::open)
			type = TableConfiguration::seatopen;
		else if(player.type() == KGGZCore::Player::bot)
			type = TableConfiguration::seatbot;
		else if(player.type() == KGGZCore::Player::player)
			type = TableConfiguration::seatplayer;
		else if(player.type() == KGGZCore::Player::reserved)
			type = TableConfiguration::seatplayerreserved;
		else if(player.type() == KGGZCore::Player::abandoned)
			type = TableConfiguration::seatplayerreserved;
		tableconf.setSeatType(i, type);
		if((player.type() == KGGZCore::Player::reserved)
		|| (player.type() == KGGZCore::Player::reserved))
			tableconf.setReservation(i, player.name());
		// FIXME: no way to distinguish from bot/buddy reservations here
	}
	tableconf.setDescription(table.description());
	tableconf.exec();
}

/*KGGZCore::Table TableList::configuration() const
{
	return KGGZCore::Table(QString());
}*/

void TableList::clear()
{
	//m_apixmaps.clear();
	m_model->clear();
}
