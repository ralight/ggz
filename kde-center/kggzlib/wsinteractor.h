#ifndef WS_INTERACTOR_H
#define WS_INTERACTOR_H

// Qt includes
#include <QObject>
#include <QMap>
#include <QByteArray>

#include "kggzlib_export.h"

namespace KIO
{
	class Job;
}

class KGGZLIB_EXPORT WSInteractor : public QObject
{
	Q_OBJECT
	public:
		WSInteractor(QObject *parent = NULL);
		void post(QString url, QByteArray data);
		void remove(QString url);
	private slots:
		void slotJob(KIO::Job *job, QByteArray &data);
	private:
		QMap<KIO::Job*, QByteArray> m_data;
};

#endif

