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
 * @defgroup conf Config file parsing
 *
 * (currently exists in server and ggzcore )
 *
 * @{
 */
#define CONF_RDONLY	((unsigned char) 0x01)
#define CONF_RDWR	((unsigned char) 0x02)
#define CONF_CREATE	((unsigned char) 0x04)

int ggz_conf_initialize	(const char	*g_path,
			 const char	*u_path);
void ggz_conf_cleanup (void);

int ggz_conf_parse		(const char *path,
			 const unsigned char options);
int ggz_conf_commit		(int handle);
int ggz_conf_write_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*value);
int ggz_conf_write_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	value);
int ggz_conf_write_list	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	argc,
			 char	**argv);
char * ggz_conf_read_string	(int	handle,
			 const char	*section,
			 const char	*key,
			 const char	*def);
int ggz_conf_read_int	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	def);
int ggz_conf_read_list	(int	handle,
			 const char	*section,
			 const char	*key,
			 int	*argcp,
			 char	***argvp);
int ggz_conf_remove_section	(int	handle,
			 const char	*section);
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

#endif  /* __GGZCORE_H__ */
