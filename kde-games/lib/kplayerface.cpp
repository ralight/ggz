#include "kplayerface.h"

#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpixmap.h>

#include <kio/job.h>
#include <kurl.h>
#include <ktempfile.h>

#include <unistd.h>

KPlayerFace::KPlayerFace(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox, *fbox;
	QFrame *f;

	f = new QFrame(this);
	f->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	m_image = new QFrame(f);
	m_image->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	m_name = new QLabel("", this);
	m_name->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	m_text = new QLabel("", this);
	m_text->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	fbox = new QVBoxLayout(f, 5);
	fbox->addStretch(1);
	fbox->add(m_image);
	fbox->addStretch(1);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(f);
	vbox->add(m_name);
	vbox->add(m_text);
}

KPlayerFace::~KPlayerFace()
{
}

void KPlayerFace::setName(QString name)
{
	m_name->setText(name);
}

void KPlayerFace::setText(QString text)
{
	m_text->setText(text);
}

void KPlayerFace::setImage(QString url)
{
	KIO::Job *job;
	KURL dest;

	KTempFile tmp;

	dest.setPath(tmp.name());
	tmp.unlink();
	job = new KIO::CopyJob(KURL::List(KURL(url)), dest, KIO::CopyJob::Copy, false, false);

	connect(job, SIGNAL(result(KIO::Job*)), SLOT(slotResult(KIO::Job*)));
	connect(job, SIGNAL(copyingDone(KIO::Job*, const KURL&, const KURL&, bool, bool)),
		SLOT(slotImage(KIO::Job*, const KURL&, const KURL&, bool, bool)));
}

void KPlayerFace::slotResult(KIO::Job *job)
{
	//delete job;
}

void KPlayerFace::slotImage(KIO::Job *job, const KURL& from, const KURL& dest, bool dir, bool ren)
{
	QPixmap pix = QPixmap(dest.path());
	m_image->setErasePixmap(pix);
	m_image->setFixedSize(pix.width(), pix.height());

	unlink(dest.path().latin1());
}

