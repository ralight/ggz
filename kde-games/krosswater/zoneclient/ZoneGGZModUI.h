#ifndef ZONE_GGZ_MOD_UI_H
#define ZONE_GGZ_MOD_UI_H

#include <qwidget.h>
#include <qsocketnotifier.h>
#include <stdio.h>

#define TMPDIR "/tmp"

#include "ZoneGGZ.h"

class ZoneGGZModUI : public QWidget
{
Q_OBJECT
public:
	ZoneGGZModUI(QWidget *parent, char *name = NULL);
	~ZoneGGZModUI();
	void ZoneRegister(char *modulename);
	int fd();
	int zoneTurn();
	int zoneMe();
	int zonePlayers();

	int ZoneSeat;
	int ZoneSeats[16];
	int ZonePlayernum;
	char ZonePlayers[16][17];
	int ZoneRules, ZoneMaxplayers;
signals:
	void signalZoneReady();
	void signalZoneInput(int op);
	void signalZoneTurn();
	void signalZoneTurnOver();
	void signalZoneOver();
	void signalZoneInvalid();
private slots:
	void slotZoneInput();
private:
	int zoneCreateFd(char *modulename);
	void zoneGetSeats();
	void zoneGetPlayers();
	void zoneGetRules();
	void zoneCallTurn();
	void zoneCallTurnOver();
	void zoneCallOver();
	void zoneCallInvalid();

	QSocketNotifier *zone_sn;
	int zone_fd;
	int m_ready;
	int m_turn;
	int m_players;
};

#endif
