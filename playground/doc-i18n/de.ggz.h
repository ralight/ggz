/**
 * @file   ggz.h
 * @author Brent M. Hendricks
 * @date   Fri Nov  2 23:32:17 2001
 * $Id: de.ggz.h 6970 2005-03-10 15:43:24Z josef $
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
 * @defgroup debug Logfunktionen für Debug- und Fehlermeldungen
 * 
 * Funktionen für Debug- und Fehlermeldungen.
 * @{
 */

/** @brief Welche Prüfungen sollen durchgeführt werden?
 *
 *  @see ggz_debug_cleanup
 */
typedef enum {
	GGZ_CHECK_NONE = 0x00, /**< Keine Prüfungen. */
	GGZ_CHECK_MEM = 0x01   /**< Speicher-(leck-)prüfungen. */
} GGZCheckType;

/** @brief Callback-Funktion zur Verarbeitung von Debugausgaben.
 *
 *  Eine Funktion diesen Typs kann als Callback-Verarbeitung registriert werden,
 *  um Debugmeldungen zu verarbeiten, anstelle der normalerweise erfolgenden
 *  Ausgabe nach stderr oder in eine Datei.
 *  Wenn sie verwendet wird, wird jede Zeile an Ausgabetext direkt an diese
 *  Funktion gesendet werden, ohne das Anfügen von Zeilenendenmarkierungen.
 *
 *  @see ggz_debug_set_func
 *  @param priority Die Priorität der Meldung, z.B. LOG_DEBUG; siehe syslog()
 *  @param msg Die Debug-Ausgabemeldung
 *  @note If your program is threaded, this function must be threadsafe.
 */
typedef void (*GGZDebugHandlerFunc)(int priority, const char *msg);

/**
 * @brief Initialisierung und Konfiguration der Debugmeldungen für das Programm.
 * 
 * Dies sollte frühzeitig im Programm aufgerufen werden, um Debugroutinen einzurichten.
 * @param types Nullterminierte Liste von beliebigen Zeichenketten mit Debugtypen
 * @param file Datei zur Ausgabe der Debugmeldungen, oder NULL für keine Datei
 * @see ggz_debug
 */
void ggz_debug_init(const char **types, const char* file);

/** @brief Setzen der Debug-Verarbeitungsfunktion.
 *
 *  Der Aufruf dieser Funktion registriert eine Verarbeitungsfunktion für
 *  Debugmeldungen. Wird NULL übergeben, wird die Verarbeitung ausgeschaltet.
 *  Wird hingegen eine Funktion übergeben, so wird diese aufgerufen,
 *  um Debug-Meldungen zu verarbeiten, und ersetzt damit eventuell vorher
 *  eingestellte Ausgabedateien.
 *
 *  @param func Die neue Funktion zur Verarbeitung von Debug-Meldungen
 *  @return Die vorherige Verarbeitungsfunktion
 *  @note Diese Funktion ist nicht thread-sicher (re-entrant).
 */
GGZDebugHandlerFunc ggz_debug_set_func(GGZDebugHandlerFunc func);

/**
 * @brief Ermöglicht Debug-Meldungen eines speziellen Typs.
 *
 * Jeder Aufruf von ggz_debug(), der diesen Typ enhält, wird dann mitprotokolliert.
 * @param type Der Debug-Typ, welcher ermöglicht werden soll
 * @see ggz_debug
 */
void ggz_debug_enable(const char *type);

/**
 * @brief Schaltet Debug-Meldungen eines speziellen Typs ab.
 *
 * Aufrufe von ggz_debug(), welche den angegebenen Debug-Typ enthalten, werden
 * anschließend nicht mehr protokolliert.
 * @param type Der abzuschaltende Debug-Typ
 * @see ggz_debug
 */
void ggz_debug_disable(const char *type);

/**
 * @brief Protokollierung einer Debug-Meldung.
 *
 * Diese Funktion nimmt als Argumente einen Debug-Typ sowie eine printf-artige
 * Liste von Argumenten. Daraus baut sie die Debug-Meldung zusammen und
 * protokolliert sie, wenn der jeweilige Debug-Typ eingeschalten ist.
 * @param type Der Debug-Typ der Meldung (ähnlich einem Logniveau)
 * @param fmt Eine printf-artige Formatzeichenkette
 * @see ggz_debug_enable, ggz_debug_disable
 */
void ggz_debug(const char *type, const char *fmt, ...)
               ggz__attribute((format(printf, 2, 3)));

/** @brief Protokollierung einer Hinweis-Meldung.
 *
 *  Diese Funktion ist nahezu identisch mit ggz_debug(), außer dass
 *  im Falle einer Übergabe der Meldung an die Debug-Verarbeitungsfunktion
 *  die Priorität auf LOG_NOTICE statt auf LOG_DEBUG gesetzt wird.
 *  Dies ist nur für wenige Anwendungsfälle notwendig.
 *  @param type Der Debug-Typ (ähnlich einem Logniveau)
 *  @param fmt Eine printf-artige Formatzeichenkette
 *  @see ggz_debug, ggz_debug_set_func
 */
