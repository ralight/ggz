#ifndef PLASMA_KGGZ_ROOMSELECTOR_HEADER
#define PLASMA_KGGZ_ROOMSELECTOR_HEADER

#include <qdialog.h>

class QListWidget;
class QPushButton;

namespace KIO
{
	class Job;
};
class KJob;

class RoomSelector : public QDialog
{
	Q_OBJECT

	public:
		RoomSelector(QWidget *parent);
		~RoomSelector();

		void setGGZApi(QString uri);
		QString room();

	signals:
		void signalRoomSelected(const QString &room);

	private slots:
		void slotRoomSelected();
		void slotSelectionChanged();

		void slotData(KIO::Job *job, const QByteArray &data);
		void slotResult(KJob *job);

	private:
		QListWidget *m_roomlist;
		QPushButton *m_button;
		QByteArray m_data;
};

#endif
