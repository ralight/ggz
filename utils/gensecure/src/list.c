#include "list.h"

/* Global variables */
TLSList *list = NULL;
TLSList *head = NULL;
TLSList *el;

/* Add a pair to the list */
void tls_list_add(int fd, SSL *tls, int active)
{
	el = (TLSList*)malloc(sizeof(TLSList));
	el->fd = fd;
	el->tls = tls;
	el->active = active;
	el->next = NULL;
	if(head)
	{
		head->next = el;
		el->last = head;
	}
	else el->last = NULL;
	head = el;
	if(!list) list = el;
}

/* Remove a pair dynamically from the list */
void tls_list_remove(int fd)
{
	el = list;
	while(el)
	{
		if(el->fd == fd)
		{
			if(el->next)
			{
				if(el->last)
				{
					el->last->next = el->next;
					el->next->last = el->last;
				}
				else
				{
					el->next->last = NULL;
					list = el->next;
				}
			}
			else if(el->last)
			{
				el->last->next = NULL;
				head = el->last;
			}
			else
			{
				list = NULL;
				head = NULL;
			}
			free(el);
			return;
		}
		else el = el->next;
	}
}

/* Try to find a pair, or return NULL */
SSL *tls_list_get(int fd)
{
	el = list;
	while(el)
	{
		if(el->fd == fd) return el->tls;
		el = el->next;
	}
	return NULL;
}

/* Return whether a connection object is active */
int tls_list_active(int fd)
{
	el = list;
	while(el)
	{
		if(el->fd == fd) return el->active;
		el = el->next;
	}
	return -1;
}

