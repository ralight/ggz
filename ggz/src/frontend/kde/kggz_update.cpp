#include "kggz_update.h"
#ifdef USE_FTP
#include <qnetwork.h>
#include <kmessagebox.h>
#include <qlayout.h>
#include <stdlib.h>
#include <klocale.h>
#include <ggzcore.h>
#endif

#ifdef CONFIG_H
#include "config.h"
#else
#define instdir "/usr/local/share/kggz/"
#endif

QListView *list;

void KGGZ_Update::ftpserver()
{
#ifdef USE_FTP
	int i, j;
	char **clist = NULL;
	char *tmp_ftpserver, *tmp_ftpdirectory;

	if(m_ftpserver) free(m_ftpserver);
	if(m_ftpdirectory) free(m_ftpdirectory);

	ggzcore_conf_read_list("FTP-Servers", "FTP-Servers", &i, &clist);
	for(j = 0; j < i; j++)
	{
		if(strcmp(clist[j], m_description) == 0)
		{
			tmp_ftpserver = ggzcore_conf_read_string(clist[j], "Host", "unknown host");
			tmp_ftpdirectory = ggzcore_conf_read_string(clist[j], "Path", "unknown directory");

			m_ftpserver = (char*)malloc(strlen(tmp_ftpserver) + 1);
			strcpy(m_ftpserver, tmp_ftpserver);
			m_ftpdirectory = (char*)malloc(strlen(tmp_ftpdirectory) + 1);
			strcpy(m_ftpdirectory, tmp_ftpdirectory);
		}
		free(clist[j]);
	}
	if(clist) free(clist);
#endif
}

KGGZ_Update::KGGZ_Update(QWidget *parent, char *name)
: QWidget(parent, name)
{
#ifdef USE_FTP
	QPushButton *close;
	QHBoxLayout *hbox;
	QVBoxLayout *vbox;
	int i, j;
	char **clist = NULL;

	m_ftpserver = NULL;
	m_ftpdirectory = NULL;
	m_description = NULL;

	qInitNetworkProtocols(); // don't miss this single line!!

	showfiles = new QPushButton(i18n("Search games"), this);
	download = new QPushButton(i18n("Download"), this);
	close = new QPushButton(i18n("Close"), this);
	list = new QListView(this);

	m_display = new QLabel(i18n("Ready"), this);
	m_progress = new QProgressBar(100, this);

	list->setRootIsDecorated(TRUE);

	// !!!
	//list->insertItem(new QListViewItem(list, "test"));
	list->addColumn(i18n("Game modules"));

	/*QListViewItem *tmp;
	tmp = new QListViewItem(list, "GGZ");
	tmp->setPixmap(0, QPixmap(QString(instdir) + "images/icons/server.png"));
	tmp->setText(1, "http://ggz.sourceforge.net");
	tmp->setText(2, "/pub/ggz/modules/");
	tmp = new QListViewItem(list, "MindX");
	tmp->setPixmap(0, QPixmap(QString(instdir) + "images/icons/server.png"));*/
	ggzcore_conf_read_list("FTP-Servers", "FTP-Servers", &i, &clist);
	for(j = 0; j < i; j++)
	{
		QListViewItem *tmp = new QListViewItem(list, clist[j]);
		tmp->setPixmap(0, QPixmap(QString(instdir) + "images/icons/server.png"));
		free(clist[j]);
	}
	if(clist) free(clist);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(list);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(showfiles);
	hbox->add(download);
	hbox->add(close);

	vbox->add(m_display);
	vbox->add(m_progress);

	connect(showfiles, SIGNAL(clicked()), SLOT(slotShowfiles()));
	connect(download, SIGNAL(clicked()), SLOT(slotDownload()));
	connect(close, SIGNAL(clicked()), SLOT(close()));

	setCaption(i18n("FTP Updates"));
	resize(300, 300);
	show();

	connect(&lop, SIGNAL(finished(QNetworkOperation *)), SLOT(slotLFinished(QNetworkOperation *)));
	connect(&lop, SIGNAL(start(QNetworkOperation *)), SLOT(slotLStart(QNetworkOperation *)));
	connect(&lop, SIGNAL(dataTransferProgress(int, int, QNetworkOperation *)), SLOT(slotLProgress(int, int, QNetworkOperation *)));
	connect(&rop, SIGNAL(finished(QNetworkOperation *)), SLOT(slotRFinished(QNetworkOperation *)));
	connect(&rop, SIGNAL(start(QNetworkOperation *)), SLOT(slotRStart(QNetworkOperation *)));
	connect(&rop, SIGNAL(dataTransferProgress(int, int, QNetworkOperation *)), SLOT(slotRProgress(int, int, QNetworkOperation *)));
	connect(&rop, SIGNAL(connectionStateChanged(int, const QString &)), SLOT(slotRConnectionStateChanged(int, const QString &)));
	connect(&rop, SIGNAL(newChildren(const QValueList<QUrlInfo> &, QNetworkOperation *)), SLOT(slotRChildren(const QValueList<QUrlInfo> &)));
	connect(&lop, SIGNAL(newChildren(const QValueList<QUrlInfo> &, QNetworkOperation *)), SLOT(slotRChildren(const QValueList<QUrlInfo> &))); // tmp
#endif
}