void ggz_log(const char *type, const char *fmt, ...)
             ggz__attribute((format(printf, 2, 3)));

/**
 * @brief Protokollierung eines Fehlers während eines Systemaufrufes.
 *
 * Protokolliert eine Fehlermeldung ähnlich wie die Debug-Meldungen von
 * ggz_debug(). Die Protokollierung erfolgt aber unabhängig davon,
 * ob Debugmeldungen erlaubt sind oder welche Debug-Typen eingeschaltet sind.
 * Die Variablen errno und strerror werden ebenfalls verwendet, um die
 * Nützlichkeit der Meldung zu erhöhen.
 * @param fmt Eine printf-artige Formatzeichenkette
 * @see ggz_debug
 */
void ggz_error_sys(const char *fmt, ...)
                   ggz__attribute((format(printf, 1, 2)));

/**
 * @brief Protokollierung eines schwerwiegenden Fehlers eines Systemaufrufes.
 *
 * Diese Funktion protokolliert eine Meldung genauso wie ggz_error_sys(),
 * beendet aber zusätzlich anschließend das Programm.
 * @param fmt Eine printf-artige Formatzeichenkette
 */
void ggz_error_sys_exit(const char *fmt, ...)
                        ggz__attribute((format(printf, 1, 2)))
                        ggz__attribute((noreturn));

/**
 * @brief Protokollierung einer Fehlermeldung.
 *
 * Protokolliert eine Fehlermeldung ähnlich wie die Debug-Meldungen von
 * ggz_debug(). Die Protokollierung erfolgt aber unabhängig davon,
 * ob Debugmeldungen erlaubt sind oder welche Debug-Typen eingeschaltet sind.
 * @param fmt Eine printf-artige Formatzeichenkette
 * @note Dies ist äquivalent zu ggz_debug(NULL, ...) ohne eingeschaltete Debug-Typen.
 */
void ggz_error_msg(const char *fmt, ...)
                   ggz__attribute((format(printf, 1, 2)));

/**
 * @brief Protokollierung einer schwerwiegenden Fehlermeldung.
 *
 * Diese Funktion protokolliert eine Meldung genauso wie ggz_error_msg(),
 * beendet aber zusätzlich anschließend das Programm.
 * @param fmt Eine printf-artige Formatzeichenkette
 */
void ggz_error_msg_exit(const char *fmt, ...)
                        ggz__attribute((format(printf, 1, 2)))
                        ggz__attribute((noreturn));

/**
 * @brief Aufräumen der Debug-Einrichtungen und Vorbereitung auf das Programmende.
 *
 * Diese Funktion sollte direkt vor dem Beenden eines Programmes aufgerufen werden.
 * Sie säubert sämtliche Debug-Zustandsdaten, schreibt (falls konfiguriert)
 * Daten der Prüfungen auf Speicherzugriffsverletzungen, und schließt eventuell
 * offene Debug-Ausgabedateien.
 * @param check Maske von durchzuführenden Prüfungen
 */
void ggz_debug_cleanup(GGZCheckType check);

/** @} */


/**
 * @defgroup misc Verschiedene komfortable Funktionen
 *
 * @{
 */

/**
 * Maskierung von XML-Zeichen in einer Textzeichenkette.
 * @param str Zu kodierende Zeichenkette
 * @return Pointer zu einer dynamisch allokierten Zeichenkette, deren XML-Zeichen
 * durch Kaufmannsund-Zeichen ersetzt worden sind, oder NULL im Fehlerfall
 * @note Der dynamische Speicherbereich wird mit Hilfe von ggz_malloc() allokiert,
 * und der Aufrufer sollte später ggz_free() verwenden, um ihn wieder freizugeben.
 * Wenn die ursprüngliche Zeichenkette keine Zeichen enthielt, welche maskiert
 * werden müssen, wird eine Kopie mit Hilfe von ggz_strdup() zurückgeliefert.
 */
char * ggz_xml_escape(const char *str);

/**
 * Wiederherstellung von XML-Zeichen in einer Textzeichenkette.
 * @param str Zu enkodierende Zeichenkette
 * @return Pointer zu einer dynamisch allokierten Zeichenkette, der maskierte
 * XML-Zeichen durch die normalen ASCII-Zeichen ersetzt worden sind, oder NULL
 * im Fehlerfall
 * @note Der dynamische Speicherbereich wird mit Hilfe von ggz_malloc() allokiert,
 * und der Aufrufer sollte später ggz_free() verwenden, um ihn wieder freizugeben.
 * Wenn die ursprüngliche Zeichenkette keine Zeichen enthielt, welche unmaskiert
 * werden müssen, wird eine Kopie mit Hilfe von ggz_strdup() zurückgeliefert.
 */
char * ggz_xml_unescape(const char *str);

/** @brief Struktur, die intern von ggz_read_line() verwendet wird.
 */
typedef struct _GGZFile GGZFile;

