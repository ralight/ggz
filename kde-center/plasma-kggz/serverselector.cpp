#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>

#include <qtcpsocket.h>
#include <qurl.h>
#include <qxmlstream.h>

#include <klocale.h>
#include <kmessagebox.h>

#include "serverselector.h"

#define GGZ_PROTOCOL_VERSION "10"

ServerSelector::ServerSelector(QWidget *parent)
: QDialog(parent), m_sock(NULL)
{
	m_serverlist = new QListWidget();

	QListWidgetItem *item = new QListWidgetItem("ggz://live.ggzgamingzone.org");
	m_serverlist->addItem(item);

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

QString ServerSelector::server()
{
	QListWidgetItem *item = m_serverlist->currentItem();

	if(item)
		return item->text();

	return QString();
}

void ServerSelector::slotServerSelected()
{
	QListWidgetItem *item = m_serverlist->currentItem();

	if(item)
	{
		emit signalServerSelected(item->text());
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
}

void ServerSelector::slotError(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error);

	KMessageBox::error(this,
		i18n("The list of servers cannot be retrieved."),
		i18n("Metaserver failure"));
}

#include "serverselector.moc"
