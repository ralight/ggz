/**
 * @file   ggz.h
 * @author Brent M. Hendricks
 * @date   Fri Nov  2 23:32:17 2001
 * $Id: de.ggz.h 6967 2005-03-10 11:13:33Z josef $
 * 
 * Header file for ggz components lib
 *
 * Copyright (C) 2001 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#ifndef __GGZ_H__
#define __GGZ_H__

#define LIBGGZ_VERSION_MAJOR 0
#define LIBGGZ_VERSION_MINOR 0
#define LIBGGZ_VERSION_MICRO 10
#define LIBGGZ_VERSION_IFACE "3:0:0"

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Verwendung von GCCs Makro "attribute".
 *
 * Unter GCC kann das Makro __attribute__ genutzt werden, um die Argumente
 * für variadische Funktionen wie etwa printf zu prüfen. Andere Compiler
 * unterstützen eventuell ähnliche Methoden, aber normalerweise ist deren
 * Verwendung nicht notwendig, da deren einziger Zweck darin besteht,
 * während des Kompilierens Warnungen auszugeben.
 */
#ifdef __GNUC__
#  define ggz__attribute(att)  __attribute__(att)
#else
#  define ggz__attribute(att)
#endif

#if defined __GNUC__ && (__GNUC__ >= 3)
#  define _GGZFUNCTION_
#else
#  ifndef __cplusplus
#    define _GGZFUNCTION_ __FUNCTION__
#  else
#    define _GGZFUNCTION_
#  endif
#endif

/**
 * @defgroup memory Speicherverwaltung
 *
 * Diese Makros bieten eine Alternative zu den normalen Funktionen
 * zur dynamischen Speicherverwaltung der Systembibliothek.
 * Sie halten im Hintergrund eine Liste der angeforderten Speicherbereiche
 * vor, mitsamt Funktionsnamen und Datei der aufrufenden Stelle.
 *
 * Anschließend kann ggz_memory_check() aufgerufen werden, um
 * sicherzustellen, dass alle Speicherbereich wieder freigegeben worden
 * sind. Hinweis: Um diese Ausgaben zu sehen, muss die Debug-Einstellung
 * MEMORY aktiviert sein.
 * 
 * @{
 */

/** @brief Debug-Typ für Speicher-Debugging.
 *  @see ggz_debug_enable
 */
#define GGZ_MEM_DEBUG "ggz_mem"

/** 
 * Makro für die Speicheranforderung
 * 
 * @param size die größe des zu allokierenden Speicherbereiches in Byte
 * 
 * @return Pointer zum allokierten und ausgenullten Speicherbereich
 */
#define ggz_malloc(size) _ggz_malloc(size, _GGZFUNCTION_ " in " __FILE__, __LINE__)
						   

/** 
 * Makro zur Größenänderung eines vorher allokierten Speicherbereiches
 * 
 * @param mem Pointer auf den zu ändernden Speicherbereich
 * @param size die neue Größe, in Bytes
 * 
 * @return Pointer zum Speicherbereich
 */
#define ggz_realloc(mem, size) _ggz_realloc(mem, size, _GGZFUNCTION_ " in " __FILE__, __LINE__)
						       

/** 
 * Makro zur Freigabe von vorher allokiertem Speicherbereich
 * 
 * @param mem Pointer zum allokierten Speicher
 * 
 * @return Fehlercode
 */
#define ggz_free(mem) _ggz_free(mem, _GGZFUNCTION_ " in " __FILE__,  __LINE__)
						 

/** 
 * Makro für die Kopie einer Zeichenkette
 * 
 * @param string die zu duplizierende Zeichenkette
 * 
 * @return Pointer zur Kopie
 *
 * @note Es ist möglich, einen NULL-Wert zu übergeben.
 */
#define ggz_strdup(string) _ggz_strdup(string, _GGZFUNCTION_ " in " __FILE__,  __LINE__)
						 


/** 
 * Diese Funktion führt die tatsächliche Speicheranforderung aus. Sie sollte nicht
 * direkt verwendet werden. Stattdessen ist ggz_malloc() zu verwenden.
 * 
 * @param size Größe des angeforderten Speicherbereiches, in Bytes
 * @param tag Zeichenkette, welche die aufrufende Funktion beschreibt
 * @param line Zeilennummer
 * 
 * @return Pointer zum allokierten, genullten Speicherbereich
*/
void * _ggz_malloc(const size_t size, const char * tag, int line);

/** 
 * Funktion zur Größenänderung eines Speicherbereiches. Sie sollte nicht direkt
 * verwendet werden, stattdessen ist ggz_realloc() zu verwenden.
 * 
 * @param ptr Pointer zum bereits existierenden Speicherbereich
 * @param size neue Größe, in Bytes
 * @param tag Zeichenkette, welche die aufrufende Funktion beschreibt
 * @param line Zeilennummer
 * 
 * @return Pointer zum allokierten Speicherbereich
*/
void * _ggz_realloc(const void * ptr, const size_t size,
                    const char * tag, int line);

/** 
 * Funktion zur Freigabe von Speicherbereichen. Sie sollte nicht direkt
 * verwendet werden, stattdessen ist ggz_free() zu verwenden.
 * 
 * @param ptr Pointer zum Speicherbereich
 * @param tag Zeichenkette, welche die aufrufende Funktion beschreibt
 * @param line Zeilennummer
 * 
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
*/
int _ggz_free(const void * ptr, const char * tag, int line);

/** 
 * Funktion zur Kopie einer Zeichenkette. Sie sollte nicht direkt verwendet
 * werden, stattdessen ist ggz_strdup() zu verwenden.
 * 
 * @param ptr die zu duplizierende Zeichenkette
 * @param tag Zeichenkette, welche die aufrufende Funktion beschreibt
 * @param line Zeilennummer
 * 
 * @return Kopie der Zeichenkette
 *
 * @note Es ist möglich, einen NULL-Wert zu übergeben.
 */
char * _ggz_strdup(const char * ptr, const char * tag, int line);


/** 
 * Prüfung sämtlicher allokierter und freigegebener Speicherblöcke, sowie
 * Anzeige eventueller Diskrepanzen.
 * 
 * 
 * @return 0 falls alle Speicherbereiche freigegeben wurden, -1 sonst
 */
int ggz_memory_check(void);

/** @} */


