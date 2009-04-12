#ifndef TABLE_CONFIGURATION_H
#define TABLE_CONFIGURATION_H

#include <qdialog.h>
#include <QPixmap>
#include <QLabel>
#include <QMenu>

#include "kggzlib_export.h"

// Forward declarations
class QPushButton;
class QLineEdit;
class QMenu;
class QLabel;
class QSlider;
class QStandardItem;
class QStandardItemModel;
class QTreeView;

// Show the launch dialog
class KGGZLIB_EXPORT TableConfiguration : public QDialog
{
	Q_OBJECT
	public:
		// Constructor
		TableConfiguration(QWidget *parent, bool readonly = false);
		// Destructor
		~TableConfiguration();

		// All possible seat types
		enum SeatTypes
		{
			seatunknown,
			seatunused,
			seatopen,
			seatbot,
			seatplayer,
			seatbuddyreserved,
			seatbotreserved,
			seatplayerreserved
		};

		// Initialize the dialog with the given parameters
		void initLauncher(QString playername, int maxplayers, int maxbots);
		// Return the customizable room description
		QString description();
		// Return the number of seats wanted
		int seats();
		// For each seat, return the set type
		SeatTypes seatType(int seat);
		// Enable/disable seat combinations
		void setSeatAssignment(int seat, int enabled);
		// Return the reservation name
		QString reservation(int seat);

		// Add a named bot
		void addBot(QString botname, QString botclass);
		// Add a buddy
		void addBuddy(QString buddyname);
		// Add a grubby
		void addGrubby(QString grubbyname);

		// The set* methods are for displaying tables only with readonly=true
		void setDescription(QString description);
		void setSeatType(int seat, SeatTypes type);
		void setReservation(int seat, QString name);

	//signals:
	//	// Emitted if table is to be launched
	//	void signalLaunch();

	private slots:
		// User clicks on a seat entry
		void slotSelected(const QPoint& pos);
		// All settings are accepted
		void slotAccepted();
		// The number of seats has changed
		void slotChanged(int value);

	private:
		// Return the i18n'd name of a seat type
		QString typeName(SeatTypes seattype);
		// Prevent a name from being used again
		void addReservation(int seat, QString name, SeatTypes seattype);
		// Liberate a name for usage again
		void freeReservation(int seat, QString name, SeatTypes seattype);

		// Widget holding all seat entries
		QTreeView *m_seats;
		// Allows editing of the number of players
		QSlider *m_slider;
		// Widget for the table name
		QLineEdit *m_edit;
		// Holds the player name
		QString m_playername;
		// Holds the current configuration
		QByteArray m_array;
		// Hold the combination matrix
		QByteArray m_assignment;
		// OK button
		QPushButton *m_ok;
		// Description of this dialog
		QLabel *m_label;
		// Holds the maximum number of bots
		int m_maxbots;
		// Holds the current number of players and bots
		int m_curplayers;
		// Current number of bots
		int m_curbots;
		// Reservation names
		QMap<int, QString> m_reservations;
		// Currently edited seat
		int m_seat;

		QStandardItemModel *m_model;

		QMap<QString, QString> m_namedbots;
		QStringList m_buddies;
		QStringList m_grubbies;

		QMap<QString, bool> m_buddies_reserved;
		QMap<QString, bool> m_namedbots_reserved;
		QMap<QString, bool> m_players_reserved;

		bool m_readonly;
};

#endif
