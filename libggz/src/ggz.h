/**
 * @file   ggz.h
 * @author Brent M. Hendricks
 * @date   Fri Nov  2 23:32:17 2001
 * 
 * Header file for ggz componenets lib
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


/**
 * @defgroup memory Memory Handling
 *
 * These macros proivide an alternative to the normal C library
 * functions for dynamically allocating memory.  They keep track of
 * memory allocated by storing the name of the function and file in
 * which they were called 
 *
 * You can then call ggz_memory_check() to make sure all allocated
 * memory has been freed  Note that you will need to enable MEMORY
 * debugging to see this
 * 
 * @{
 */

/** 
 * Macro for memory allocation
 * 
 * @param size the size of memory to allocate
 * 
 * @return a pointer to the newly allocated memory
 */
#define ggz_malloc(size) _ggz_malloc(size, __FUNCTION__ " in " __FILE__, __LINE__)
						   

/** 
 * Macro for resizing previously allocated memory
 * 
 * @param mem pointer to memory to reallocate
 * @param size new size requested
 * 
 * @return pointer to allocated memory
 */
#define ggz_realloc(mem, size) _ggz_realloc(mem, size, __FUNCTION__ " in " __FILE__, __LINE__)
						       

/** 
 * Macro for freeing memory previously allocated 
 * 
 * @param mem pointer to allocated memory
 * 
 * @return failure code
 */
#define ggz_free(mem) _ggz_free(mem, __FUNCTION__ " in " __FILE__,  __LINE__)
						 

/** 
 * Macro for duplicating string
 * 
 * @param string string to duplicate
 * 
 * @return pointer to new string
 */
#define ggz_strdup(string) _ggz_strdup(string, __FUNCTION__ " in " __FILE__,  __LINE__)
						 


/** 
 * Function to actually perform memory allocation.  Don't call this
 * directly.  Instead, call ggz_malloc()
 * 
 * @param int size of memory to allocate
 * @param  char * string describing the calling function 
 * @param int linenumber 
 * 
 * @return pointer to newly allocated memory
*/
void * _ggz_malloc(const unsigned int, char *, int);

/** 
 * Function to perform memory reallocation.  Don't call this
 * directly.  Instead, call ggz_realloc()
 * 
 * @param void * pointer to memory to reallocate
 * @param const unsigned int new size 
 * @param  char * string describing the calling function 
 * @param int linenumber 
 * 
 * @return pointer to allocated memory
*/
void * _ggz_realloc(const void *, const unsigned int, char *, int);

/** 
 * Function to free allocated memory.  Don't call this
 * directly.  Instead, call ggz_free()
 * 
 * @param const void * pointer to memory
 * @param  char * string describing the calling function 
 * @param int linenumber 
 * 
 * @return 
*/
int _ggz_free(const void *, char *, int);

/** 
 * Function to copy a string.  Don't call this
 * directly.  Instead, call ggz_strdup()
 * 
 * @param const char* string to duplicate
 * @param  char * string describing the calling function 
 * @param int linenumber 
 * 
 * @return newly allocated string
*/
char * _ggz_strdup(const char *, char *, int);


/** 
 * Check memory allocated against memory freed and display any discrepencies
 * 
 * 
 * @return 
 */int ggz_memory_check(void);

/** @} */



/**
 * @defgroup conf Configuration file parsing
 * Configuration file routines to store and retrieve values.
 * Configuration file parsing begins by calling ggz_conf_parse() to open
 * a config file.  The file can be created automatically if CONF_CREATE
 * is specified.  The returned handle uniquely identifies the configuration
 * file, so multiple files can be open at one time.
 *
 * If the same configuration file is opened multiple times, the original
 * handle will be returned and only one copy will be retained within memory.
 * One use of this feature is that a file may be opened read only initially and
 * later have writing enabled by merely reparsing the file using the same
 * pathname.  Note that this feature can be fooled if the same file is
 * opened using different pathnames.  Chaos may or may not result in this
 * case, and it is considered a feature not a bug.
 *
 * Values are stored using a system of section and keys.  A key must be
 * unique within a section (you cannot store both an integer and a string
 * under the same key.  Section and key names may contain any characters
 * (including whitespace) other than an equals sign.  Although keys may not
 * have leading or trailing whitespace, section names may.  It is suggested
 * that any whitespace (other than possibly internal spaces) be avoided when
 * specifying section and key names.  Alphabetic case is preserved, and must
 * be matched.
 *
 * An important caveat to remember when using the configuration functions
 * is that writing a value only caches the value in memory.  In order to
 * write the values to the physical file, ggz_conf_commit() must be called
 * at some point.  This makes writing multiple values in rapid succession
 * more efficient, as the entire file must be regenerated in order to be
 * written to the flat-file format of the configuration file.
 *
 * The string and list reading functions return dynamically allocated
 * memory to the caller.  The user is responsible for calling free() on
 * this memory when they no longer need the returned values.
 *
 * All memory used internally by the configuration functions will be
 * released when ggz_conf_cleanup() is called.  Note that this does NOT
 * commit any changes made to the configuration files.
 * The user is expected to call this at program termination, but it may
 * be called at any time earlier than termination and new files may
 * be subsequently opened.
 *
 * @bug The maximum length of a configuration file line is fixed at 1024
 * characters.  Exceeding this length will result in parsing errors when
 * the file is parsed.  No internal problems should (hopefully) result,
 * but values will be lost.
 * @{
 */