/**
 * @defgroup conf Nutzung von Konfigurationsdateien
 *
 * Konfigurationsdatei-Routinen zum Schreiben und Lesen von Werten.
 * Die Nutzung von Konfigurationsdateien beginnt mit einem Aufruf von
 * ggz_conf_parse(), um die Datei zu öffnen. Wenn GGZ_CONF_CREATE mit
 * angegeben wurde, wird die Datei dabei automatisch erstellt, sofern sie
 * noch nicht existiert. Es können mehrere Dateien zur selben Zeit geöffnet
 * sein, da die Funktion eine eindeutige Kennung für jede Datei zurückliefert.
 *
 * Wenn dieselbe Konfigurationsdatei mehrfach geöffnet wird, wird die
 * ursprüngliche Kennung zurückgeliefert, und es wird nur eine Kopie im
 * Speicher beibehalten. Eine Einsatzmöglichkeit dieses Merkmals ist das
 * Einlesen einer Datei und spätere Beschreiben unter demselben Namen.
 * Das funktioniert nicht, wenn die Datei unter verschiedenen Namen
 * angesprochen wird. In einem solchen Fall wäre das Verhalten undefiniert,
 * und das nicht zu planende Ergebis sollte nicht als Fehler gewertet werden.
 *
 * Die Werte werden in einem System von Sektionen und Schlüsseln gespeichert.
 * Ein Schlüssel muss innerhalb einer Sektion eindeutig sein, es können nicht
 * ein Zahlwert und eine Zeichenkette unter demselben Namen gespeichert
 * werden. Die Sektions- und Schlüsselnamen können beliebige Zeichen
 * enthalten, auch Leerzeichen, aber nicht das Gleichheitszeichen.
 * Schlüsselnamen dürfen im Gegensatz zu Sektionsnamen keine führenden
 * oder nachfolgenden Leerzeichen besitzen. Allgemein sollten Leerzeichen
 * vermieden werden. Die Groß- und Kleinschreibung wird beibehalten und
 * ist signifikant für Treffer.
 *
 * Eine wichtige Eigenschaft, die nicht vergessen werden sollte, ist,
 * dass die Benutzung der Konfigurationsfunktionen eventuelle Schreibvorgänge
 * nur im Speicher ausführt. Damit die Werte physikalisch in der Datei
 * erscheinen, muss ggz_conf_commit() aufgerufen werden.
 * Damit wird ein effizientes mehrfaches Verändern von Werten möglich,
 * da normalerweise jeder Schreibvorgang der Daten in das Format
 * der Konfigurationsdatei eine komplette Neuerstellung der Datei bedeuten
 * würde.
 *
 * Die Funktionen zum Auslesen von Zeichenketten und Listen geben dynamisch
 * allokierte Speicherbereiche an den Aufrufer zurück.
 * Der Nutzer ist dafür verantwortlich, für diese Werte ggz_free() aufzurufen,
 * wenn er sie nicht länger benötigt.
 *
 * Der von den Konfigurationsfunktionen intern benötigte Speicher wird wieder
 * freigegeben, wenn ggz_conf_cleanup() aufgerufen wird. Hinweis: Diese Funktion
 * schreibt nicht die geänderten Werte in die Datei zurück.
 * Der Nutzer sollte diese Funktion zum Programmende ausführen, aber sie kann
 * auch früher aufgerufen werden, und neue Dateien können anschließend
 * eingelesen werden.
 *
 * @bug Die maximale Länge der Zeilen in den Konfigurationsdateien ist auf
 * 1024 Zeichen beschränkt. Eine Überschreitung führt zu Lesefehlern
 * während des Einlesens der Datei. Es werden (hoffentlich) keine internen
 * Fehler auftreten, aber die abgeschnittenen Informationen gehen verloren.
 * @{
 */

/** @brief Debug-Typ für Konfigurationsdateien-Debugging.
 *  @see ggz_debug_enable
 */
#define GGZ_CONF_DEBUG "ggz_conf"

/** 
 * Gibt den Modus für das Einlesen der Konfigurationsdatei an.
 * @see ggz_conf_parse()
 */
typedef enum {
	GGZ_CONF_RDONLY = ((unsigned char) 0x01), /**< Nur lesen */
	GGZ_CONF_RDWR = ((unsigned char) 0x02),   /**< Lesen und schreiben */
	GGZ_CONF_CREATE = ((unsigned char) 0x04)  /**< Datei erzeugen */
} GGZConfType;


/**
 * Schließt alle Konfigurationsdateien.
 * @note Veränderte Werte werden hierbei nicht automatisch zurückgeschrieben.
 * Alle nicht gespeicherten Werte gehen verloren.
 */
void ggz_conf_cleanup (void);

/**
 * Schließt eine Konfigurationsdatei.
 * @note Es gelten dieselben Warnungen wie für ggz_conf_cleanup().
 */
void ggz_conf_close (int handle);

/**
 * Öffnet eine Konfigurationsdatei und liest ihre Werte ein,
 * welche anschließend mit den Zugriffsfunktionen erhalten werden können.
 * @param path Eine Zeichenkette, welche den Dateinamen angibt
 * @param options Eine logische Oder-Verknüpfung der GGZConfType-Optionen
 * @return Eine eindeutige Konfigurationsdateikennung oder -1 im Fehlerfall
 * @see GGZConfType
 */
int ggz_conf_parse		(const char *path,
				 const GGZConfType options);

/**
 * Schreibt jegliche geänderten Werte zurück in die Konfigurationsdatei.
 * Die Datei bleibt geöffnet für weitere Schreibvorgänge.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_conf_commit		(int handle);

/**
 * Schreibt einen Zeichenkettenwert mit Sektion und Schlüssel in die Konfiguration.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu verwendenden Sektion
 * @param key Name des zu verwendenden Schlüssels
 * @param value Zu schreibende Zeichenkette
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_conf_write_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*value);

/**
 * Schreibt einen Ganzzahlwert mit Sektion und Schlüssel in die Konfiguration.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu verwendenden Sektion
 * @param key Name des zu verwendenden Schlüssels
 * @param value Zu schreibende Ganzzahl
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_conf_write_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	value);

/**
 * Schreibt eine Liste von Zeichenketten mit Sektion und Schlüssel in die Konfiguration.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu verwendenden Sektion
 * @param key Name des zu verwendenden Schlüssels
 * @param argc Anzahl der Zeichenketten im Feld argv
 * @param argv Feld von Zeichenketten, aus denen die Liste konstruiert wird
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_conf_write_list	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	argc,
			 char	**argv);

/**
 * Liest einen Zeichenkettenwert aus der Konfiguration. Falls die Kombination aus
 * Sektion und Schlüssel nicht gefunden werden kann, wird der angegebene Standardwert
 * zurückgegeben.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu verwendenden Sektion
 * @param key Name des zu verwendenden Schlüssels
 * @param def Zurückzugebener Wert, falls der Eintrag nicht existiert (darf NULL sein)
 * @return Dynamisch allokierte Kopie des gefundenen Wertes oder des Standardwertes,
 * oder NULL
 * @note Die Kopie wird mit Hilfe von ggz_malloc() erzeugt und der Aufrufer sollte
 * sie demnach mit ggz_free() wieder freigeben, wenn sie nicht länger benötigt wird.
 * Falls der Standardwert mit NULL angegeben wird, wird kein Speicher allokiert.
 */
char * ggz_conf_read_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*def);

/**
 * Liest einen Ganzzahlwert aus der Konfiguration. Falls die Kombination aus
 * Sektion und Schlüssel nicht gefunden werden kann, wird der angegebene Standardwert
 * zurückgegeben.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu verwendenden Sektion
 * @param key Name des zu verwendenden Schlüssels
 * @param def Zurückzugebener Wert, falls der Eintrag nicht existiert
 * @return Ganzzahlwert aus der Konfiguration, oder der Standardwert
 */
int ggz_conf_read_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	def);

/**
 * Liest eine Liste von Zeichenketten aus der Konfiguration.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu verwendenden Sektion
 * @param key Name des zu verwendenden Schlüssels
 * @param argcp Pointer zu einer Ganzzahl, in welche die Anzahl der Listeneinträge
 * geschrieben wird
 * @param argvp Pointer zu einem Zeichenkettenfeld, in welches ein zu der dynamisch
 * allokierten Kopie der Zeichenkettenwerte zeigender Wert geschrieben wird,
 * wobei die Liste NULL-terminiert sein wird
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 * @note Das Feld wird mit Hilfe von ggz_malloc() erzeugt und der Aufrufer sollte
 * es inklusive der einzelnen Zeichenkettenwerte demnach mit ggz_free() wieder freigeben,
 * wenn es nicht länger benötigt wird.
 * Wenn die Kombination aus Sektion und Schlüssel nicht gefunden wird, wird -1
 * zurückgegeben, *argcp wird auf den Wert Null gesetzt, *argvp wird auf NULL gesetzt,
 * und es wird kein Speicher allokiert.
 */
int ggz_conf_read_list(int	handle,
			 const char	*section,
			 const char	*key,
			 int	*argcp,
			 char	***argvp);

/**
 * Entfernt eine komplette Sektion inklusive aller ihrer Schlüssel von der Konfiguration.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu entfernenden Sektion
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_conf_remove_section	(int	handle,
			 const char	*section);

/**
 * Entfernt einen einzelnen Schlüssen von der Konfiguration.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Eine Zeichenkette mit dem Namen der Sektion, in der sich der
 * Schlüssel befindet
 * @param key Name des zu entfernenden Schlüssels
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_conf_remove_key	(int	handle,
			 const char	*section,
			 const char	*key);

/**
 * Gibt die Liste aller Sektionen der Konfiguration zurück.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param argcp Pointer zu einer Ganzzahl, in welche die Anzahl der Sektionen
 * geschrieben wird
 * @param argvp Pointer zu einem Zeichenkettenfeld, in welches ein zu der dynamisch
 * allokierten Kopie der Zeichenkettenwerte zeigender Wert geschrieben wird,
 * wobei die Liste NICHT NULL-terminiert sein wird
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 * @note Das Feld wird mit Hilfe von ggz_malloc() erzeugt und der Aufrufer sollte
 * es inklusive der einzelnen Zeichenkettenwerte demnach mit ggz_free() wieder freigeben,
 * wenn es nicht länger benötigt wird.
 */
