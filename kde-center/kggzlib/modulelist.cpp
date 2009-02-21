// Table configuration list includes
#include "modulelist.h"

// KGGZ includes
#include "qasyncpixmap.h"
#include "util.h"
#include <kggzcore/module.h>

// KDE includes
#include <kstandarddirs.h>
#include <klocale.h>

// Qt includes
#include <qlistview.h>
#include <qitemdelegate.h>
#include <qstandarditemmodel.h>
#include <qlayout.h>
#include <qpainter.h>

Q_DECLARE_METATYPE(KGGZCore::Module);

class ModuleItemDelegate : public QItemDelegate
{
public:
	enum Roles
	{
		ModuleRole = Qt::UserRole + 1,
		PixmapRole = Qt::UserRole + 2
	};

	ModuleItemDelegate(QWidget *parent = NULL)
	: QItemDelegate(parent)
	{
		size = 64;
	}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		KStandardDirs d;

		if(option.state & QStyle::State_Selected)
			painter->fillRect(option.rect, option.palette.highlight());

		KGGZCore::Module module = index.data(ModuleRole).value<KGGZCore::Module>();

		int x = option.rect.left();
		int y = option.rect.top();

		painter->save();
		painter->setFont(QFont(QString(), 15));
		painter->drawText(x + 10, y + 30, module.name());
		painter->setFont(QFont(QString(), 8));
		painter->drawText(x + 10, y + 50, module.frontend());
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

ModuleList::ModuleList()
: QWidget()
{
	QListView *listview = new QListView();

	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(listview);
	setLayout(vbox);

	m_model = new QStandardItemModel();

	ModuleItemDelegate *delegate = new ModuleItemDelegate(this);

	listview->setModel(m_model);
	listview->setItemDelegate(delegate);
	listview->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QItemSelectionModel *ism = listview->selectionModel();
	connect(ism,
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		SLOT(slotActivated(const QItemSelection&, const QItemSelection&)));
}

void ModuleList::addModule(const KGGZCore::Module& module)
{
	QVariant modulevariant = QVariant::fromValue(module);

	QStandardItem *item = new QStandardItem();
	item->setData(modulevariant, ModuleItemDelegate::ModuleRole);
	item->setData(QPixmap(), ModuleItemDelegate::PixmapRole);
	m_model->appendRow(item);
}

void ModuleList::slotActivated(const QItemSelection& selected, const QItemSelection& deselected)
{
	QModelIndexList desel_indexes = deselected.indexes();
	QModelIndexList sel_indexes = selected.indexes();
	if(sel_indexes.size() == 0)
	{
		for(int i = 0; i < desel_indexes.size(); i++)
		{
			emit signalSelected(KGGZCore::Module());
		}
	}
	for(int i = 0; i < sel_indexes.size(); i++)
	{
		QModelIndex index = sel_indexes.at(i);
		QStandardItem *item = m_model->itemFromIndex(index);

		KGGZCore::Module module = item->data(ModuleItemDelegate::ModuleRole).value<KGGZCore::Module>();
		emit signalSelected(module);
	}
}