/** 
 * Specifies opening a configuration file in read-only mode
 * @see ggz_conf_parse()
 */
#define CONF_RDONLY	((unsigned char) 0x01)
/** 
 * Specifies opening a configuration file for both reading and writing
 * @see ggz_conf_parse()
 */
#define CONF_RDWR	((unsigned char) 0x02)
/** 
 * Specifies creating the configuration file if it does not already exist
 * @see ggz_conf_parse()
 */
#define CONF_CREATE	((unsigned char) 0x04)

/**
 * Closes all open configuration files.
 * @note This does not automatically commit changed values.  Any non-committed
 * values written will be lost.
 */
void ggz_conf_cleanup (void);

/**
 * Opens a configuration file and parses the variables so they can
 * be retrieved with the access functions.
 * @param path A string specifying the filename to be parsed
 * @param options An or'ed set of CONF_* option bits
 * @return An integer configuration file handle or -1 on error
 * @see CONF_RDONLY, CONF_RDWR, CONF_CREATE
 */
int ggz_conf_parse		(const char *path,
			 const unsigned char options);

/**
 * Commits any changed variables to the configuration file. The configuration
 * file remains open and may continue to be written to.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @return 0 if successful, -1 on failure
 */
int ggz_conf_commit		(int handle);

/**
 * Writes a string value to a section and key in an open configuration file.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name to write to
 * @param key A string variable key name to write to
 * @param value The string value to write
 * @return 0 if successful, -1 on failure
 */
int ggz_conf_write_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*value);

/**
 * Writes an integer value to a section and key in an open configuration file.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name to write to
 * @param key A string variable key name to write to
 * @param value The integer value to write
 * @return 0 if successful, -1 on failure
 */
int ggz_conf_write_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	value);

/**
 * Writes a list of string values to a section and key in an open
 * configuration file.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name to write to
 * @param key A string variable key name to write to
 * @param argc The number of string array entries in argv
 * @param argv An array of strings to create a list from
 * @return 0 if successful, -1 on failure
 */
int ggz_conf_write_list	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	argc,
			 char	**argv);

/**
 * Reads a string value from an open configuration file.  If the section/key
 * combination is not found, the default entry is returned.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name to read from
 * @param key A string variable key name to read from
 * @param def A value to be returned if the entry does not exist (may be NULL)
 * @return A dynamically allocated copy of the stored (or default) value
 * or NULL
 * @note The copy is allocated via a standard malloc() call and the caller
 * is expected to be responsible for calling free() on the returned value
 * when they no longer need the value.  No memory is allocated if a default
 * value of NULL is returned.
 */
char * ggz_conf_read_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*def);

/**
 * Reads an integer value from an open configuration file.  If the section/key
 * combination is not found, the default entry is returned.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name to read from
 * @param key A string variable key name to read from
 * @param def A value to be returned if the entry does not exist
 * @return The integer value stored in the configuration file, or the default
 */
int ggz_conf_read_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	def);

/**
 * Reads a list of string values from an open configuration file.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name to read from
 * @param key A string variable key name to read from
 * @param argcp A pointer to an integer which will receive the number of
 * list entries read
 * @param argvp A pointer to a string array.  This will receive a value
 * pointing to a dynamically allocated array of string values.
 * @return 0 on success, -1 on failure
 * @note The array is allocated via standard malloc() calls and the caller
 * is expected to be responsible for calling free() on the string values
 * and the associated array structure when they no longer need the list.
 * If the section/key combination is not found -1 will be returned, argcp
 * is set to a value of zero, no memory will be allocated, and
 * argvp will retain it's (possibly undefined) value.
 */
int ggz_conf_read_list	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	*argcp,
			 char	***argvp);

/**
 * This will remove an entire section and all it's associated keys from
 * a configuration file.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name to remove
 * @return 0 on success, -1 on failure
 */
int ggz_conf_remove_section	(int	handle,
			 const char	*section);

/**
 * This will remove a single key from a configuration file.
 * @param handle A valid handle returned by ggz_conf_parse()
 * @param section A string section name the key is located in
 * @param key A string key name to remove
 * @return 0 on success, -1 on failure
 */
int ggz_conf_remove_key	(int	handle,
			 const char	*section,
			 const char	*key);
/** @} */



/**
 * @defgroup list List functions 
 * 
 * @{
 */