/**
 * Erstellung einer Dateistruktur zur Benutzung mit ggz_read_line().
 * @param fdes Ein bereits geöffneter Dateideskriptor, von welchem gelesen werden soll
 * @return Pointer zu einer dynamisch allokierten ::GGZFile-Struktur
 * @note Der Nutzer MUSS die Datei vor der Benutzung dieser Funktion
 * für den Lesezugriff geöffnet haben. Nach der Verwendung von ggz_read_line()
 * sollte diese Struktur mit Hilfe von ggz_free_file_struct() wieder freigegeben werden.
 */
GGZFile * ggz_get_file_struct(int fdes);

/**
 * Einlesen einer Zeile beliebiger Länge von einer Datei.
 * @param file Eine ::GGZFile-Struktur, die mit ggz_get_file_struct() angelegt wurde
 * @return Nullterminierte Zeile beliebiger Länge oder NULL am Dateiende
 * @note Der dynamische Speicherbereich wird mit Hilfe von ggz_malloc() allokiert,
 * und der Aufrufer sollte später ggz_free() verwenden, um ihn wieder freizugeben.
 */
char * ggz_read_line(GGZFile *file);

/**
 * Freigabe einer mit Hilfe von ggz_get_file_struct() angelegten Dateistruktur.
 * @param file Eine ::GGZFile-Struktur, die mit ggz_get_file_struct() angelegt wurde
 * @note Der Aufrufer sollte neben der Freigabe der Struktur auch die zugehörige
 * Datei schließen.
 */
void ggz_free_file_struct(GGZFile *file);


/**
 * Zeichenkettenvergleichsfunktion, die auch mit NULL-Werten umgehen kann.
 * @param s1 Erste Zeichenkette für den Vergleich
 * @param s2 Zweite Zeichenkette für den Vergleich
 * @return Ganzzahl, welche entweder kleiner, gleich oder größer Null ist,
 * je nachdem ob s1 kleiner, gleich oder größer als s2 ist
 * @note NULL wird kleiner als alle Nicht-NULL-Zeichenketten eingestuft,
 * und gleich zu sich selbst.
 */
int ggz_strcmp(const char *s1, const char *s2);


/** Zeichenkettenvergleichsfunktion, die auch mit NULL-Werten umgehen kann,
 *  ohne Beachtung der Groß- und Kleinschreibung.
 *  Diese Funktion liefert eine Ganzzahl zurück, welche entweder kleiner,
 *  gleich oder größer Null ist, je nachdem ob s1 kleiner, gleich oder
 *  größer als s2 ist.
 *  @param s1 Erste Zeichenkette für den Vergleich
 *  @param s2 Zweite Zeichenkette für den Vergleich
 *  @return Ganzzahl als Vergleichswert
 *  @note NULL wird kleiner als alle Nicht-NULL-Zeichenketten eingestuft,
 *  und gleich zu sich selbst.
 */
int ggz_strcasecmp(const char *s1, const char *s2);
 
/** @} */


/**
 * @defgroup easysock Easysock-Netzwerkfunktionen
 * 
 * Einfache Funktionen, um binäre Daten über Dateideskriptoren zu lesen und schreiben.
 *
 * @{
 */

/** @brief Debug-Typ für Netzwerk-Debugging.
 *
 *  Um Socket-Debugmeldungen zu erlauben, muß dieser Wert zur Liste der Debug-Werte
 *  hinzugefügt werden.
 *  @see ggz_debug_enable
 */
#define GGZ_SOCKET_DEBUG "socket"

/** @brief Fehlertypen für die Socketfunktionen von GGZ.
 *
 *  Wenn es einen GGZ-Socketfehler gibt, wird die registrierte Verarbeitungsfunktion
 *  aufgerufen und bekommt einen dieser Fehlertypen mitgeliefert.
 */
typedef enum {
	GGZ_IO_CREATE,  /**< Fehler beim Erstellen eines Sockets. */
	GGZ_IO_READ,    /**< Fehler beim Lesen von einem Socket. */
	GGZ_IO_WRITE,   /**< Fehler beim Schreiben auf einen Socket. */
	GGZ_IO_ALLOCATE /**< Fehler bei der Anforderung von Speicher. */
} GGZIOType;

/** @brief Datentypen für die Verarbeitungsfunktion für GGZ-Socketfehler.
 *
 *  Wenn es einen GGZ-Socketfehler gibt, wird die registrierte Verarbeitungsfunktion
 *  aufgerufen und bekommt einen dieser Fehlerdatentypen mitgeliefert.
 */
typedef enum {
	GGZ_DATA_NONE,   /**< Keine Daten sind mit diesem Fehler verknüpft. */
	GGZ_DATA_CHAR,   /**< Der Fehler trat bei der Bearbeitung eines Zeichens auf. */
	GGZ_DATA_INT,    /**< Der Fehler trat bei der Bearbeitung einer Ganzzahl auf. */
	GGZ_DATA_STRING, /**< Der Fehler trat bei der Bearbeitung einer Zeichenkette auf. */
	GGZ_DATA_FD      /**< Fehler bei der Behandlung eines Dateideskriptors. */
} GGZDataType;

