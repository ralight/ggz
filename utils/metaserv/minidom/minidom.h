#ifndef MINIDOM_H
#define MINIDOM_H

struct att_t
{
	char *name;				/* name of the attribute */
	char *value;			/* value of the attribute */
};

struct ele_t
{
	struct ele_t *parent;	/* pointer to the parent */
	struct ele_t **el;		/* list of child elements */
	struct att_t **at;		/* list of attributes */
	char *name;				/* tag identifier */
	char *value;			/* value inside the tag*/
	int elnum;				/* number of child elements [redundant] */
	int atnum;				/* number of attribute pairs [redundant] */
};

struct dom_t
{
	struct ele_t *el;		/* the root node (may be NULL) */
	int valid;				/* validity of the DOM */
	int processed;			/* indicates whether you can work with it */
};

typedef struct dom_t DOM;	/* Domain Object Model */
typedef struct ele_t ELE;	/* Elements */
typedef struct att_t ATT;	/* Attributes */

#endif

