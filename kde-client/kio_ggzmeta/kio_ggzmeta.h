#ifndef KIO_GGZMETA_H
#define KIO_GGZMETA_H

#include <kio/slavebase.h>
#include <kio/jobclasses.h>

class GGZMetaProtocol : public QObject, public KIO::SlaveBase
{
	Q_OBJECT
	public:
		GGZMetaProtocol(const QCString& pool, const QCString& app);
		~GGZMetaProtocol();
		void get(const KURL& url);
		void listDir(const KURL& url);

		void work(QString queryclass, QString query);
		void delegate(QString queryclass, QString url);

	public slots:
		void slotResult(KIO::Job *job);

	private:
		void jobOperator(const KURL& url);
		void init(const KURL& url);
		void debug(QString s);

		KIO::UDSEntry entry;
		QString m_class;
};

#endif