/** @brief Fehlerfunktionstyp.
 *
 *  Eine Funktion diesen Typs wird aufgerufen, wenn ein Fehler in einem GGZ-Socket
 *  auftritt.
 *
 *  @param msg Die zugehörige Fehlermeldung als Zeichenkette
 *  @param type Der Typ des aufgetretenen Fehlers
 *  @param fd Der Socket, auf welchem der Fehler passierte, oder -1 falls unbekannt
 *  @param data Zusätzliche Daten, die mit dem Fehler zusammenhängen
 */
typedef void (*ggzIOError) (const char * msg,
                            const GGZIOType type,
                            const int fd,
			    const GGZDataType data);

/** @brief Richtet die Fehlerbehandlungsfunktion für GGZ/Easysock ein.
 *
 *  Jedes Mal, wenn in einer GGZ-Socketfunktion ein Fehler auftritt,
 *  wird die registrierte Fehlerbehandlungsfunktion aufgerufen. Mit
 *  dieser Funktion kann man diese Fehlerfunktion registrieren. Dabei
 *  werden vorher registrierte Funktionen verworfen.
 *
 *  @param func Die neue Fehlerbehandlungsfunktion
 *  @return 0
 *  @todo Sollte diese Funktion nicht ein void oder ggzIOError zurückliefern?
 */
int ggz_set_io_error_func(ggzIOError func);

/** @brief Entfernt eine Fehlerbehandlungsfunktion.
 *
 *  Das Standardverhalten im Fehlerfall in einem GGZ-Socket ist es, nichts zu
 *  tun, abgesehen vom Rückgabewert der Funktionen. Durch die Einrichtung
 *  einer Fehlerbehandlungsfunktion mit ggz_set_io_error_func() kann das
 *  geändert werden, aber das alte Verhalten kann auch wieder hergestellt werden,
 *  indem diese Funktion aufgerufen wird.
 *
 *  @return Die vorherige Fehlerbehandlungsfunktion, oder NULL falls es keine gab.
 */
ggzIOError ggz_remove_io_error_func(void);


/** @brief Funktionstyp zur Beendigung des Programms.
 *
 *  Eine Funktion diesen Typs wird aufgerufen, um das Programm zu beenden.
 *  @param status Statuscode für das Programmende.
 */
typedef void (*ggzIOExit) (int status);

/** @brief Einrichtung der Programmbeendigungsfunktion für GGZ/Easysock.
 *
 *  Jegliche der *_or_die()-Funktionen ruft die eingerichtete Beendigungsfunktion
 *  auf, falls ein Fehler auftritt. Wenn keine solche Funktion gesetzt ist,
 *  wird einfach exit() aufgerufen.
 *  @param func Die neu gesetzte Beendigungsfunktion
 *  @return 0
 *  @todo Sollte hier nicht ein void zurückgegeben werden?
 */
int ggz_set_io_exit_func(ggzIOExit func);

/** @brief Entfernt eine Programmbeendigungsfunktion.
 *
 *  Dies entfernt die existierende Beendigungsfunktion, und im Fehlerfall
 *  wird von nun an wieder exit() aufgerufen, um das Programm zu beenden.
 *  @return Die vorherige Beendigungsfunktion, oder NULL falls es keine gab.
 */
ggzIOExit ggz_remove_io_exit_func(void);

/** @brief Rückgabe der maximalen Größe für Speicherallokationen.
 *
 *  @return Obergrenze für alle ggz_read_*_alloc()-Aufrufe, in Bytes
 *  @see ggz_set_io_alloc_limit
 */
unsigned int ggz_get_io_alloc_limit(void);

/** @brief Setzen der Obergrenze für Speicherallokationen.
 *
 *  In Funktionen der Form ggz_read_*_alloc() allokiert die Bibliothek
 *  selbständig Speicher für das jeweilige *-Objekt, welches eingelesen
 *  wird. Aus Sicherheitsgründen kann der Speicherbereich begrenzt
 *  werden. Der Standardwert sind 32.767 Bytes, aber er kann durch
 *  diese Funktion abgeändert werden.
 *
 *  @param limit Die neue Obergrenze für Speicherallokationen, in Bytes
 *  @return Die vorherige Obergrenze
 *  @see ggz_get_io_alloc_limit
 *  @see ggz_read_string_alloc
 */
unsigned int ggz_set_io_alloc_limit(const unsigned int limit);


/** @brief Initialisierung der Netzwerkfunktionen.
 *
 *  Diese Funktion führt alle Aufgaben durch, die in Zusammenhang mit der
 *  Initialisierung des Netzwerkes (je nach Plattform) notwendig sind.
 *  Sie sollte mindestens einmal aufgerufen werden, bevor andere Socketfunktionen
 *  verwendet werden. Mehrfache Aufrufe sind möglich.
 *  Die Funktionen, welche GGZ-Sockets erstellen, rufen diese Funktion auch
 *  automatisch auf.
 *
 *  @return 0 im Erfolgsfall, negativ im Fehlerfall
 */
int ggz_init_network(void);
			      

