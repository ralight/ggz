#ifndef KGGZ_STATE_H
#define KGGZ_STATE_H

#include <qwidget.h>
#include <qlabel.h>

#include <ggzcore.h>

/* workaround for foolish g++ error message */
#define GGZStateID unsigned int

class KGGZ_State : public QWidget
{
Q_OBJECT
public:
	KGGZ_State(QWidget *parent, char *name);
	~KGGZ_State();
	void showState(const char *state);
	static void changedState(GGZStateID id, void *state_data, void *user_data);
	static char *getState(GGZStateID state);
	static void registerStates();
private:
	QLabel *label;
};

#endif