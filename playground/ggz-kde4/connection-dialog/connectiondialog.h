#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

class QStandardItem;

// The game window
class ConnectionDialog : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionDialog();

	private slots:
		void slotLoaded(const QString& url, const QPixmap& pixmap);

	private:
		QMap<QString, QStandardItem*> m_apixmaps;
};

#endif