/****************************************************************************
 * Erstellung eines Sockets.
 * 
 * type  :  entweder GGZ_SERVER oder GGZ_CLIENT
 * port  :  TCP-Portnummer
 * server:  Hostname für die Verbindung (nur relevant für den Client)
 * 
 * Gibt den Dateideskriptor zurück, oder -1 im Fehlerfall
 ***************************************************************************/

/** @brief Sockettyp.
 *
 *  Diese Sockettypen werden von Funktionen wie ggz_make_socket() verwendet,
 *  um entscheiden zu können, welche Aktionen für eine Verbindung notwendig sind.
 */
typedef enum {
	GGZ_SOCK_SERVER, /**< Lauschen auf einem angegebenen Port. */
	GGZ_SOCK_CLIENT  /**< Verbinden zu einem Port auf einem Server. */
} GGZSockType;

/** @brief Erstellen einer Netzwerkverbindung.
 *
 *  Diese Funktion erstellt eine TCP-Socketverbindung.
 *    - Bei einem Serversocket wird auf dem angegebenen Port gelauscht,
 *      und die erste Verbindung angenommen, die dort ankommt.
 *    - Bei einem Clientsocket wird sich zu dem Server verbunden, welcher
 *      (hoffentlich) auf den angegebenen Port lauscht.
 *
 *  @param type Der Sockettyp (Server oder Client)
 *  @param port Der Port für die Verbindungs bzw. Verbindungsannahme
 *  @param server Der Hostname des Servers für Clients, ansonsten die Adresse der
 *  Schnittstelle
 *  @return Dateideskriptor im Erfolgsfall, -1 bei Fehlern der Erstellung,
 *  -2 bei Fehlern der Hostnamen-Auflösung
 */
int ggz_make_socket(const GGZSockType type, 
		    const unsigned short port, 
		    const char *server);

/** @brief Erstellung einer Netzwerkverbindung mit Abbruch im Fehlerfall.
 *
 *  Abgesehen von der Fehlerbedingung ist diese Funktion identisch zu ggz_make_socket().
 */
int ggz_make_socket_or_die(const GGZSockType type,
			   const unsigned short port, 
			   const char *server);

/** @brief Verbindung zu einem Unix-Domain-Socket.
 *
 *  Diese Funktion stellt eine Verbindung zu einem Unix-Domain-Socket her,
 *  welcher mit einer Datei auf dem Dateisystem verknüpft ist.
 *    - Bei Serversockets wird diese Datei erst gelöscht und dann neu angelegt.
 *    - Bei Clientsockets wird sich zu einer bereits existierenden Datei verbunden.
 *
 *  @param type Der Sockettyp (Server oder Client)
 *  @param name Der Name der zugehörigen Datei
 *  @return Dateideskriptor des Sockets im Erfolgsfall, -1 im Fehlerfall
 *  @note Wenn möglich, sollte diese Funktion nicht genutzt werden.
 *  Stattdessen ist socketpair() zu empfehlen.
 */
int ggz_make_unix_socket(const GGZSockType type, const char* name);

/** @brief Verbindung zu einem Unix-Domain-Socket mit Abbruch im Fehlerfall.
 *
 *  Abgesehen von der Fehlerbedingung ist diese Funktion identisch zu
 *  ggz_make_unix_socket().
 */
int ggz_make_unix_socket_or_die(const GGZSockType type, const char* name);


/** @brief Schreibt ein Zeichen auf den angegebenen Socket.
 *
 *  This function will write a single character to the socket.  The
 *  character will be readable at the other end with ggz_read_char.
 *
 *  @param sock Der Dateideskriptor, auf den geschrieben werden soll
 *  @param data Einzelnes Zeichen zum Schreiben
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_write_char(const int sock, const char data);

/** @brief Schreibt ein Zeichen auf den angegebenen Socket mit Abbruch im Fehlerfall.
 *
 *  @param sock Der Dateideskriptor, auf den geschrieben werden soll
 *  @param data Einzelnes Zeichen zum Schreiben
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_write_char().
 */
void ggz_write_char_or_die(const int sock, const char data);

/** @brief Liest ein Zeichen von einem Socket.
 *
 *  Diese Funktion liest ein einzelnes Zeichen, welches von ggz_write_char()
 *  geschrieben wurde, von einem Socket.
 *  Der Wert wird in das angegebene Zeichen übertragen.
 *
 *  @param sock Der Dateideskriptor, von dem gelesen werden soll
 *  @param data Pointer zu einem einzelnen Zeichen
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_read_char(const int sock, char *data);

/** @brief Liest ein Zeichen von einem Socket mit Abbruch im Fehlerfall.
 *
 *  @param sock Der Dateideskriptor, von dem gelesen werden soll
 *  @param data Pointer zu einem einzelnen Zeichen
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_read_char().
 */
void ggz_read_char_or_die(const int sock, char *data);

