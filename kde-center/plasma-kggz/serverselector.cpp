#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>

#include <qtcpsocket.h>
#include <qurl.h>
#include <qxmlstream.h>
#include <qdom.h>

#include <klocale.h>
#include <kmessagebox.h>

#include "serverselector.h"

#define GGZ_PROTOCOL_VERSION "10"

ServerSelector::ServerSelector(QWidget *parent)
: QDialog(parent), m_sock(NULL)
{
	m_serverlist = new QListWidget();

	m_button = new QPushButton(i18n("Select this server"));
	m_button->setEnabled(false);

	QPushButton *cancel = new QPushButton(i18n("Cancel"));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(m_button);
	hbox->addWidget(cancel);

	QVBoxLayout *vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addWidget(m_serverlist);
	vbox->addLayout(hbox);

	connect(m_button, SIGNAL(clicked()), SLOT(slotServerSelected()));
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	connect(m_serverlist, SIGNAL(itemSelectionChanged()), SLOT(slotSelectionChanged()));

	setWindowTitle(i18n("Server selection from metaserver"));
	show();
}

ServerSelector::~ServerSelector()
{
}

GGZServer ServerSelector::server()
{
	GGZServer server;
	QListWidgetItem *item = m_serverlist->currentItem();

	if(item)
		return m_servers[item->text()];

	return server;
}

void ServerSelector::slotServerSelected()
{
	QListWidgetItem *item = m_serverlist->currentItem();

	if(item)
	{
		emit signalServerSelected(m_servers[item->text()]);
		accept();
	}
}

void ServerSelector::slotSelectionChanged()
{
	QListWidgetItem *item = m_serverlist->currentItem();

	m_button->setEnabled((item != NULL));
}

void ServerSelector::setMetaUri(QString uri)
{
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

	qDebug("Query: %s", qPrintable(query));

	m_sock->write(query.toUtf8());
}

void ServerSelector::slotData()
{
	QByteArray raw = m_sock->readAll();
	QString response = QString::fromUtf8(raw.data());

	qDebug("Response: %s", qPrintable(response));

	QDomDocument dom;
	if(!dom.setContent(response))
	{
		KMessageBox::error(this,
			i18n("The metaserver didn't return a useful response."),
			i18n("Metaserver failure"));
		return;
	}

	QDomElement resultset = dom.documentElement();
	QDomNode resultnode = resultset.firstChild();
	while(!resultnode.isNull())
	{
		QDomElement result = resultnode.toElement();
		QString uri = result.attribute("uri");

		QListWidgetItem *item = new QListWidgetItem(uri);
		m_serverlist->addItem(item);

		QString api;

		QDomNode optionnode = result.firstChild();
		while(!optionnode.isNull())
		{
			QDomElement option = optionnode.toElement();
			QString optname = option.attribute("name");

			if(optname == "api")
				api = option.text();

			optionnode = optionnode.nextSibling();
		}

		GGZServer server;
		server.setUri(uri);
		server.setApi(api);
		m_servers[uri] = server;

		resultnode = resultnode.nextSibling();
	}
}

void ServerSelector::slotError(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error);

	KMessageBox::error(this,
		i18n("The list of servers cannot be retrieved."),
		i18n("Metaserver failure"));
}

#include "serverselector.moc"
