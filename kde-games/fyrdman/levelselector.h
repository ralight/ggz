#ifndef FYRDMAN_LEVELSELECTOR_H
#define FYRDMAN_LEVELSELECTOR_H

#include <qdialog.h>

class QComboBox;
class QTextEdit;

class LevelSelector : public QDialog
{
	Q_OBJECT
	public:
		LevelSelector(QWidget *parent = NULL, const char *name = NULL);
		~LevelSelector();

		void addLevel(QString title);
		QString level();

	protected slots:
		void accept();
		void slotActivated(int id);

	private:
		QComboBox *combo;
		QTextEdit *desc;
};

#endif