/** @brief Schreibt eine Ganzzahl auf den angegebenen Socket.
 *
 *  ggz_write_int() und ggz_read_int() können verwendet werden, um
 *  Ganzzahlen über einen Socket zu senden. Die Zahlen werden in
 *  Netzwerkanordnung (network byte order) gesendet, um die Funktionen
 *  sicher in einem Netzwerk einsetzen zu können. Es ist jedoch nicht
 *  empfohlen, mit diesen Funktionen Strukturen oder ähnliche Daten
 *  zu senden, die auf dem Zielsystem eine andere Darstellung haben könnten.
 *
 *  @param sock Der Dateideskriptor, auf den geschrieben werden soll
 *  @param data Einzelne Ganzzahl zum Schreiben
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_write_int(const int sock, const int data);

/** @brief Schreibt eine Ganzzahl auf den angegebenen Socket mit Abbruch im Fehlerfall.
 *
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_write_int().
 */
void ggz_write_int_or_die(const int sock, const int data);

/** @brief Liest eine Ganzzahl von einem Socket.
 *
 *  @see ggz_write_int
 *
 *  @param sock Der Dateideskriptor, von dem gelesen werden soll
 *  @param data Pointer zu einer einzelnen Ganzzahl
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_read_int(const int sock, int *data);

/** @brief Liest eine Ganzzahl von einem Socket mit Abbruch im Fehlerfall.
 *
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_read_int().
 */
void ggz_read_int_or_die(const int sock, int *data);

/** @brief Schreibt eine Zeichenkette auf den angegebenen Socket.
 *
 *  Diese Funktion schreibt eine komplette Zeichenkette auf den
 *  angegebenen Socket. Die Zeichenkette kann am anderen Ende
 *  durch Funktionen wie ggz_read_string() wieder gelesen werden.
 *
 *  @param sock Der Dateideskriptor, auf den geschrieben werden soll
 *  @param data Pointer zur Zeichenkette zum Schreiben
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_write_string(const int sock, const char *data);

/** @brief Schreibt eine Zeichenkette auf den Socket mit Abbruch im Fehlerfall.
 *
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_write_string().
 */
void ggz_write_string_or_die(const int sock, const char *data);

/** @brief Schreibt eine printf-artige Formatzeichenkette auf den Socket.
 *
 *  Diese Funktion nimmt eine Formatzeichenkette und eine Liste von
 *  Argumenten entgegen. Die Zeichenkette wird dann printf-artig
 *  zusammengesetzt und auf den Socket geschrieben.
 *
 *  @param sock Der Dateideskriptor, auf den geschrieben werden soll
 *  @param fmt Eine printf-artige Formatzeichenkette
 *  @param ... Eine printf-artige Liste von Argumenten
 *  @note Diese Funktion arbeitet prinzipiell wie ggz_write_string().
 */
int ggz_va_write_string(const int sock, const char *fmt, ...)
                        ggz__attribute((format(printf, 2, 3)));

/** @brief Schreibt eine Formateichenkette auf den Socket mit Abbruch im Fehlerfall.
 *
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_va_write_string().
 */
void ggz_va_write_string_or_die(const int sock, const char *fmt, ...)
                                ggz__attribute((format(printf, 2, 3)));

/** @brief Liest eine Zeichenkette von einem Socket.
 *
 *  Diese Funktion liest eine kompletten Zeichenkette von dem angegebenen Socket.
 *  Die Zeichenkette ist von Funktionen wie ggz_write_string() am anderen Ende
 *  geschrieben worden.
 *  Die Länge der Zeichenkette wird ebenfalls mitgegeben, um Pufferüberläufe
 *  zu vermeiden; Zeichen nach dieser Grenze gehen verloren.
 *
 *  @param sock Der Dateideskriptor, von dem gelesen werden soll
 *  @param data Pointer zu einer Zeichenkette; dieser wird modifiziert
 *  @param len Länge der Zeichenkette, auf die data zeigt
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_read_string(const int sock, char *data, const unsigned int len);

/** @brief Liest eine Zeichenkette von einem Socket mit Abbruch im Fehlerfall.
 *
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_read_string().
 */
void ggz_read_string_or_die(const int sock, char *data, const unsigned int len);

/** @brief Liest eine Zeichenkette von einem Socket und allokiert dafür Speicher.
 *
 *  Diese Funktion liest eine Zeichenkette ein genau so wie ggz_read_string().
 *  Allerdings wird kein vorher allokierter Puffer zur Verfügung gestellt,
 *  sondern ein Pointer zu einer Zeichenkette.
 *
 *  @code
 *    char* str;
 *    if (ggz_read_string_alloc(fd, &str) >= 0) {
 *        // ... handle the string ...
 *        ggz_free(str);
 *    }
 *  @endcode
 *
 *  @param sock Der Dateideskriptor, von dem gelesen werden soll
 *  @param data Pointer vom Typ Zeichenkette; die Zeichenkette wird allokiert
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 *  @note Die Verwendung dieser Funktion ist ein Sicherheitsproblem.
 *  @see ggz_set_io_alloc_limit
 */
int ggz_read_string_alloc(const int sock, char **data);