int ggz_conf_get_sections (int handle,
			 int	*argcp,
			 char	***argvp);

/**
 * Gibt die Liste aller Schlüsselnamen einer Sektion der Konfiguration zurück.
 * @param handle Gültige Kennung wie von ggz_conf_parse() zurückgegeben
 * @param section Name der zu verwendenden Sektion
 * @param argcp Pointer zu einer Ganzzahl, in welche die Anzahl der Schlüssel
 * geschrieben wird
 * @param argvp Pointer zu einem Zeichenkettenfeld, in welches ein zu der dynamisch
 * allokierten Kopie der Zeichenkettenwerte zeigender Wert geschrieben wird,
 * wobei die Liste NICHT NULL-terminiert sein wird
 * @return 0 im Erfolgsfall, -1 im Fehlerfall
 * @note Das Feld wird mit Hilfe von ggz_malloc() erzeugt und der Aufrufer sollte
 * es inklusive der einzelnen Zeichenkettenwerte demnach mit ggz_free() wieder freigeben,
 * wenn es nicht länger benötigt wird.
 */
int ggz_conf_get_keys (int handle,
			 const char *section,
			 int	*argcp,
			 char	***argvp);

/** @} */


/**
 * @defgroup list Funktionen zur Listenmanipulation
 *
 * Datenstrukturen und Funktionen zur Manipulation von verketteten Listen.
 * @{
 */

/** @brief Funktionstyp zum Datenvergleich zweier Einträge in einer ::GGZList.
 *
 *  @param a Beliebiges Element in der ::GGZList.
 *  @param b Beliebiges Element in der ::GGZList.
 *  @return Negativ falls a < b; 0 fals a == b; positiv falls a > b.  */
typedef int (*ggzEntryCompare)(const void *a, const void *b);


/** @brief Funktionstyp zur Erstellung einer Kopie eines Eintrages für das Einfügen
 *  in eine ::GGZList.
 *
 *  Eine Funktion diesen Typs kann für ein Element aufgerufen werden, wenn dieses
 *  erstmalig in eine ::GGZList eingefügt wird.
 *  @param data Pointer zum hinzuzufügenden Eintrag
 *  @return Kopie des Eintrags, welche problemlos in die Liste eingefügt werden kann
 */
typedef	void *	(*ggzEntryCreate)	(void *data);


/** @brief Funktionstyp zur Auflösung eines Eintrages in einer ::GGZList.
 *
 *  Eine Funktion diesen Typs kann für ein Element aufgerufen werden,
 *  wenn es von einer ::GGZList entfernt wird.
 *  @param data Aufzulösenden Eintrag
 */
typedef	void	(*ggzEntryDestroy)	(void *data);


/** @brief Einzelner Eintrag in einer ::GGZList.
 *
 *  Die Komponenten sollten nicht direkt angesprochen werden.
 *  Für den Zugriff stehen die Funktionen ggz_list_get_data(), ggz_list_next()
 *  und ggz_list_prev() bereit.
 */
typedef struct _GGZListEntry {
	void		     *data;     /**< Pointer zu den Daten dieses Knotens */
	struct _GGZListEntry *next, /**< Pointer zum nächsten Knoten der Liste */
		             *prev;     /**< Pointer zum verherigen Knoten der Liste */
} GGZListEntry;


/** @brief Einfache doppelt verlinkte Liste.
 *
 *  Die Komponenten sollten nicht direkt angesprochen werden, sondern nur
 *  über die entsprechenden Zugriffsfunktionen.
 */
typedef struct _GGZList {
	GGZListEntry	*head,         /**< Pointer zum ersten Knoten der Liste */
		        *tail;         /**< Pointer zum letzten Knoten der Liste */
	ggzEntryCompare	compare_func;  /**< Funktion zum Vergleich von Einträgen */
	ggzEntryCreate	create_func;   /**< Funktion zum Kopieren von Einträgen */
	ggzEntryDestroy	destroy_func;  /**< Funktion zur Auflösung von Einträgen */
	int		options;       /**< Listen-Optionen */
	int		entries;       /**< Aktuelle Anzahl von Listeneinträgen (Listenlänge) */
} GGZList;



#define GGZ_LIST_REPLACE_DUPS 0x00 /**< Überschreiben doppelter Einträge beim Einfügen */
#define GGZ_LIST_ALLOW_DUPS   0x01 /**< Erlauben doppelter Einträge in der Liste */

/** @brief Erstellung einer neuen ::GGZList.
 *
 * Bei der Erstellung einer neuen Liste gibt es einige Kontrollmöglichkeiten
 * über deren Verhalten. Der erste Parameter, compare_func, erlaubt die
 * Spezifikation einer Vergleichsfunktion zur Sortierung der Listenelemente.
 * Wenn NULL für compare_func angegeben wird, bleibt die Liste ungeordnet.
 * Der zweite Parameter, create_func, erlaubt die Spezifikation der Art
 * und Weise der Kopie neuer Einträge. ::GGZList-Objekte speichern Pointer
 * zu den Daten in ::GGZListEntry-Knoten. Wenn NULL für create_func
 * angegeben wird, wird direkt dieser Pointer zu den Daten in die
 * Liste aufgenommen. Wenn hingegen eine create_func angegeben wird,
 * wird diese aufgerufen, um eine neue Kopie des Objektes zur Aufnahme
 * in die Liste anzulegen.
 * Der dritte Parameter, destroy_func, erlaubt die Spezifikation einer
 * Auflösungsfunktion für Dateneinträge bei ihrer Herausnahme aus der Liste.
 * @note Die Funktionen ggz_list_compare_str(), ggz_list_create_str()
 * und ggz_list_destroy_str() werden für Zeichenkettendaten bereitgestellt.
 *
 * Das letzte muss entweder #GGZ_LIST_REPLACE_DUPS oder #GGZ_LIST_ALLOW_DUPS sein,
 * und gibt an, wie die Liste mit doppelten Einträgen umgeht.
 * Wenn #GGZ_LIST_REPLACE_DUPS übergeben wird, werden doppelte Einträge
 * (nach der Erkennung von compare_func) beim Aufruf von ggz_list_insert()
 * ersetzt. Wenn hingegen GGZ_LIST_ALLOW_DUPS# angegeben wird,
 * dürfen doppelte Dateneinträge in der Liste existieren.
 *
 * @note Wenn compare_func NULL ist, hat #GGZ_LIST_REPLACE_DUPS keinerlei
 * Bedeutung.
 *
 * @param compare_func Funktion zum Vergleich von Einträgen
 * @param create_func Funktion zur Erstellung und Kopie von Einträgen
 * @param destroy_func Funktion zur Auflösung von Einträgen
 * @param options Entweder #GGZ_LIST_REPLACE_DUPS oder #GGZ_LIST_ALLOW_DUPS,
 * je nachdem wie doppelte Einträge in der Liste behandelt werden sollen
 * @return Pointer zur neu allokierten ::GGZList
 */
GGZList *ggz_list_create (ggzEntryCompare compare_func,
			  ggzEntryCreate create_func,
			  ggzEntryDestroy destroy_func,
			  int options);


/** @brief Einfügen von Daten in eine Liste.
 *
 * @param list Pointer zu einer ::GGZList
 * @param data Pointer zu den einzufügenden Daten
 * @return -1 im Fehlerfall, 0 falls die Daten eingefügt wurden, und 1 falls
 * die Daten einen bereits existierenden Eintrag ersetzt haben
 * @note Das Ersetzen tritt nur dann auf, wenn #GGZ_LIST_REPLACE_DUPS
 * an ggz_list_create() übergeben wurde.
 */
int ggz_list_insert            (GGZList *list, void *data);