KGGZ_Update::~KGGZ_Update()
{
}

void KGGZ_Update::reset()
{
	download->setEnabled(TRUE);
	showfiles->setEnabled(TRUE);
	m_display->setText(i18n("Ready"));
	m_progress->setProgress(0);
}

void KGGZ_Update::slotLFinished(QNetworkOperation *op)
{
#ifdef USE_FTP
	printf("FTP (L): finished\n");
	if(op->state() == QNetworkProtocol::StFailed)
	{
		KMessageBox::information(this, i18n("The download was not successful."), i18n("Failed! (L)"));
	}
	else
	{
		KMessageBox::information(this, i18n("Downloading file finished."), i18n("Success! (L)"));
		if(KMessageBox::questionYesNo(this, i18n("Configure and install new game?"), i18n("Installation"), i18n("Yes"), i18n("No")) == KMessageBox::Yes)
		{
			printf("KGGZ_Install would go here...\n");
			m_install = new KGGZ_Install(NULL, "kggz_install");
			m_install->show();
		}
	}

	reset();
#endif
}

void KGGZ_Update::slotLStart(QNetworkOperation *op)
{
#ifdef USE_FTP
	printf("FTP (L): starting operation\n");
#endif
}

void KGGZ_Update::slotLProgress(int done, int total, QNetworkOperation *op)
{
#ifdef USE_FTP
	printf("FTP (L): %i out of %i bytes done!\n", done, total);
	m_display->setText(i18n("Copying..."));
	m_progress->setTotalSteps(total);
	m_progress->setProgress(done);
#endif
}

void KGGZ_Update::slotRFinished(QNetworkOperation *op)
{
#ifdef USE_FTP
	char detail[512];

	printf("FTP (R): finished\n");
	if(op->state() == QNetworkProtocol::StFailed)
	{
		KMessageBox::information(this, i18n("The listing was not successful."), i18n("Failed! (R)"));
		int i = op->errorCode();
		printf("ErrorCode: %i\n", i);
		strcpy(detail, op->protocolDetail());
		printf("ErrorCodeDetail: %s\n", detail);
	}
	else
	{
		KMessageBox::information(this, i18n("Listing files finished."), i18n("Success! (R)"));
	}

	reset();
#endif
}

void KGGZ_Update::slotRStart(QNetworkOperation *op)
{
#ifdef USE_FTP
	printf("FTP (R): starting operation\n");
#endif
}

void KGGZ_Update::slotRProgress(int done, int total, QNetworkOperation *op)
{
#ifdef USE_FTP
	printf("FTP (R): %i out of %i bytes done!\n", done, total);
#endif
}

void KGGZ_Update::slotRConnectionStateChanged(int state, const QString &data)
{
#ifdef USE_FTP
	const char *d = data;

	printf("FTP (R): state is now %i, %s\n", state, d);
#endif
}