/** @brief Liest eine allokierte Zeichenkette von einem Socket mit Abbruch im Fehlerfall.
 *
 *  @note Abgesehen von der Fehlerbehandlung ist diese Funktion identisch zu
 *  ggz_read_string_alloc().
 */
void ggz_read_string_alloc_or_die(const int sock, char **data);

/* ggz_write_fd/ggz_read_fd werden nicht auf allen Systemen unterstützt. */

/** @brief Schreibt einen Dateideskriptor auf einen gegebenen (lokalen) Socket.
 *
 *  ggz_write_fd() und ggz_read_fd() übernehmen die ziemlich schwierige
 *  Aufgabe, einen Dateideskriptor über einen Socket zu übertragen. Der
 *  Dateideskriptor wird mit Hilfe von ggz_write_fd() geschrieben und am
 *  anderen Ende mit ggz_read_fd() gelesen. Dies funktioniert nur über
 *  lokale Sockets, also nicht über das Netz.
 *  Vielen Dank an Richard Stevens und seine wundervollen Bücher, aus
 *  denen diese Funktionen stammen.
 *
 *  @param sock Der Socket, auf den geschrieben werden soll
 *  @param sendfd Der zu sendende Dateideskriptor
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_write_fd(const int sock, int sendfd);

/** @brief Liest einen Dateideskriptor von einem gegebenen (lokalen) Socket.
 *  
 *  @see ggz_write_fd
 *
 *  @param sock Der Socket, von dem gelesen werden soll
 *  @param recvfd Der zu lesende Dateideskriptor
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 **/
int ggz_read_fd(const int sock, int *recvfd);

/** @brief Schreibt eine Bytesequenz auf einen gegebenen Socket.
 *
 *  ggz_writen() und ggz_readn() werden genutzt, um beliebige Mengen
 *  an Daten über einen Socket zu senden. Die Daten werden mit Hilfe von
 *  ggz_writen() geschrieben und können am anderen Ende mit ggz_readn()
 *  wieder gelesen werden. Vielen Dank an Richard Stevens und seine
 *  wundervollen Bücher, von denen diese Funktionen kommen.
 *
 *  @param sock Der Socket, auf den geschrieben werden soll
 *  @param vdata Pointer auf die zu schreibenden Daten
 *  @param n Die Größe der zu schreibenden Daten, in Bytes
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 */
int ggz_writen(const int sock, const void *vdata, size_t n);

/** @brief Liest eine Bytesequenz von einem gegebenen Socket.
 *
 *  @see ggz_writen
 *
 *  @param sock Der Socket, von dem gelesen werden soll
 *  @param data Pointer auf die zu lesenden Daten, mindestens n Bytes groß
 *  @param n Die Größe der zu lesenden Daten, in Bytes
 *  @return 0 im Erfolgsfall, -1 im Fehlerfall
 *  @note Es muß VOR dem Aufruf dieser Funktion bekannt sein, wieviele Daten
 *  gelesen werden sollen.
 */
int ggz_readn(const int sock, void *data, size_t n);

/** @} */

/**
 * @defgroup security Sicherheitsfunktionen
 *
 * Alle Funktionen, welche einen Bezug zur Kryptographie und Kodierung haben,
 * sind in dieser Gruppe zu finden.
 *
 * Die Verschlüsselungsfunktionen nutzen gcrypt, und schlagen immer fehl,
 * wenn keine Unterstützung für gcrypt einkompiliert worden ist. Die Funktionen
 * zur Kodierung sind hingegen immer verfügbar.
 *
 * @{
 */

/** @brief Hash-Datenstruktur.
 *
 *  Enthält eine Zeichenkette und ihre Länge, so dass die Behandlung
 *  von Zeichenketten mit NULL-Zeichen möglich ist.
 */
typedef struct
{
	char *hash;		/**< Hashwert */
	int hashlen;	/**< Länge des Hashwertes, in Bytes */
} hash_t;

/** @brief Bildung einer Hashsumme für einen Text.
 *
 *  Die Hashsumme für den angegebenen Text wird mit dem
 *  ebenfalls gegebenen Algorithmus erstellt. Dabei wird Speicher
 *  allokiert, falls notwendig.
 *
 *  @param algo Der Algorithmus, wie md5 oder sha1
 *  @param text Klartext zur Berechnung der Hashsumme
 *  @return Hashwert in einer ::hash_t-Struktur
 */
hash_t ggz_hash_create(const char *algo, const char *text);

/** @brief Bildung einer HMAC-Hashsumme für einen Text.
 *
 *  Bildet mit Hilfe eines geheimen Schlüssels eine Hashsumme.
 *  Speicher wird, falls notwendig, allokiert und muss anschließend
 *  wieder freigegeben werden.
 *
 *  @param algo Der Algorithmus, wie md5 oder sha1
 *  @param text Klartext zur Berechnung der Hashsumme
 *  @param secret Geheimer Schlüssel für die Bildung des HMAC
 *  @return Hashwert in einer ::hash_t-Struktur
 */
hash_t ggz_hmac_create(const char *algo, const char *text, const char *secret);

