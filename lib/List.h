#ifndef _LIST_H_
#define _LIST_H_

#include "util.h"

/*
 * This class represents a generic list. There will be NO derivated lists from this.
 * Every object needst to know how to handle his list.
 */
class List
{

public:
	/*
	 * This initializes the two values needed here
	 */
	List()
	{
		debug("created ne list");
		first=nullptr;
		if(first==nullptr)
		{
			//debug
			char s[256];
			sprintf(s,"first(%i) is nullptr",first);
			debug(s);
			//end debug

		}
		p=nullptr;
	}

	/*
	 * This adds an address to the list
	 */
	void AddElement(void *dat)
	{
		list *tmp=nullptr;
		if(first==nullptr)
		{
			first=new list;
			p=first;
			first->next=nullptr;
			first->inf=dat;
			return;
		}
		tmp=new list;
		if(!tmp)
		{
			//handle the insuff memory error
		}
		tmp->inf=dat;
		tmp->next=nullptr;
		p->next=tmp;
		p=p->next;
	}

	/*
	 * This will return nullptr if the desired adress was not in the list
	 * Else it will return the address at which the desired addrs is
	 */
	list *InList(void *desiree)
	{
		list *q=first;
		while(q)
		{
			if(q->inf==desiree)
			{
				return q;
			}
			q=q->next;
		}
		return nullptr;
	}

	/*
	 * This removes from the list the item, which holds ezt
	 */
	void RemoveElement(void* ezt)
	{
		list *q=first,*q1=q;
		while(q)
		{
			if(q->inf==ezt)
			{
				if(q==first)
				{
					first=q->next;
				}
				else
				{
					q1->next=q->next;
				}
				delete q;
				return;
			}
			q1=q;
			q=q->next;
		}
	}

	/*
	 * This function calls the ENUMERATOR for each list element. If depending on the
	 * values, that were passed to the enumerator it returns 0 no more enumeration will be, and
	 * this function returns the element, which caused the enumerator to return 0.
	 * Otherwere it returns nullptr after passing each element to the enumerator function.
	 * And of course, the parameter is passed to the enumerator function
	 */
	list* Enumerate(ENUMERATOR func, void * parm)
	{
		if(first==nullptr)
		{
			return nullptr;
		}
		debug("first is not null");
		list *q=first;
		
		while(q!=nullptr)
		{
			//debug
			char s[256];
			sprintf(s,"%i",first);
			debug(s);
			//end debug
			if(func(q->inf, parm) == 0)
			{
				return q;
			}
			q=q->next;
		}
		return nullptr;
	}
	

private:
	list *first, *p;
};

#endif