/** @brief Rückgabe des ersten Knotens in einer Liste.
 *
 * @param list Pointer zu einer ::GGZList
 * @return Der ::GGZListEntry, welcher an erster Stelle der Liste steht
 */
GGZListEntry *ggz_list_head	(GGZList *list);


/** @brief Rückgabe des letzten Knotens in einer Liste.
 *
 * @param list Pointer zu einer ::GGZList
 * @return Der ::GGZListEntry, welcher an letzter Stelle der Liste steht
 */
GGZListEntry *ggz_list_tail	(GGZList *list);


/** @brief Rückgabe des nächsten Knotens einer Liste.
 *
 * @param entry Pointer zu einem ::GGZListEntry in einer ::GGZList
 * @return Der nächste ::GGZListEntry in der Liste
 */
GGZListEntry *ggz_list_next  	(GGZListEntry *entry);


/** @brief Rückgabe des vorherigen Knotens einer Liste.
 *
 * @param entry Pointer zu einem ::GGZListEntry in einer ::GGZList
 * @return Der vorherige ::GGZListEntry in der Liste
 */
GGZListEntry *ggz_list_prev	(GGZListEntry *entry);


/** @brief Rückgabe der in einem Listeneintrag enthaltenen Daten.
 *
 * @param entry Pointer zu einem ::GGZListEntry
 * @return Pointer zu den im betreffenden Knoten gespeicherten Daten (::GGZListEntry)
 */
void *ggz_list_get_data		(GGZListEntry *entry);


/** @brief Suche nach einem gegebenen Eintrag in der Liste.
 *
 * ggz_list_search() sucht in einer Liste nach einem speziellen Entrag
 * mit Hilfe der Funktion ggzEntryCompare, welche als compare_func an
 * ggz_list_create() übergeben wurde. Soll eine alternative Vergleichsfunktion
 * genutzt werden, siehe ggz_list_search_alt().
 *
 * @param list Pointer zu einer ::GGZList
 * @param data Pointer zu den zu suchenden Daten
 * @return Pointer zum ::GGZListEntry, welcher den angegebenen Knoten enthält
 * (NULL wenn die Daten nicht gefunden werden konnten oder keine compare_func
 * zur Zeit der Listenerstellung angegeben worden war)
 */
GGZListEntry *ggz_list_search	(GGZList *list, void *data);


/** @brief Suche nach einem gegebenen Eintrag in der Liste mit einer Vergleichsfunktion.
 *
 * ggz_list_search_alt() durchsucht eine Liste nach einem speziellen Eintrag
 * mit Hilfe der angegebenen Funktion ggzEntryCompare.
 *
 * @param list Pointer zu einer ::GGZList
 * @param data Pointer zu den zu suchenden Daten
 * @param compare_func Vergleichsfunktion
 * @return Pointer zum ::GGZListEntry, welcher den angegebenen Knoten enthält
 * (NULL wenn die Daten nicht gefunden werden konnten oder keine compare_func
 * zur Zeit der Listenerstellung angegeben worden war)
 */
GGZListEntry *ggz_list_search_alt(GGZList *list, void *data,
				  ggzEntryCompare compare_func);

/** @brief Entfernt einen Eintrag aus einer Liste, und löst ihn auf.
 *
 * ggz_list_delete_entry() entfernt den angegebenen Eintrag von der Liste.
 * Wenn eine Funktion ggzEntryDestroy als destroy_func zu ggz_list_create()
 * übergeben worden war, wird sie für den Eintrag aufgerufen, nachdem er
 * entfernt worden ist.
 *
 * @param list Pointer zu einer ::GGZList
 * @param entry Pointer zu dem zu entfernenden ::GGZListEntry
 */
void ggz_list_delete_entry	(GGZList *list, GGZListEntry *entry);


/** @brief Freigabe aller mit einer Liste verknüpften Ressourcen.
 *
 * ggz_list_free() gibt alle mit der Liste verknüpften Ressource frei.
 * Wenn eine funktion ggzEntryDestroy als destroy_func zu ggz_list_create()
 * übergeben worden war, wird sie auf alle Listeneinträge angewendet
 * werden.
 *
 * @param list Pointer zu einer ::GGZList
 */
void ggz_list_free		(GGZList *list);


/** @brief Rückgabe der Länge einer Liste.
 *
 * @param list Pointer zu einer::GGZList
 * @return Anzahl der Einträge in der Liste
 */
int ggz_list_count		(GGZList *list);


/* Listenfunktionen für Zeichenketten */

/** @brief Vergleich zweier Zeichenketteneinträge.
 *
 *  Die Funktion ist dafür gedacht, als compare_func an ggz_list_create()
 *  übergeben zu werden, wenn eine Liste für Zeichenketten erzeugt wird.
 *
 * @param a Zeichenkette für den Vergleich
 * @param b Zeichenkette für den Vergleich
 * @return Das Ergebnis von strcmp() auf a und b, oder 1 wenn eine von beiden NULL ist
 * */
int ggz_list_compare_str	(void *a, void *b);


/** @brief Kopiert einen Zeichenketteneintrag.
 *
 *  Diese Funktion ist dafür gedacht, als create_func an ggz_list_create()
 *  übergeben zu werden, wenn eine Liste für Zeichenketten erzeugt wird.
 *
 *  @param data Zu kopierende Zeichenkette
 *  @return Allokierte Kopie der Zeichenkette
 */
void * ggz_list_create_str	(void *data);


/** @brief Freigabe einer Zeichenkette.
 *
 *  Diese Funktion ist dafür gedacht, als destroy_func an ggz_list_create()
 *  übergeben zu werden, wenn eine Liste für Zeichenketten erzeugt wird.
 *
 *  @param data Freizugebende Zeichenkette
 */
void ggz_list_destroy_str	(void *data);

/** @} */


/**
 * @defgroup stack Stacks (Stapelspeicher)
 *
 * Datenstrukturen und Funktionen zur Manipulation von Stacks.
 *
 * @{
 */

/** @brief Einfache Implementierung von Stacks mit Hilfe einer ::GGZList.
 */
typedef struct _GGZList GGZStack;


/** @brief Erzeugung eines neuen Stack.
 *
 * @return Pointer zum neu allokierten ::GGZStack-Objekt
 */
GGZStack* ggz_stack_new(void);


/** @brief Ablage eines Elementes auf den Stack.
 *
 * @param stack Pointer zu einem ::GGZStack
 * @param data Pointer zu den abzulegenden Daten
 */
void ggz_stack_push(GGZStack *stack, void *data);


/** @brief Herunternahme eines Elementes vom Stack.
 *
 * @param stack Pointer zu einem ::GGZStack
 * @return Pointer zu dem obenauf befindlichen heruntergenommenen Element
 */
void* ggz_stack_pop(GGZStack *stack);


/** @brief Rückgabe des obersten Elementes ohne Herunternahme von Stack.
 *
 * @param stack Pointer zu einer ::GGZList
 * @return Pointer zu dem obenauf befindlichen Element
 */
void* ggz_stack_top(GGZStack *stack);


/** @brief Freigabe des Stack.
 *
 * @note Diese Funktion gibt nicht die auf dem Stack abgelegten Elemente frei.
 *
 * @param stack Pointer zu einem ::GGZStack
 */
void ggz_stack_free(GGZStack *stack);

/** @} */


/**
 * @defgroup xml XML-Verarbeitung
 *
 * Hilfsfunktionen zur einfachen Verarbeitung von XML. Diese können mit einem
 * XML-Flussparser eingesetzt werden, ohne den Aufwand eines vollen DOM-Baumes
 * in Kauf nehmen zu müssen. ::GGZXMLElement steht dabei für ein einzelnes
 * Element, zusammen mit seinen Attributen und Textdaten.
 *
 * @note Die Funktionalität enthält keinen XML-Parser. Sie ist nur dafür da,
 * die bereits ausgelesenen Werte abzuspeichern.
 *
 * @{ */

/** @brief Objekt, welches ein einzelnes XML-Element darstellt.
 *
 *  Außer "process" sollte keine Komponente direkt angesprochen werden.
 *  Dafür stehen Zugriffsfunktionen bereit. "process" sollte als Methode
 *  auf Instanzen von GGZXMLElement angewendet werden.
 */
