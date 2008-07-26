#ifndef PLASMA_KGGZ_ROOMSELECTOR_HEADER
#define PLASMA_KGGZ_ROOMSELECTOR_HEADER

#include <qdialog.h>

#include "ggzserver.h"

class QListWidget;
class QPushButton;

class RoomSelector : public QDialog
{
	Q_OBJECT

	public:
		RoomSelector(QWidget *parent);
		~RoomSelector();

		void setGGZUri(QString uri);
		QString room();

	signals:
		void signalRoomSelected(const QString &room);

	private slots:
		void slotRoomSelected();
		void slotSelectionChanged();

	private:
		QListWidget *m_roomlist;
		QPushButton *m_button;
};

#endif