typedef int	(*ggzEntryCompare)	(void *a, void *b);
typedef	void *	(*ggzEntryCreate)	(void *data);
typedef	void	(*ggzEntryDestroy)	(void *data);


typedef struct _GGZListEntry {
	void				*data;
	struct _GGZListEntry	*next, *prev;
} GGZListEntry;


typedef struct _GGZList {
	GGZListEntry	*head, *tail;
	ggzEntryCompare		compare_func;
	ggzEntryCreate		create_func;
	ggzEntryDestroy		destroy_func;
	int			options;
	int			entries;
} GGZList;



#define GGZ_LIST_REPLACE_DUPS       0x00
#define GGZ_LIST_ALLOW_DUPS         0x01



GGZList *ggz_list_create (ggzEntryCompare compare_func,
			   ggzEntryCreate create_func,
			   ggzEntryDestroy destroy_func,
			   int options);

int ggz_list_insert		(GGZList *list, void *data);
GGZListEntry *ggz_list_head	(GGZList *list);
GGZListEntry *ggz_list_tail	(GGZList *list);
GGZListEntry *ggz_list_next  	(GGZListEntry *entry);
GGZListEntry *ggz_list_prev	(GGZListEntry *entry);
void *ggz_list_get_data		(GGZListEntry *entry);
GGZListEntry *ggz_list_search	(GGZList *list, void *data);
GGZListEntry *ggz_list_search_alt(GGZList *list, void *data,
				  ggzEntryCompare compare_func);

void ggz_list_delete_entry	(GGZList *list, GGZListEntry *entry);
void ggz_list_free		(GGZList *list);
int ggz_list_count		(GGZList *list);


/* String list functions */
int ggz_list_compare_str	(void *a, void *b);
void * ggz_list_create_str	(void *data);
void ggz_list_destroy_str	(void *data);
/** @} */

/**
 * @defgroup stcak Stacks 
 *
 * @{
 */
typedef struct _GGZList GGZStack;

GGZStack* ggz_stack_new(void);
void ggz_stack_push(GGZStack*, void*);
void* ggz_stack_pop(GGZStack*);
void* ggz_stack_top(GGZStack*);
void ggz_stack_free(GGZStack*);
/** @} */

/**
 * @defgroup xml XML parsing 
 * 
 * (currently used on client and server)
 *
 * @{
 */

struct _GGZXMLElement {
	
	/* The name of the tag */
	char *tag;

	/* Text content */
	char *text;

	/* Tag attributes */
	GGZList *attributes;
	
	/* Extra data associated with tag (usually gleaned from children) */
	void *data;

	/* Function to free allocated memory */
	void (*free)();

	/* Function to process tag */
	void (*process)();
};


typedef struct _GGZXMLElement GGZXMLElement;

GGZXMLElement* ggz_xmlelement_new(char *tag, char **attrs, void (*process)(), void (*free)());
void ggz_xmlelement_init(GGZXMLElement *element, char *tag, char **attrs, void (*process)(), void (*free)());

void ggz_xmlelement_set_data(GGZXMLElement*, void *data);

char* ggz_xmlelement_get_tag(GGZXMLElement*);
char* ggz_xmlelement_get_attr(GGZXMLElement *element, char *attr);
void* ggz_xmlelement_get_data(GGZXMLElement*);
char* ggz_xmlelement_get_text(GGZXMLElement*);
void ggz_xmlelement_add_text(GGZXMLElement*, const char *text, int len);

void ggz_xmlelement_free(GGZXMLElement*);
/** @} */

/**
 * @defgroup debug Debug/error logging
 * 
 * (currently used on client and server)
 * @{
 */
void ggz_debug_init(const char **types, const char* file);
void ggz_debug_enable(const char *type);
void ggz_debug_disable(const char *type);
void ggz_debug(const char *type, const char *fmt, ...);
void ggz_error_sys(const char *fmt, ...);
void ggz_error_sys_exit(const char *fmt, ...);
void ggz_error_msg(const char *fmt, ...);
void ggz_error_msg_exit(const char *fmt, ...);
void ggz_debug_cleanup(void);
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
 * replaced with ampersand tags, or NULL.
 * @note The dyanmic memory is allocated using ggz_malloc() and the caller is
 * expected to later free this memory using ggz_free().  If the original string
 * did not contain any characters which required escaping, a NULL value is
 * returned and no memory is allocated.
 */
char * ggz_xml_escape(char *str);
/**
 * Restore escaped XML characters into a text string.
 * @param str The string to decode
 * @return A pointer to a dynamically allocated string with XML ampersand tags
 * replaced with their normal ASCII characters, or NULL.
 * @note The dyanmic memory is allocated using ggz_malloc() and the caller is
 * expected to later free this memory using ggz_free().  If the original string
 * did not contain any characters which required decoding, a NULL value is
 * returned and no memory is allocated.
 */
char * ggz_xml_unescape(char *str);
/** @} */

#endif  /* __GGZCORE_H__ */