struct _GGZXMLElement {
	
	char *tag;           /**< Der Name des Elements */
	char *text;          /**< Textinhalt des Elements */
	GGZList *attributes; /**< Liste der Elementattribute */
	void *data;          /**< Weitere Daten des Elements */
	void (*free)();      /**< Funktion zur Freigabe allokierter Speicherbereiche */
	void (*process)();   /**< Funktion zur Verarbeitung dieses Objektes */
};


/** @brief Objekt, welches ein einzelnes XML-Element darstellt.
 */
typedef struct _GGZXMLElement GGZXMLElement;


/** @brief Erzeugung eines neuen ::GGZXMLElement.
 *
 * @param tag Name des XML-Elements (Tag)
 * @param attrs NULL-terminiertes Feld von Attributen und Werten, welche
 * eindimensional alternierend angeordnet sind
 * @param process Nutzerdefinierte Funktion zur Verarbeitung der XML-Elemente
 * @param free Nutzerdefinierte Funktion zur Freigabe von ::GGZXMLElement-Objekten,
 * welche von ggz_xmlelement_free() aufgerufen wird, und zusätzlich für das
 * Element ggz_free() aufrufen sollte
 * @return Pointer zum neu allokierten ::GGZXMLElement-Objekt
 */
GGZXMLElement* ggz_xmlelement_new(const char *tag, const char * const *attrs,
				  void (*process)(), void (*free)());


/** @brief Initialisierung eines ::GGZXMLElement
 *
 * @param element Pointer zum zu initialisierenden ::GGZXMLElement
 * @param tag Name des XML-Elements (Tag)
 * @param attrs NULL-terminiertes Feld von Attributen und Werten, welche
 * eindimensional alternierend angeordnet sind
 * @param process Nutzerdefinierte Funktion zur Verarbeitung der XML-Elemente
 * @param free Nutzerdefinierte Funktion zur Freigabe von ::GGZXMLElement-Objekten,
 * welche von ggz_xmlelement_free() aufgerufen wird, und zusätzlich für das
 * Element ggz_free() aufrufen sollte
 * @return Pointer zum neu allokierten ::GGZXMLElement-Objekt
 */
void ggz_xmlelement_init(GGZXMLElement *element, const char *tag,
			 const char * const *attrs,
			 void (*process)(), void (*free)());


/** @brief Ergänzt zusätzliche Daten zu einem ::GGZXMLElement-Objekt.
 *
 * Verknüpfung von zusätzlichen Daten mit einem ::GGZXMLElement-Objekt.
 * 
 * @param element Pointer zu einem XML-Element
 * @param data Pointer zu nutzerspezifischen Daten
 * @return Name des Elements
 */
void ggz_xmlelement_set_data(GGZXMLElement *element, void *data);


/** @brief Rückgabe des Namens eines XML-Elements.
 *
 * @param element Pointer zum betreffenden XML-Element
 * @return Name des Elements
 */
const char *ggz_xmlelement_get_tag(GGZXMLElement *element);


/** @brief Rückgabe eines Attributwertes von einem XML-Element.
 *
 * @param element Pointer zu einem XML-Element
 * @param attr Attributname
 * @return Wert des Attributs, oder NULL falls es nicht existiert
 */
const char *ggz_xmlelement_get_attr(GGZXMLElement *element, const char *attr);


/** @brief Rückgabe der nutzerspezifischen Daten eines XML-Elements.
 *
 * @param element Pointer zu einem XML-Element
 * @return Pointer zu den nutzerspezifischen Daten
 */
void* ggz_xmlelement_get_data(GGZXMLElement *element);


/** @brief Rückgabe der Kontext-Textdaten eines XML-Elements.
 *
 * @param element Pointer zu einem XML-Element
 * @return Textinhalt des Elements
 */
char* ggz_xmlelement_get_text(GGZXMLElement *element);


/** @brief Anfügen einer Zeichenkette zum Textinhalt eines Elements.
 *
 * @param element Pointer zu einem XML-Element
 * @param text Anzuhängende Zeichenkette
 * @param len Länge der Zeichenkette, in Bytes
 */
void ggz_xmlelement_add_text(GGZXMLElement *element, const char *text, int len);


/** @brief Freigabe der mit einem XML-Element verknüpften Speicherbereiche
 *
 * @param element Pointer zu einem XML-Element
 */
void ggz_xmlelement_free(GGZXMLElement *element);

/** @} */


/**
 * @defgroup debug Debug/error logging
 * 
 * Functions for debugging and error messages
 * @{
 */

/** @brief What memory checks should we do?
 *
 *  @see ggz_debug_cleanup
 */
typedef enum {
	GGZ_CHECK_NONE = 0x00, /**< No checks. */
	GGZ_CHECK_MEM = 0x01   /**< Memory (leak) checks. */
} GGZCheckType;

/** @brief A callback function to handle debugging output.
 *
 *  A function of this type can be registered as a callback handler to handle
 *  debugging output, rather than having the output go to stderr or to a file.
 *  If this is done, each line of output will be sent directly to this function
 *  (no trailing newline will be appended).
 *
 *  @see ggz_debug_set_func
 *  @param priority The priority of the log, i.e. LOG_DEBUG; see syslog().
 *  @param msg The debugging output message.
 *  @note If your program is threaded, this function must be threadsafe.
 */
typedef void (*GGZDebugHandlerFunc)(int priority, const char *msg);

/**
 * @brief Initialize and configure debugging for the program.
 * 
 * This should be called early in the program to set up the debugging routines.
 * @param types A null-terminated list of arbitrary string debugging "types".
 * @param file A file to write debugging output to, or NULL for none.
 * @see ggz_debug
 */
void ggz_debug_init(const char **types, const char* file);

/** @brief Set the debug handler function.
 *
 *  Call this function to register a debug handler function.  NULL can
 *  be passed to disable the debug handler.  If set, the debug handler
 *  function will be called to handle debugging output, overriding any
 *  file that had previously been specified.
 *
 *  @param func The new debug handler function.
 *  @return The previous debug handler function.
 *  @note This function is not threadsafe (re-entrant).
 */
GGZDebugHandlerFunc ggz_debug_set_func(GGZDebugHandlerFunc func);

/**
 * @brief Enable a specific type of debugging.
 *
 * Any ggz_debug calls that use that type will then be logged.
 * @param type The "type" of debugging to enable.
 * @see ggz_debug
 */
void ggz_debug_enable(const char *type);

/**
 * @brief Disable a specific type of debugging.
 *
 * Any ggz_debug calls that use the given type of debugging will then not be
 * logged.
 * @param type The "type" of debugging to disable.
 * @see ggz_debug
 */
void ggz_debug_disable(const char *type);

/**
 * @brief Log a debugging message.
 *
 * This function takes a debugging "type" as well as a printf-style list of
 * arguments.  It assembles the debugging message (printf-style) and logs it
 * if the given type of debugging is enabled.
 * @param type The "type" of debugging (similar to a loglevel).
 * @param fmt A printf-style format string
 * @see ggz_debug_enable, ggz_debug_disable
 */
void ggz_debug(const char *type, const char *fmt, ...)
               ggz__attribute((format(printf, 2, 3)));

/** @brief Log a notice message.
 *
 *  This function is nearly identical to ggz_debug, except that if the
 *  debugging output ends up passed to the debug handler function,
 *  the priority will be LOG_NOTICE instead of LOG_DEBUG.  This is only
 *  of interest to a few programs.
 *  @param type The "type" of debugging (similar to a loglevel).
 *  @param fmt A printf-style format string
 *  @see ggz_debug, ggz_debug_set_func
 */
void ggz_log(const char *type, const char *fmt, ...)
             ggz__attribute((format(printf, 2, 3)));

/**
 * @brief Log a syscall error.
 *
 * This logs an error message in a similar manner to ggz_debug's debug
 * logging.  However, the logging is done regardless of whether
 * debugging is enabled or what debugging types are set.  errno and
 * strerror are also used to create a more useful message.
 * @param fmt A printf-style format string
 * @see ggz_debug
 */
