#ifndef SHADOW_CONTAINER_H
#define SHADOW_CONTAINER_H

#include <qwidget.h>
#include <klistview.h>
#include <kcombobox.h>

class ShadowContainer : public QWidget
{
	Q_OBJECT
	public:
		ShadowContainer(QWidget *parent = NULL, const char *name = NULL);
		~ShadowContainer();
	public slots:
		void slotIncoming(const char *data);
		void slotOutgoing(const char *data);
		void slotAdmin(const char *data);
	private:
		void addEntry(const char *data, const char *pixmap);

		KListView *view;
		KComboBox *combo;
};

#endif

