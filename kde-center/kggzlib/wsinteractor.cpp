// kggzlib includes
#include "wsinteractor.h"

// Qt includes
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/deletejob.h>

WSInteractor::WSInteractor(QObject *parent)
: QObject(parent)
{
}

void WSInteractor::post(QString url, QByteArray data)
{
	qDebug("POST DATA: '%s' AT URL: '%s'", data.data(), qPrintable(url));
	// FIXME: actually we would need put for buddy/ignored lists, and post only for karma updates etc.

	KIO::TransferJob *job = KIO::http_post(url, data, KIO::DefaultFlags);
	job->addMetaData("content-type", "Content-type: application/ggzapi+xml");
	/* also: KIO::HideProgressInfo, KIO::Overwrite(?) */

	//connect(job,
	//	SIGNAL(dataReq(KIO::Job*, QByteArray&)),
	//	SLOT(slotJob(KIO::Job*, QByteArray)));

	//m_data[job] = data;
}

void WSInteractor::remove(QString url)
{
	// FIXME: KIO tries to remove local file here for some reason
	qDebug("REMOVE URL: '%s'", qPrintable(url));

	KIO::DeleteJob *job = KIO::del(url, KIO::DefaultFlags);
	Q_UNUSED(job);
}

void WSInteractor::slotJob(KIO::Job *job, QByteArray &data)
{
	data = m_data[job];
	m_data.remove(job);
}