void ggz_error_sys(const char *fmt, ...)
                   ggz__attribute((format(printf, 1, 2)));

/**
 * @brief Log a fatal syscall error.
 *
 * This logs an error message just like ggz_error_sys, and also
 * exits the program.
 * @param fmt A printf-style format string
 */
void ggz_error_sys_exit(const char *fmt, ...)
                        ggz__attribute((format(printf, 1, 2)))
                        ggz__attribute((noreturn));

/**
 * @brief Log an error message.
 *
 * This logs an error message in a similar manner to ggz_debug's debug
 * logging.  However, the logging is done regardless of whether
 * debugging is enabled or what debugging types are set.
 * @param fmt A printf-style format string
 * @note This is equivalent to ggz_debug(NULL, ...) with debugging enabled.
 */
void ggz_error_msg(const char *fmt, ...)
                   ggz__attribute((format(printf, 1, 2)));

/**
 * @brief Log a fatal error message.
 *
 * This logs an error message just like ggz_error_msg, and also
 * exits the program.
 * @param fmt A printf-style format string
 */
void ggz_error_msg_exit(const char *fmt, ...)
                        ggz__attribute((format(printf, 1, 2)))
                        ggz__attribute((noreturn));

/**
 * @brief Cleans up debugging state and prepares for exit.
 *
 * This function should be called right before the program exits.  It cleans
 * up all of the debugging state data, including writing out the memory check
 * data (if enabled) and closing the debugging file (if enabled).
 * @param check A mask of things to check.
 */
void ggz_debug_cleanup(GGZCheckType check);

/** @} */


/**
 * @defgroup misc Miscellaneous convenience functions
 *
 * @{
 */

/**
 * Escape XML characters in a text string.
 * @param str The string to encode
 * @return A pointer to a dynamically allocated string with XML characters
 * replaced with ampersand tags, or NULL on error.
 * @note The dyanmic memory is allocated using ggz_malloc() and the caller is
 * expected to later free this memory using ggz_free().  If the original string
 * did not contain any characters which required escaping a ggz_strdup() copy
 * is returned.
 */
char * ggz_xml_escape(const char *str);

/**
 * Restore escaped XML characters into a text string.
 * @param str The string to decode
 * @return A pointer to a dynamically allocated string with XML ampersand tags
 * replaced with their normal ASCII characters, or NULL on error.
 * @note The dyanmic memory is allocated using ggz_malloc() and the caller is
 * expected to later free this memory using ggz_free().  If the original string
 * did not contain any characters which required decoding, a ggz_strdup() copy
 * is returned.
 */
char * ggz_xml_unescape(const char *str);

/** @brief Structure used internally by ggz_read_line()
 */
typedef struct _GGZFile GGZFile;

/**
 * Setup a file structure to use with ggz_read_line()
 * @param fdes A preopened integer file descriptor to read from
 * @return A pointer to a dynamically allocated GGZFile structure
 * @note The user MUST have  opened the requested file for reading before
 * using this function.  When finished using ggz_read_line, the user should
 * cleanup this struct using ggz_free_file_struct().
 */
GGZFile * ggz_get_file_struct(int fdes);

/**
 * Read a line of arbitrary length from a file
 * @param file A GGZFile structure allocated via ggz_get_file_struct()
 * @return A NULL terminated line from the file of arbitrary length or
 * NULL at end of file.
 * @note The dyanmic memory is allocated using ggz_malloc() and the caller is
 * expected to later free this memory using ggz_free().
 */
char * ggz_read_line(GGZFile *file);

/**
 * Deallocate a file structure allocated via ggz_get_file_struct()
 * @param file A GGZFile structure allocated via ggz_get_file_struct()
 * @note The caller is expected to close the I/O file before or after
 * freeing the file structure.
 */
void ggz_free_file_struct(GGZFile *file);


/**
 * String comparison function that is safe with NULLs
 * @param s1 First string to compare
 * @param s2 Second string to compare
 * @return An integer less than, equal to, or greater than zero if s1
 * is found, respectively, to be less than, to match, or be greater
 * than s2.  NULL in considered to be less than any non-NULL string
 * and equal to itself */
int ggz_strcmp(const char *s1, const char *s2);


/** @brief Case-insensitive string comparison function that is safe with NULLs
 *  The function returns an integer less than, equal to, or greater than
 *  zero if s1 is found, respectively, to be less than, to match, or be greater
 *  than s2.  NULL in considered to be less than any non-NULL string
 *  and equal to itself
 *  @param s1 First string to compare
 *  @param s2 Second string to compare
 *  @return The comparison value.
 */
int ggz_strcasecmp(const char *s1, const char *s2);
 
/** @} */


/**
 * @defgroup easysock Easysock IO
 * 
 * Simple functions for reading/writing binary data across file descriptors.
 *
 * @{
 */

/** @brief ggz_debug debugging type for Easysock debugging.
 *
 *  To enable socket debugging, add this to the list of debugging types.
 *  @see ggz_debug_enable
 */
#define GGZ_SOCKET_DEBUG "socket"

/** @brief An error type for the GGZ socket functions.
 *
 *  If there is a GGZ socket error, the registered error handler will be
 *  called and will be given one of these error types.
 */
typedef enum {
	GGZ_IO_CREATE,  /**< Error creating a socket. */
	GGZ_IO_READ,    /**< Error reading from a socket. */
	GGZ_IO_WRITE,   /**< Error writing to a socket. */
	GGZ_IO_ALLOCATE /**< Error when the allocation limit is exceeded. */
} GGZIOType;

/** @brief A data type for the GGZ socket function error handler.
 *
 *  If there is a GGZ socket error, the registered error handler will be
 *  called and will be given one of these error data types.
 */
typedef enum {
	GGZ_DATA_NONE,   /**< No data is associated with the error. */
	GGZ_DATA_CHAR,   /**< The error occurred while dealing with a char. */
	GGZ_DATA_INT,    /**< The error occurred in dealing with an integer. */
	GGZ_DATA_STRING, /**< The error occurred in dealing with a string. */
	GGZ_DATA_FD      /**< Error while dealing with a file descriptor. */
} GGZDataType;

/** @brief An error function type.
 *
 *  This function type will be called when there is an error in a GGZ
 *  socket function.
 *
 *  @param msg The strerror message associated with the error.
 *  @param type The type of error that occurred.
 *  @param fd The socket on which the error occurred, or -1 if not applicable.
 *  @param data Extra data associated with the error.
 */
typedef void (*ggzIOError) (const char * msg,
                            const GGZIOType type,
                            const int fd,
			    const GGZDataType data);

/** @brief Set the ggz/easysock error handling function.
 *
 *  Any time an error occurs in a GGZ socket function, the registered error
 *  handling function will be called.  Use this function to register a new
 *  error function.  Any previous error function will be discarded.
 *
 *  @param func The new error-handling function.
 *  @return 0
 *  @todo Shouldn't this function return a void or ggzIOError?
 */
int ggz_set_io_error_func(ggzIOError func);

/** @brief Remove the ggz/easysock error handling function.
 *
 *  The default behavior when a socket failure occurs in one of the GGZ socket
 *  functions is to do nothing (outside of the function's return value).  This
 *  may be overridden by registering an error handler with
 *  ggz_set_io_error_func, but the behavior may be returned by calling this
 *  function to remove the error handler.
 *
 *  @return The previous error-handling function, or NULL if none.
 */
ggzIOError ggz_remove_io_error_func(void);


/** @brief An exit function type.
 *
 *  This function type will be called to exit the program.
 *  @param status The exit value.
 */
typedef void (*ggzIOExit) (int status);

/** @brief Set the ggz/easysock exit function.
 *
 *  Any of the *_or_die() functions will call the set exit function if there
 *  is an error.  If there is no set function, exit() will be called.
 *  @param func The newly set exit function.
 *  @return 0
 *  @todo Shouldn't this return a void?
 */
int ggz_set_io_exit_func(ggzIOExit func);

/** @brief Remove the ggz/easysock exit function.
 *
 *  This removes the existing exit function, if one is set.  exit() will
 *  then be called to exit the program.
 *  @return The old exit function (or NULL if none).
 */
