#include "serverselector.h"

#include <QtGui/QPushButton>
#include <QtGui/QLayout>

#include <qtcpsocket.h>
#include <qurl.h>
#include <qxmlstream.h>
#include <qdom.h>
#include <qlabel.h>
#include <qprogressbar.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include "serverlist.h"

#define GGZ_PROTOCOL_VERSION "10"

ServerSelector::ServerSelector(QWidget *parent)
: QDialog(parent), m_sock(NULL)
{
	KStandardDirs d;

	m_serverlist = new ServerList();

	m_button = new QPushButton(i18n("Select this server"));
	m_button->setEnabled(false);

	QPushButton *cancel = new QPushButton(i18n("Cancel"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(m_button);
	hbox->addWidget(cancel);

	QLabel *logo = new QLabel();
	logo->setFixedSize(64, 64);
	logo->setPixmap(QPixmap(d.findResource("data", "kggzlib/icons/ggzmetaserv.png")));

	m_indicator = new QProgressBar();
	m_indicator->setEnabled(false);
	m_indicator->setMinimum(0);
	m_indicator->setMaximum(0);

	QHBoxLayout *hboxtop = new QHBoxLayout();
	hboxtop->addWidget(logo);
	hboxtop->addStretch(1);

	QVBoxLayout *vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addLayout(hboxtop);
	vbox->addWidget(m_serverlist);
	vbox->addWidget(m_indicator);
	vbox->addLayout(hbox);

	connect(m_button, SIGNAL(clicked()), SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(m_serverlist, SIGNAL(signalSelected(const GGZProfile&, int)), SLOT(slotSelected(const GGZProfile&, int)));

	setWindowTitle(i18n("Server selection from metaserver"));
	resize(400, 550);
	show();
}

ServerSelector::~ServerSelector()
{
}

GGZServer ServerSelector::server()
{
	return m_server;
}

void ServerSelector::slotSelected(const GGZProfile& profile, int pos)
{
	m_server = profile.ggzServer();
	m_button->setEnabled((pos != -1));
}

void ServerSelector::setMetaUri(QString uri)
{
	m_indicator->setEnabled(true);

	QUrl url(uri);
	m_sock = new QTcpSocket(this);
	connect(m_sock, SIGNAL(connected()), SLOT(slotConnected()));
	connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(slotError(QAbstractSocket::SocketError)));
	connect(m_sock, SIGNAL(readyRead()), SLOT(slotData()));
	m_sock->connectToHost(url.host(), url.port(15689));
}

void ServerSelector::slotConnected()
{
	QString query;
	QXmlStreamWriter writer(&query);
	writer.writeStartElement("query");
	writer.writeAttribute("class", "ggz");
	writer.writeAttribute("type", "connection");
	writer.writeStartElement("option");
	writer.writeAttribute("name", "protocol");
	writer.writeCharacters(GGZ_PROTOCOL_VERSION);
	writer.writeEndElement();
	writer.writeEndElement();

	//qDebug("Query: %s", qPrintable(query));

	m_sock->write(query.toUtf8());
}

void ServerSelector::slotData()
{

	QByteArray raw = m_sock->readAll();
	QString response = QString::fromUtf8(raw.data());

	//qDebug("Response: %s", qPrintable(response));

	QDomDocument dom;
	if(!dom.setContent(response))
	{
		KMessageBox::error(this,
			i18n("The metaserver didn't return a useful response."),
			i18n("Metaserver failure"));
		m_indicator->setEnabled(false);
		return;
	}

	QDomElement resultset = dom.documentElement();
	QDomNode resultnode = resultset.firstChild();
	while(!resultnode.isNull())
	{
		QDomElement result = resultnode.toElement();
		QString uri = result.attribute("uri");

		QString api;
		QString logo;
		QString name;

		QDomNode optionnode = result.firstChild();
		while(!optionnode.isNull())
		{
			QDomElement option = optionnode.toElement();
			QString optname = option.attribute("name");

			if(optname == "api")
				api = option.text();
			else if(optname == "logo")
				logo = option.text();
			else if(optname == "name")
				name = option.text();

			optionnode = optionnode.nextSibling();
		}

		GGZServer server;
		server.setUri(uri);
		server.setName(name);
		server.setApi(api);
		server.setIcon(logo);
		GGZProfile profile;
		profile.setGGZServer(server);
		m_serverlist->addProfile(profile);

		resultnode = resultnode.nextSibling();
	}

	m_indicator->setEnabled(false);
	m_indicator->setMaximum(1);
}

void ServerSelector::slotError(QAbstractSocket::SocketError error)
{
	m_indicator->setEnabled(false);

	Q_UNUSED(error);

	KMessageBox::error(this,
		i18n("The list of servers cannot be retrieved."),
		i18n("Metaserver failure"));
}

#include "serverselector.moc"
