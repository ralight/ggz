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

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Under gcc, we use the __attribute__ macro to check variadic arguments,
   for instance to printf-style functions.  Other compilers may be able
   to do something similar.  Great for debugging. */
#ifdef __GNUC__
#  define ggz__attribute(att)  __attribute__(att)
#else
#  define ggz__attribute(att)
#endif

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
 * @return a pointer to the newly allocated and zeroed memory
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
 *
 * @note It is safe to pass a NULL string.
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
 * @return pointer to newly allocated, zeroed memory
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
 *
 * @note It is safe to pass a NULL string.
 */
char * _ggz_strdup(const char *, char *, int);


/** 
 * Check memory allocated against memory freed and display any discrepencies
 * 
 * 
 * @return 
 */
int ggz_memory_check(void);

/** @} */



/**
 * @defgroup conf Configuration file parsing
 * Configuration file routines to store and retrieve values.
 * Configuration file parsing begins by calling ggz_conf_parse() to open
 * a config file.  The file can be created automatically if GGZ_CONF_CREATE
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
 * Specifies the mode for opening a configuration file
 * @see ggz_conf_parse()
 */
typedef enum {
	GGZ_CONF_RDONLY = ((unsigned char) 0x01),
	GGZ_CONF_RDWR = ((unsigned char) 0x02),
	GGZ_CONF_CREATE = ((unsigned char) 0x04)
} GGZConfType;


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
 * @param options An or'ed set of GGZ_CONF_* option bits
 * @return An integer configuration file handle or -1 on error
 * @see GGZConfType
 */
