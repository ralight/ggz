#ifndef KGGZ_BROWSER_H
#define KGGZ_BROWSER_H

#ifdef USE_KHTML
#include <qwidget.h>
#include <khtml_part.h>
#include <kurl.h>
#include <kparts/browserextension.h>
#include <qlineedit.h>
#define BROWSER QWidget
#else
#include <qtextbrowser.h>
#define BROWSER	QTextBrowser
#endif

class KGGZ_Browser : public BROWSER // Qt moc is unable to recognize #ifdefs - har har
{
Q_OBJECT
public:
	KGGZ_Browser(QWidget *parent, char *name);
	~KGGZ_Browser();
#ifdef USE_KHTML
protected slots:
	void slotRequest(const KURL &url, const KParts::URLArgs &);
	void slotExplicitRequest();
	void slotDirectRequestGGZ();
	void slotDirectRequestMindX();
private:
	KHTMLPart *part;
	QLineEdit *ed;
#endif
};

#endif