/** @brief Kodierung von Text nach base64
 *
 *  Klartext mit möglicherweise unsicheren Zeichen wird durch diese Funktion
 *  in das base64-Format gewandelt.
 *  Die zurückgelieferte Zeichenkette wird intern allokiert und muss deshalb
 *  anschließend freigegeben werden.
 *
 *  @param text Zu kodierender Text
 *  @param length Länge des Textes, welcher Binärzeichen enthalten könnte, in Bytes
 *  @return Base64-Darstellung des Textes
 */
char *ggz_base64_encode(const char *text, int length);

/** @brief Dekodierung von Text im base64-Format
 *
 *  Diese Funktion ist die Reverse von ggz_base64_encode().
 *  Sie allokiert ebenso den benötigten Speicher.
 *
 *  @param text Text base64-Darstellung
 *  @param length Länge des Textes, in Bytes
 *  @return Unkodierte Darstellung, möglicherweise mit Binärzeichen
 */
char *ggz_base64_decode(const char *text, int length);

/** @brief Modus der TLS-Operationen.
 *
 *  Gibt an, ob der TLS-Handshake als Client oder als Server erfolgt.
 *
 *  @see ggz_tls_enable_fd
 */
typedef enum {
	GGZ_TLS_CLIENT,		/**< Als Client verbinden */
	GGZ_TLS_SERVER		/**< Als Server verbinden */
} GGZTLSType;

/** @brief TLS-Verifikationstyp.
 *
 *  Das Modell für die Authentisierung (Verifikation),
 *  welches für den Handshake verwendet wird. None heißt,
 *  dass keine Zertifikate überprüft werden.
 *
 *  @see ggz_tls_enable_fd
 */
typedef enum {
	GGZ_TLS_VERIFY_NONE,		/**< Keine Verifikation durchführen */
	GGZ_TLS_VERIFY_PEER			/**< Überprüfung des Server-Zertifikats */
} GGZTLSVerificationType;

/** @brief Initialisierung von TLS auf der Serverseite.
 *
 *  Diese Funktion darf nur auf der Serverseite ausgeführt werden.
 *  Sie richtet die notwendigen Startwerte ein.
 *
 *  @param certfile Datei welche das Zertifikat enthält, oder NULL
 *  @param keyfile Datei welche den privaten Schlüssel enthält, oder NULL
 *  @param password Passwort zum privaten Schlüssel, oder NULL
 */
void ggz_tls_init(const char *certfile, const char *keyfile, const char *password);

/** @brief Überprüfung auf Unterstützung von TLS.
 *
 *  Da Handshakes fehlschlagen können, überprüft diese Funktion,
 *  ob die Unterstützung von TLS erfolgreich aktiviert worden ist.
 *
 *  @return 1 wenn TLS unterstützt wird, 0 wenn keine Unterstützung möglich ist
 */
int ggz_tls_support_query(void);

/** @brief TLS für einen Dateideskriptor einschalten.
 *
 *  Ein TLS-Handshake läuft für eine bereits existierenden Verbindung auf
 *  deren Dateideskriptor ab. Im Erfolgsfall werden alle nachfolgenden
 *  Daten verschlüsselt.
 *
 *  @param fdes Verwendeter Dateideskriptor
 *  @param whoami Modus der Operation (Client oder Server)
 *  @param verify Modus der Verifikation
 *  @return 1 im Erfolgsfall, 0 im Fehlerfall
 */
int ggz_tls_enable_fd(int fdes, GGZTLSType whoami, GGZTLSVerificationType verify);

/** @brief TLS für einen Dateideskriptor ausschalten.
 *
 *  Eine existierende TLS-Verbindung wird auf eine normale Verbindung zurückgesetzt,
 *  auf der sämtliche Kommunikation ohne Verschlüsselung abläuft.
 *
 *  @param fdes Verwendeter Dateideskriptor
 *  @return 1 im Erfolgsfall, 0 im Fehlerfall
 */
int ggz_tls_disable_fd(int fdes);

/** @brief Schreibt ein paar Bytes auf einen gesicherten Dateideskriptor.
 *
 *  Diese Funktion agiert als TLS-fähiger Ersatz für write(2).
 *
 *  @param fd Zu verwendender Dateideskriptor
 *  @param ptr Pointer zu den zu schreibenden Daten
 *  @param n Länge der zu schreibenden Daten, in Bytes
 *  @return Tatsächlich geschriebene Anzahl an Bytes
 */
size_t ggz_tls_write(int fd, void *ptr, size_t n);

/** @brief Liest ein paar Bytes von einem gesicherten Dateideskriptor.
 *
 *  Diese Funktion agiert als TLS-fähiger Ersatz für read(2).
 *
 *  @param fd Zu verwendender Dateideskriptor
 *  @param ptr Pointer zum Puffer, in den gelesen werden soll
 *  @param n Länge der zu lesenden Daten, in Bytes
 *  @return Tatsächlich gelesene Anzahl an Bytes
 */
size_t ggz_tls_read(int fd, void *ptr, size_t n);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __GGZ_H__ */