int ggz_conf_parse		(const char *path,
				 const GGZConfType options);

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
 * @note The copy is allocated via a standard ggz_malloc() call and the
 * caller is expected to be responsible for calling ggz_free() on the
 * returned value when they no longer need the value.  No memory is allocated
 * if a default value of NULL is returned.
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
 * @note The array is allocated via standard ggz_malloc() calls and the
 * caller is expected to be responsible for calling ggz_free() on the string
 * values and the associated array structure when they no longer need the
 * list.  If the section/key combination is not found -1 will be returned,
 * argcp is set to a value of zero, no memory will be allocated, and
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
 * Functions for debugging and error messages
 * @{
 */
typedef enum {
	GGZ_CHECK_NONE = 0x00,
	GGZ_CHECK_MEM = 0x01
} GGZCheckType;

/**
 * @brief Initialize and configure debugging for the program.
 * 
 * This should be called early in the program to set up the debugging routines.
 * @param types A null-terminated list of arbitrary string debugging "types".
 * @param file A file to write debugging output to, or NULL for none.
 * @see ggz_debug
 */
void ggz_debug_init(const char **types, const char* file);

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
void ggz_error_sys(const char *fmt, ...)
                   ggz__attribute((format(printf, 1, 2)));
void ggz_error_sys_exit(const char *fmt, ...)
                        ggz__attribute((format(printf, 1, 2)));
void ggz_error_msg(const char *fmt, ...)
                   ggz__attribute((format(printf, 1, 2)));
void ggz_error_msg_exit(const char *fmt, ...)
                        ggz__attribute((format(printf, 1, 2)));

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
char * ggz_xml_escape(char *str);
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
char * ggz_xml_unescape(char *str);

/**
 * Structure used internally by ggz_read_line()
 */
struct _GGZFile {
	int fdes;	/* File descriptor */
	char *buf;	/* Data buffer */
	char *p;	/* Current position in buffer */
	char *e;	/* Points one char past end of valid data in buffer */
	int bufsize;	/* Current buffer size */
};

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

/** @} */

/**
 * @defgroup easysock Easysock IO
 * 
 * Simple functions for reading/writing binary data across file descriptors
 *
 * @{
 */

/****************************************************************************
 * Error function
 *
 * Any error encountered in an easysock function will cause the error 
 * function to be called if one has been set.
 * 
 * Upon removal, the previous error function is returned
 *
 ***************************************************************************/
typedef enum {
	GGZ_IO_CREATE,
	GGZ_IO_READ,
	GGZ_IO_WRITE,
	GGZ_IO_ALLOCATE
} GGZIOType;

typedef enum {
	GGZ_DATA_NONE,
	GGZ_DATA_CHAR,
	GGZ_DATA_INT,
	GGZ_DATA_STRING,
	GGZ_DATA_FD
} GGZDataType;



/****************************************************************************
 * Error function
 *
 * Any easysock function that generates an error will call this
 * function, if defined.  
 * 
 * Upon removal, the old error function is returned.
 *
 ***************************************************************************/
typedef void (*ggzIOError) (const char *, const GGZIOType, const GGZDataType);
			      

int ggz_set_io_error_func(ggzIOError func);
ggzIOError ggz_remove_io_error_func(void);


/****************************************************************************
 * Exit function
 *
 * Any of the *_or_die() functions will call the set exit function
 * if there is an error.  If there is no set function, exit() will be 
 * called.
 * 
 * Upon removal, the old exit function is returned.
 *
 ***************************************************************************/
typedef void (*ggzIOExit) (int);

int ggz_set_io_exit_func(ggzIOExit func);
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
 */
unsigned int ggz_set_io_alloc_limit(const unsigned int limit);


/****************************************************************************
 * Creating a socket.
 * 
 * type  :  one of GGZ_SERVER or GGZ_CLIENT
 * port  :  tcp port number 
 * server:  hostname to connect to (only relevant for client)
 * 
 * Returns socket fd or -1 on error
 ***************************************************************************/
typedef enum {
	GGZ_SOCK_SERVER,
	GGZ_SOCK_CLIENT
} GGZSockType;
	
int ggz_make_socket(const GGZSockType type, 
		    const unsigned short port, 
		    const char *server);

int ggz_make_socket_or_die(const GGZSockType type,
			   const unsigned short port, 
			   const char *server);

int ggz_make_unix_socket(const GGZSockType type, const char* name);
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


/****************************************************************************
 * Reading/Writing an integer in network byte order
 * 
 * sock  :  socket fd
 * data  :  int for write.  pointer to int for read
 * 
 * Returns 0 if successful, -1 on error.
 ***************************************************************************/
int ggz_write_int(const int sock, const int data);
void ggz_write_int_or_die(const int sock, const int data);
int ggz_read_int(const int sock, int *data);
void ggz_read_int_or_die(const int sock, int *data);


/****************************************************************************
 * Reading/Writing a char string
 * 
 * sock  : socket fd
 * data  : char string or address of char string for alloc func
 * fmt   : format string for sprintf-like behavior  
 * len   : length of user-provided data buffer in bytes
 * 
 * Returns 0 if successful, -1 on error.
 *
 ***************************************************************************/
int ggz_write_string(const int sock, const char *data);
void ggz_write_string_or_die(const int sock, const char *data);
int ggz_va_write_string(const int sock, const char *fmt, ...);
void ggz_va_write_string_or_die(const int sock, const char *fmt, ...);
int ggz_read_string(const int sock, char *data, const unsigned int len);
void ggz_read_string_or_die(const int sock, char *data, const unsigned int len);
int ggz_read_string_alloc(const int sock, char **data);
void ggz_read_string_alloc_or_die(const int sock, char **data);


/****************************************************************************
 * Reading/Writing a file descriptor
 * 
 * sock  : socket fd
 * data  : address of data to be read/written
 * n     : size of data (in bytes) to be read/written
 * 
 * Returns 0 on success, or -1 on error
 *
 * Many thanks to Richard Stevens and his wonderful books, from which
 * these functions come.
 *
 ***************************************************************************/
int ggz_read_fd(const int sock, int *recvfd);
int ggz_write_fd(const int sock, int sendfd);

/****************************************************************************
 * Reading/Writing a byte sequence 
 * 
 * sock  : socket fd
 * data  : address of data to be read/written
 * n     : size of data (in bytes) to be read/written
 * 
 * Returns number of bytes processed, or -1 on error
 *
 * Many thanks to Richard Stevens and his wonderful books, from which
 * these functions come.
 *
 ***************************************************************************/
int ggz_readn(const int sock, void *data, size_t n);
int ggz_writen(const int sock, const void *vdata, size_t n);

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __GGZ_H__ */