ggzIOExit ggz_remove_io_exit_func(void);

/** @brief Get libggz's limit on memory allocation.
 *
 *  @return The limit (in bytes) to allow on ggz_read_XXX_alloc() calls.
 *  @see ggz_set_io_alloc_limit
 */
unsigned int ggz_get_io_alloc_limit(void);

/** @brief Set libggz's limit on memory allocation.
 *
 *  In functions of the form ggz_read_XXX_alloc(), libggz will itself
 *  allocate memory for the XXX object that is being read in.  This
 *  presents an obvious security concern, so we limit the amount of
 *  memory that can be allocated.  The default value is 32,767 bytes,
 *  but it can be changed by calling this function.
 *
 *  @param limit The new limit (in bytes) to allow on alloc-style calls.
 *  @return The previous limit.
 *  @see ggz_get_io_alloc_limit
 *  @see ggz_read_string_alloc
 */
unsigned int ggz_set_io_alloc_limit(const unsigned int limit);


/** @brief Initialize the network.
 *
 *  This function will do anything necessary to initialize the network to
 *  use sockets (this is needed on some platforms).  It should be called at
 *  least once before any sockets are put to use.  Calling it more than once
 *  is harmless.  It will be called automatically at the start of all GGZ
 *  socket creation functions.
 *
 *  @return 0 on success; negative on failure */
int ggz_init_network(void);
			      

/****************************************************************************
 * Creating a socket.
 * 
 * type  :  one of GGZ_SERVER or GGZ_CLIENT
 * port  :  tcp port number 
 * server:  hostname to connect to (only relevant for client)
 * 
 * Returns socket fd or -1 on error
 ***************************************************************************/

/** @brief A socket type.
 *
 *  These socket types are used by ggz_make_socket and friends to decide
 *  what actions are necessary in making a connection.
 */
typedef enum {
	GGZ_SOCK_SERVER, /**< Just listen on a particular port. */
	GGZ_SOCK_CLIENT  /**< Connect to a particular port of a server. */
} GGZSockType;

/** @brief Make a socket connection.
 *
 *  This function makes a TCP socket connection.
 *    - For a server socket, we'll just listen on the given port an accept
 *      the first connection that is made there.
 *    - For a client socket, we'll connect to a server that is (hopefully)
 *      listening at the given port and hostname.
 *
 *  @param type The type of socket (server or client).
 *  @param port The port to listen/connect to.
 *  @param server The server hostname for clients, the interface address else
 *  @return File descriptor on success, -1 on creation error, -2 on lookup
 *  error.
 */
int ggz_make_socket(const GGZSockType type, 
		    const unsigned short port, 
		    const char *server);

/** @brief Make a socket connection, exiting on error.
 *
 *  Aside from the error condition, this is identical to ggz_make_socket.
 */
int ggz_make_socket_or_die(const GGZSockType type,
			   const unsigned short port, 
			   const char *server);

/** @brief Connect to a unix domain socket.
 *
 *  This function connects to a unix domain socket, a socket associated with
 *  a file on the VFS.
 *    - For a server socket, we unlink the socket file first and then
 *      create it.
 *    - For a client socket, we connect to a pre-existing socket file.
 *
 *  @param type The type of socket (server or client).
 *  @param name The name of the socket file.
 *  @return The socket FD on success, -1 on error.
 *  @note When possible, this should not be used.  Use socketpair() instead.
 */
int ggz_make_unix_socket(const GGZSockType type, const char* name);

/** @brief Connect to a unix domain socket, exiting on error.
 *
 *  Aside from the error condition, this is identical to ggz_make_unix_socket.
 */
int ggz_make_unix_socket_or_die(const GGZSockType type, const char* name);


/** @brief Write a character value to the given socket.
 *
 *  This function will write a single character to the socket.  The
 *  character will be readable at the other end with ggz_read_char.
 *
 *  @param sock The socket file descriptor to write to.
 *  @param data A single character to write.
 *  @return 0 on success, -1 on error.
 */
int ggz_write_char(const int sock, const char data);

/** @brief Write a character value to the given socket, exiting on error.
 *
 *  @param sock The socket file descriptor to write to.
 *  @param data A single character to write.
 *  @note Aside from error handling, this is identical to ggz_write_char.
 */
void ggz_write_char_or_die(const int sock, const char data);

/** @brief Read a character value from the given socket.
 *
 *  This function will read a single character (as written by
 *  ggz_write_char) from a socket.  It places the value into the
 *  character pointed to.
 *
 *  @param sock The socket file descriptor to read from.
 *  @param data A pointer to a single character.
 *  @return 0 on success, -1 on error
 */
int ggz_read_char(const int sock, char *data);

/** @brief Read a character value from the given socket, exiting on error.
 *
 *  @param sock The socket file descriptor to read from.
 *  @param data A pointer to a single character.
 *  @note Aside from error handling, this is identical to ggz_read_char.
 */
void ggz_read_char_or_die(const int sock, char *data);

/** @brief Write an integer to the socket in network byte order.
 *
 *  ggz_write_int and ggz_read_int can be used to send an integer across a
 *  socket.  The integer will be sent in network byte order, so the
 *  functions may safely be used across a network.  Note, though, that it
 *  is probably not safe to use this function to send structs or other
 *  data that may use a different representation than a simple integer.
 *
 *  @param sock The socket to write to.
 *  @param data The integer to write.
 *  @return 0 on success, -1 on error.
 */
int ggz_write_int(const int sock, const int data);

/** @brief Write an integer to the socket, exiting on error.
 *
 *  Aside from the error condition, this function is identical to
 *  ggz_write_int.
 */
void ggz_write_int_or_die(const int sock, const int data);

/** @brief Read an integer from the socket in network byte order.
 *  
 *  @see ggz_write_int
 *
 *  @param sock The socket to write to.
 *  @param data A pointer to an integer in which to place the data.
 *  @return 0 on success, -1 on error.
 */
int ggz_read_int(const int sock, int *data);

/** @brief Read an integer from the socket, exiting on error.
 *
 *  Aside from the error condition, this function is identical to
 *  ggz_read_int.
 */
void ggz_read_int_or_die(const int sock, int *data);

/** @brief Write a string to the given socket.
 *
 *  This function will write a full string to the given socket.  The string
 *  may be read at the other end by ggz_read_string and friends.
 *
 *  @param sock The socket file descriptor to write to.
 *  @param data A pointer to the string to write.
 *  @return 0 on success, -1 on error.
 */
int ggz_write_string(const int sock, const char *data);

/** @brief Write a string to the given socket, exiting on error.
 *
 *  Aside from the error condition, this function is identical to
 *  ggz_write_string.
 */
void ggz_write_string_or_die(const int sock, const char *data);

/** @brief Write a printf-style formatted string to the given socket.
 *
 *  This function allows a format string and a list of arguments
 *  to be passed in.  The function will assemble the string (printf-style)
 *  and write it to the socket.
 *
 *  @param sock The socket file descriptor to write to.
 *  @param fmt A printf-style formatting string.
 *  @param ... A printf-style list of arguments.
 *  @note This function will write identically to ggz_write_string.
 */
int ggz_va_write_string(const int sock, const char *fmt, ...)
                        ggz__attribute((format(printf, 2, 3)));
			
/** @brief Write a formatted string to the socket, exiting on error.
 *
 *  Aside from the error condition, this function is identical to
 *  ggz_va_write_string.
 */
void ggz_va_write_string_or_die(const int sock, const char *fmt, ...)
                                ggz__attribute((format(printf, 2, 3)));
			
/** @brief Read a string from the given socket.
 *
 *  This function will read a full string from the given socket.  The string
 *  may be written at the other end by ggz_write_string and friends.  The
 *  length of the string is given as well to avoid buffer overruns; any
 *  characters beyond this will be lost.
 *
 *  @param sock The socket file descriptor to write to.
 *  @param data A pointer to the string to read; it will be changed.
 *  @param len The length of the string pointed to by len
 *  @return 0 on success, -1 on error.
 */
int ggz_read_string(const int sock, char *data, const unsigned int len);