void KGGZ_Update::slotRChildren(const QValueList<QUrlInfo> &children)
{
#ifdef USE_FTP
	QValueList<QUrlInfo>::ConstIterator it;
	char s[512];
	QListViewItem *tmp, *tmp2;

	tmp = list->firstChild();
	if(tmp == 0) printf("TMP IS NULL!!!\n");
	else tmp = tmp->itemBelow(); // gfx effect?
	if(tmp == 0) printf("TMP(2) IS NULL!!!\n");
	printf("FTP (R): children arrived\n");
	for(it = children.begin(); it != children.end(); it++)
	{
		strcpy(s, (*it).name());
		printf("Discovered file: %s\n", s);
		//list->insertItem(new QListViewItem(tmp, s));

		if((*it).isFile())
		{
			tmp2 = new QListViewItem(tmp, s);
			tmp2->setPixmap(0, QPixmap(QString(instdir) + "images/icons/module.png"));
		}
		if((*it).isDir())
		{
			tmp2 = new QListViewItem(tmp, QString(s) + "/");
			tmp2->setPixmap(0, QPixmap(QString(instdir) + "images/icons/module_dir.png"));
		}
	}

#endif
}

int KGGZ_Update::prepare(UpdateModes mode)
{
#ifdef USE_FTP
	char *saveftp;
	char localfile[512];

	m_selected = list->currentItem();
	if(!m_selected) return 0;
	if(strlen(m_selected->text(0).latin1()) == 0) return 0;
	printf("### Current: %s\n", m_selected->text(0).latin1());
	if(m_description) free(m_description);

	if(m_selected->parent())
	{
		printf("with parent: download\n");
		/* Don't allow file listing */
		if(mode == UPDATE_SHOWFILES) return 0;
		/* Don't allow to download directories */
		if(m_selected->text(0).latin1()[strlen(m_selected->text(0).latin1()) - 1] == '/') return 0;
		m_description = (char*)malloc(strlen(m_selected->parent()->text(0).latin1()) + 1);
		strcpy(m_description, m_selected->parent()->text(0).latin1());
	}
	else
	{
		printf("without parents: showfiles\n");
		/* Don't allow download of servers */
		if(mode == UPDATE_DOWNLOAD) return 0;
		m_description = (char*)malloc(strlen(m_selected->text(0).latin1()) + 1);
		strcpy(m_description, m_selected->text(0).latin1());
	}

	ftpserver();
	printf("done ftpserver\n");

	// auto-completion :-)
	if(strncmp(m_ftpserver, "ftp://", 6))
	{
		saveftp = (char*)malloc(strlen(m_ftpserver) + 7);
		strcpy(saveftp, "ftp://");
		strcat(saveftp, m_ftpserver);
		free(m_ftpserver);
		m_ftpserver = (char*)malloc(strlen(saveftp) + 1);
		strcpy(m_ftpserver, saveftp);
		free(saveftp);
	}

	rop = m_ftpserver;
	rop.setPath(m_ftpdirectory);
	rop.setPort(21);
	printf("done setup\n");

	if(mode == UPDATE_DOWNLOAD)
	{
		strcpy(localfile, getenv("HOME"));
		strcat(localfile, "/.ggz/");
		rop.setFileName(m_selected->text(0).latin1());
		lop = "file:";
		lop.setPath(localfile);
	}

	showfiles->setEnabled(FALSE);
	download->setEnabled(FALSE);

	m_display->setText(i18n("Starting transmission"));

	return 1;
#endif
}

void KGGZ_Update::slotShowfiles()
{
#ifdef USE_FTP
	if(!prepare(UPDATE_SHOWFILES)) return;

	printf("FTP: attempt to list remote files\n");
	printf("FTP: %s/%s\n", m_ftpserver, m_ftpdirectory);

	rop.listChildren();
#endif
}

void KGGZ_Update::slotDownload()
{
#ifdef USE_FTP
	if(!prepare(UPDATE_DOWNLOAD)) return;

	printf("FTP: attempt to download\n");
	printf("FTP: %s/%s\n", m_ftpserver, m_ftpdirectory);

	lop.copy(rop, lop, FALSE); //could this also be rop.copy? *lol*
#endif
}
