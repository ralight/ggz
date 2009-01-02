// Configuration
#include "config.h"

// Player list includes
#include "playerlist.h"
#include "qrecursivesortfilterproxymodel.h"
#include "player.h"
#include "util.h"

// Lokarest includes
#ifdef LOKAREST_FOUND
#include <lokarest/lokarest.h>
#endif

// KDE includes
#include <kstandarddirs.h>
#include <klocale.h>
#include <ktoolinvocation.h>

// Qt includes
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmenu.h>

#include <qdom.h>

PlayerList::PlayerList()
: QWidget()
{
	m_treeview = new QTreeView();
	m_treeview->setContextMenuPolicy(Qt::CustomContextMenu);
	m_treeview->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QLineEdit *searchbox = new QLineEdit();
	QLabel *searchlabel = new QLabel(i18n("Search for:"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(searchlabel);
	hbox->addWidget(searchbox);

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(m_treeview);
	vbox->addLayout(hbox);
	setLayout(vbox);

	m_model = new QStandardItemModel();
	m_model->setColumnCount(2);

	m_itemfriends = new QStandardItem();
	m_itemfriends->setText(i18n("Friends"));
	m_model->appendRow(m_itemfriends);

	m_itemignored = new QStandardItem();
	m_itemignored->setText(i18n("Ignored"));
	m_model->appendRow(m_itemignored);

	m_itemothers = new QStandardItem();
	m_itemothers->setText(i18n("Others"));
	m_model->appendRow(m_itemothers);

	m_model->setHeaderData(0, Qt::Horizontal, i18n("Player"), Qt::DisplayRole);
	m_model->setHeaderData(1, Qt::Horizontal, i18n("Properties"), Qt::DisplayRole);

	m_proxymodel = new QRecursiveSortFilterProxyModel(this);
	m_proxymodel->setSourceModel(m_model);
	m_proxymodel->setDynamicSortFilter(true);

	m_treeview->setModel(m_proxymodel);
	m_treeview->expandAll();

#ifdef LOKAREST_FOUND
	m_interactor = new LokaRest(this);
#endif

	connect(searchbox, SIGNAL(textChanged(const QString&)), SLOT(slotSearch(const QString&)));
	connect(m_treeview, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotSelected(const QPoint&)));

	//setWindowTitle("GGZ gets a more flexible player list!");
	resize(320, 300);
	//show();
}

void PlayerList::setGGZProfile(const GGZProfile& ggzprofile)
{
	m_ggzprofile = ggzprofile;
}

void PlayerList::addPlayer(Player *player)
{
	KStandardDirs d;

	m_players[player->name()] = player;

	QString pixmap = "ox-guest.png";
	if(player->role() == Player::Admin)
		pixmap = "ox-admin.png";
	else if(player->role() == Player::Host)
		pixmap = "host.png";
	else if(player->role() == Player::Registered)
		pixmap = "player.png";
	else if(player->role() == Player::Bot)
		pixmap = "ox-bot.png";

	QString lagpixmap = "lag0.png";
	if(player->lag() == 100)
		lagpixmap = "lag5.png";
	else if(player->lag() > 80)
		lagpixmap = "lag4.png";
	else if(player->lag() > 60)
		lagpixmap = "lag3.png";
	else if(player->lag() > 40)
		lagpixmap = "lag2.png";
	else if(player->lag() > 20)
		lagpixmap = "lag1.png";

	QPixmap pix(d.findResource("data", "kggzlib/players/" + pixmap));

	if(player->relation() == Player::Buddy)
	{
		pixmap = "ox-buddy.png";
		QPixmap pix2(d.findResource("data", "kggzlib/players/" + pixmap));
		pix = Util::composite(pix, pix2);
	}
	else if(player->relation() == Player::Ignored)
	{
		pixmap = "ox-banned.png";
		QPixmap pix2(d.findResource("data", "kggzlib/players/" + pixmap));
		pix = Util::composite(pix, pix2);
	}

	QStandardItem *itemname = new QStandardItem();
	itemname->setIcon(QIcon(pix));
	itemname->setText(player->name());

	QStandardItem *itemlagstats = new QStandardItem();
	itemlagstats->setIcon(QIcon(d.findResource("data", "kggzlib/players/" + lagpixmap)));
	itemlagstats->setText(player->statistics());

	QList<QStandardItem*> childitems;
	childitems << itemname;
	childitems << itemlagstats;
	mount(childitems, player);
}

void PlayerList::mount(QList<QStandardItem*> childitems, Player *player)
{
	QStandardItem *item = m_itemothers;
	if(player->relation() == Player::Buddy)
		item = m_itemfriends;
	else if(player->relation() == Player::Ignored)
		item = m_itemignored;

	item->appendRow(childitems);
}

void PlayerList::slotSearch(const QString& text)
{
	m_proxymodel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
	m_proxymodel->setFilterKeyColumn(0);
	m_treeview->expandAll();
}

void PlayerList::slotSelected(const QPoint& pos)
{
	QModelIndex index = m_treeview->indexAt(pos);
	if(!index.isValid())
		return;

	QString name = m_proxymodel->data(index).toString();
	if(m_players.contains(name))
	{
		Player *player = m_players[name];

		QAction *action_removebuddy = NULL;
		QAction *action_removeignored = NULL;
		QAction *action_addbuddy = NULL;
		QAction *action_addignored = NULL;
		QAction *action_chat = NULL;
		QAction *action_community = NULL;

		GGZServer ggzserver = m_ggzprofile.ggzServer();
		QString user = m_ggzprofile.username();
		QString pass = m_ggzprofile.password();
		QString baseurl = ggzserver.api() + "/api/players/" + user;

		QMenu menu;
		if(player->relation() == Player::Buddy)
		{
			action_removebuddy = menu.addAction(i18n("Remove from buddy list"));
		}
		else if(player->relation() == Player::Ignored)
		{
			action_removeignored = menu.addAction(i18n("Remove from list of ignored players"));
		}
		else
		{
			action_addignored = menu.addAction(i18n("Add to list of ignored players."));
			action_addbuddy = menu.addAction(i18n("Add to buddy list"));
		}
		menu.addSeparator();
		action_chat = menu.addAction(i18n("Private chat..."));
		if(!ggzserver.community().isEmpty())
			action_community = menu.addAction(i18n("Visit community profile..."));

		QAction *action = menu.exec(mapToGlobal(pos));
		if(action == action_removebuddy)
		{
			player->setRelation(Player::Unknown);
			QList<QStandardItem*> childitems = m_itemfriends->takeRow(index.row());
			mount(childitems, player);

			if(!ggzserver.api().isEmpty())
			{
				QString url = baseurl + "/buddies/" + name;
#ifdef LOKAREST_FOUND
				m_interactor->schedule(StateTransfer(StateTransfer::del, Resource(url, QString(), QByteArray())));
#endif
			}
		}
		else if(action == action_addbuddy)
		{
			player->setRelation(Player::Buddy);
			QList<QStandardItem*> childitems = m_itemothers->takeRow(index.row());
			mount(childitems, player);

			if(!ggzserver.api().isEmpty())
			{
				QString url = baseurl + "/buddies/" + name;
				QByteArray xmldata = playertoxml(player);
#ifdef LOKAREST_FOUND
				m_interactor->schedule(StateTransfer(StateTransfer::put, Resource(url, "application/ggzapi+xml", xmldata)));
#endif
			}
		}
		else if(action == action_removeignored)
		{
			player->setRelation(Player::Unknown);
			QList<QStandardItem*> childitems = m_itemignored->takeRow(index.row());
			mount(childitems, player);

			if(!ggzserver.api().isEmpty())
			{
				QString url = baseurl + "/ignored/" + name;
#ifdef LOKAREST_FOUND
				m_interactor->schedule(StateTransfer(StateTransfer::del, Resource(url, QString(), QByteArray())));
#endif
			}
		}
		else if(action == action_addignored)
		{
			player->setRelation(Player::Ignored);
			QList<QStandardItem*> childitems = m_itemothers->takeRow(index.row());
			mount(childitems, player);

			if(!ggzserver.api().isEmpty())
			{
				QString url = baseurl + "/ignored/" + name;
				QByteArray xmldata = playertoxml(player);
#ifdef LOKAREST_FOUND
				m_interactor->schedule(StateTransfer(StateTransfer::put, Resource(url, "application/ggzapi+xml", xmldata)));
#endif
			}
		}
		else if(action == action_chat)
		{
			// FIXME: private modal(?) k(ggz)chat window
		}
		else if(action == action_community)
		{
			QString playerurl = ggzserver.community() + "/db/players?lookup=" + name;
			KToolInvocation::invokeBrowser(playerurl);
		}
	}
}

QByteArray PlayerList::playertoxml(Player *player)
{
	QDomDocument xmldoc;
	QDomElement el = xmldoc.createElement("player");
	xmldoc.appendChild(el);
	QDomText txt = xmldoc.createTextNode(player->name());
	el.appendChild(txt);
	QByteArray xmldata = xmldoc.toByteArray();
	return xmldata;
}

