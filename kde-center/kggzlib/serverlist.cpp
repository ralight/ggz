// Server list includes
#include "serverlist.h"

// KGGZ includes
#include "qasyncpixmap.h"
#include "ggzprofile.h"

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
		ProfileRole = Qt::UserRole + 1,
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

		GGZProfile *profile = reinterpret_cast<GGZProfile*>(index.data(ProfileRole).value<void*>());

		QPixmap pix = index.data(PixmapRole).value<QPixmap>();
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
		GGZProfile::LoginType logintype = profile->loginType();
		if(logintype == GGZProfile::guest)
			loginstring = "You'll play as a guest.";
		else if(logintype == GGZProfile::registered)
			loginstring = "You're already registered.";
		else if(logintype == GGZProfile::firsttime)
			loginstring = "You'll apply for an account.";
		else
			loginstring = "Unconfigured login profile.";

		QString username = profile->username();
		if(!username.isEmpty())
			loginstring += " [" + username + "]";

		painter->save();
		painter->drawPixmap(x + 10, y + 29, pix);
		painter->setFont(QFont(QString(), 15));
		painter->drawText(x + 50, y + 30, profile->ggzServer().name());
		painter->setFont(QFont(QString(), 8));
		painter->drawText(x + 50, y + 50, profile->ggzServer().uri());
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

	QItemSelectionModel *ism = listview->selectionModel();
	connect(ism, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(slotActivated(const QItemSelection&, const QItemSelection&)));
}

void ServerList::addProfile(const GGZProfile& profile)
{
	m_profiles.append(profile);

	GGZProfile *profptr = new GGZProfile(profile);
	profptr->assign(profile);

	m_profptrs.append(profptr);

	QStandardItem *item = new QStandardItem();
	item->setData(QVariant::fromValue<void*>(profptr), ItemDelegate::ProfileRole);
	item->setData(QPixmap(), ItemDelegate::PixmapRole);
	m_model->appendRow(item);

	GGZServer server = profile.ggzServer();
	m_apixmaps[server.icon()] = item;

	QAsyncPixmap *apixmap = new QAsyncPixmap(server.icon(), this);
	if(!apixmap->isNull())
	{
		QPixmap pix = apixmap->scaled(QSize(32, 32), Qt::KeepAspectRatio);
		item->setData(pix, ItemDelegate::PixmapRole);
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

	QPixmap pix = pixmap.scaled(QSize(32, 32), Qt::KeepAspectRatio);
	item->setData(pix, ItemDelegate::PixmapRole);
}

void ServerList::updateProfile(const GGZProfile& profile, int pos)
{
	if(pos == -1)
		return;

	m_profiles[pos] = profile;

	QStandardItem *item = m_model->item(pos);

	GGZProfile *oldprofile = reinterpret_cast<GGZProfile*>(item->data(ItemDelegate::ProfileRole).value<void*>());
	oldprofile->assign(profile);
	item->setData(QVariant::fromValue<void*>(oldprofile), ItemDelegate::ProfileRole);
}

void ServerList::slotActivated(const QItemSelection& selected, const QItemSelection& deselected)
{
	QModelIndexList desel_indexes = deselected.indexes();
	QModelIndexList sel_indexes = selected.indexes();
	if(sel_indexes.size() == 0)
	{
		for(int i = 0; i < desel_indexes.size(); i++)
		{
			GGZProfile profile;
			emit signalSelected(profile, -1);
		}
	}
	for(int i = 0; i < sel_indexes.size(); i++)
	{
		QModelIndex index = sel_indexes.at(i);
		QStandardItem *item = m_model->itemFromIndex(index);

		GGZProfile *profile = reinterpret_cast<GGZProfile*>(item->data(ItemDelegate::ProfileRole).value<void*>());
		emit signalSelected(*profile, index.row());
	}
}

QList<GGZProfile> ServerList::profiles()
{
	return m_profiles;
}

void ServerList::clear()
{
	m_profiles.clear();
	m_apixmaps.clear();
	m_model->clear();
	qDeleteAll(m_profptrs);
	m_profptrs.clear();
}
