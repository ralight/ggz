#ifndef KGGZ_BROWSER_H
#define KGGZ_BROWSER_H

#include <qwidget.h>
#include <khtml_part.h>
#include <kurl.h>
#include <kparts/browserextension.h>
#include <qlineedit.h>

class KGGZBrowser : public QWidget
{
	Q_OBJECT
	public:
		KGGZBrowser(QWidget *parent = NULL, char *name = NULL);
		~KGGZBrowser();
	public slots:
		void slotRequest(const KURL& url, const KParts::URLArgs& args);
	private:
		KHTMLPart *m_part;
		QLineEdit *m_edit;
};

#endif

