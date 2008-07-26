#ifndef PLASMA_KGGZ_SERVERSELECTOR_HEADER
#define PLASMA_KGGZ_SERVERSELECTOR_HEADER

#include <qdialog.h>

#include "ggzserver.h"

class QListWidget;
class QPushButton;

class ServerSelector : public QDialog
{
	Q_OBJECT

	public:
		ServerSelector(QWidget *parent);
		~ServerSelector();

		void setMetaUri(QString uri);
		QString server();

	signals:
		void signalServerSelected(const QString &server);

	private slots:
		void slotServerSelected();
		void slotSelectionChanged();

	private:
		QListWidget *m_serverlist;
		QPushButton *m_button;
};

#endif