/** @brief Read a string from the given socket, exiting on error.
 *
 *  Aside from the error condition, this function is identical to
 *  ggz_read_string.
 */
void ggz_read_string_or_die(const int sock, char *data, const unsigned int len);

/** @brief Read and allocate a string from the given socket.
 *
 *  This function reads a string from the socket, just like ggz_read_string.
 *  But instead of passing in a pre-allocated buffer to write in, here
 *  we pass a pointer to a string pointer:
 *
 *  @code
 *    char* str;
 *    if (ggz_read_string_alloc(fd, &str) >= 0) {
 *        // ... handle the string ...
 *        ggz_free(str);
 *    }
 *  @endcode
 *
 *  @param sock The socket file descriptor to write to.
 *  @param data A pointer to an empty string pointer.
 *  @return 0 on success, -1 on error.
 *  @note The use of this function is a security risk.
 *  @see ggz_set_io_alloc_limit
 */
int ggz_read_string_alloc(const int sock, char **data);

/** @brief Read and allocate string from the given socket, exiting on error.
 *
 *  Aside from the error condition, this function is identical to
 *  ggz_read_string_alloc.
 */
void ggz_read_string_alloc_or_die(const int sock, char **data);

/* ggz_write_fd/ggz_read_fd are not supported on all systems. */

/** @brief Write a file descriptor to the given (local) socket.
 *
 *  ggz_write_fd and ggz_read_fd handle the rather tricky task of sending
 *  a file descriptor across a socket.  The FD is written with ggz_write_fd
 *  and can be read at the other end by ggz_read_fd.  Note that this will
 *  only work for local sockets (i.e. not over the network).  Many thanks to
 *  Richard Stevens and his wonderful books, from which these functions come.
 *
 *  @param sock The socket to write to.
 *  @param sendfd The FD to send across the socket.
 *  @return 0 on success, -1 on error.
 */
int ggz_write_fd(const int sock, int sendfd);

/** @brief Read a file descriptor from the given (local) socket.
 *  
 *  @see ggz_write_fd
 *
 *  @param sock The socket to read from.
 *  @param recvfd The FD that is read.
 *  @return 0 on success, -1 on error.
 **/
int ggz_read_fd(const int sock, int *recvfd);

/** @brief Write a sequence of bytes to the socket.
 *
 *  ggz_writen and ggz_readn are used to send an arbitrary quantity of raw
 *  data across the a socket.  The data is written with ggz_writen and can
 *  be read at the other end with ggz_readn.  Many thanks to Richard Stevens
 *  and his wonderful books, from which these functions come.
 *
 *  @param sock The socket to write to.
 *  @param vdata A pointer to the data to write.
 *  @param n The number of bytes of data to write from vdata.
 *  @return 0 on success, -1 on error.
 */
int ggz_writen(const int sock, const void *vdata, size_t n);

/** @brief Read a sequence of bytes from the socket.
 *
 *  @see ggz_writen
 *
 *  @param sock The socket to read from.
 *  @param data A pointer a buffer of size >= n in which to place the data.
 *  @param n The number of bytes to read.
 *  @return 0 on success, -1 on error.
 *  @note You must know how much data you want _before_ calling this function.
 */
int ggz_readn(const int sock, void *data, size_t n);

/** @} */

/**
 * @defgroup security Security functions
 *
 * All functions related to encryption and encoding go here.
 *
 * Encryption functions use gcrypt, and will always fail if support for gcrypt
 * has not been compiled in. Encoding functions will always be available.
 *
 * @{
 */

/** @brief Hash data structure
 *
 *  Contains a string and its length, so that NULL-safe
 *  functions are possible.
 */
typedef struct
{
	char *hash;		/**< Hash value */
	int hashlen;	/**< Length of the hash value in bytes */
} hash_t;

/** @brief Create a hash over a text
 *
 *  A hash sum over a given text is created, using the given
 *  algorithm. Space is allocated as needed.
 *
 *  @param algo The algorithm, like md5 or sha1
 *  @param text Plain text used to calculate the hash sum
 *  @return Hash value in a structure
 */
hash_t ggz_hash_create(const char *algo, const char *text);

/** @brief Create a HMAC hash over a text
 *
 *  Creates a hash sum using a secret key.
 *  Space is allocated as needed and must be freed afterwards.
 *
 *  @param algo The algorithm to use, like md5 or sha1
 *  @param text Plain text used to calculate the hash sum
 *  @param secret Secret key to be used for the HMAC creation
 *  @return Hash value in a structure
 */
hash_t ggz_hmac_create(const char *algo, const char *text, const char *secret);

/** @brief Encodes text to base64
 *
 *  Plain text with possibly unsafe characters is converted
 *  to the base64 format through this function.
 *  The returned string is allocated internally and must be freed.
 *
 *  @param text Plain text to encode
 *  @param length Length of the text (which may contain binary characters)
 *  @return Base64 representation of the text
 */
char *ggz_base64_encode(const char *text, int length);

/** @brief Decodes text from base64
 *
 *  This is the reverse function to ggz_base64_encode.
 *  It will also allocate space as needed.
 *
 *  @param text Text in base64 format
 *  @param length Length of the text
 *  @return Native representation, may contain binary characters
 */
char *ggz_base64_decode(const char *text, int length);

/** @brief TLS operation mode
 *
 *  Hints whether the TLS handshake will happen in either
 *  client or server mode.
 *
 *  @see ggz_tls_enable_fd
 */
typedef enum {
	GGZ_TLS_CLIENT,		/**< Operate as client */
	GGZ_TLS_SERVER		/**< Operate as server */
} GGZTLSType;

/** @brief TLS verification type
 *
 *  The authentication (verification) model to be used
 *  for the handshake. None means that no certificate
 *  is validated.
 *
 *  @see ggz_tls_enable_fd
 */
typedef enum {
	GGZ_TLS_VERIFY_NONE,		/**< Don't perform verification */
	GGZ_TLS_VERIFY_PEER			/**< Perform validation of the server's cert */
} GGZTLSVerificationType;

/** @brief Initialize TLS support on the server side
 *
 *  This function ought only be used on the server side.
 *  It sets up the necessary initialization values.
 *
 *  @param certfile File containing the certificate, or NULL
 *  @param keyfile File containing the private key, or NULL
 *  @param password Password to the private key, or NULL
 */
void ggz_tls_init(const char *certfile, const char *keyfile, const char *password);

/** @brief Check TLS support
 *
 *  Since handshakes can fail, this function checks whether TLS
 *  was actually successfully activated.
 *
 *  @return 1 if TLS is supported, 0 if no support is present
 */
int ggz_tls_support_query(void);

/** @brief Enable TLS for a file descriptor
 *
 *  A TLS handshake is performed for an existing connection on the given
 *  file descriptor. On success, all consecutive data will be encrypted.
 *
 *  @param fdes File descriptor in question
 *  @param whoami Operation mode (client or server)
 *  @param verify Verification mode
 *  @return 1 on success, 0 on failure
 */
int ggz_tls_enable_fd(int fdes, GGZTLSType whoami, GGZTLSVerificationType verify);

/** @brief Disable TLS for a file descriptor
 *
 *  An existing TLS connection is reset to a normal connection on which
 *  all communication happens without encryption.
 *
 *  @param fdes File descriptor in question
 *  @return 1 on success, 0 on failure
 */
int ggz_tls_disable_fd(int fdes);

/** @brief Write some bytes to a secured file descriptor
 *
 *  This function acts as a TLS-aware wrapper for write(2).
 *
 *  @param fd File descriptor to use
 *  @param ptr Pointer to the data to write
 *  @param n Length of the data to write
 *  @return Actual number of bytes written
 */
size_t ggz_tls_write(int fd, void *ptr, size_t n);

/** @brief Read from a secured file descriptor
 *
 *  This function acts as a TLS-aware wrapper for read(2).
 *
 *  @param fd File descriptor to use
 *  @param ptr Pointer to a buffer to store the data into
 *  @param n Number of bytes to read, and minimum size of the buffer
 *  @return Actually read number of bytes
 */
size_t ggz_tls_read(int fd, void *ptr, size_t n);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __GGZ_H__ */
