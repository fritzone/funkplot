#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "List.h"
#include "util.h"

/*
 * This is a hashlist. It maps different values to strings, throught its exposed methods.
 * The implementation is done with a void list, and a new hash_struct is created for every entry.
 * the hash struct will point toward to the object, that will be mapped to the hashtable
 */
class HashTable
{
public:
	HashTable()
	{
		debug("created new HashTable");
		hashlist=new List();
		if(hashlist==nullptr)
		{
			debug("hashlist is pretty null???");
		}
	}


	/*
	 * This function maps to different keynames different objects (values. anything)
	 * At this moment the corespondence is bijective: "KEY" -> OneObject
	 * In the future it will be possible to assign more objects to the same key.
	 */
	int MapElement(char *key, void *elm)
	{
		debug("map first step");
		hash_struct *hs=HasKey(key);
		
		if(hs)
		{
			hs->data=elm;
		}
		else
		{
			hs=new hash_struct;
			hs->name=new char[strlen(key)];
			strcpy(hs->name,key);
			hs->data=elm;
			hashlist->AddElement(hs);
		}
		return 1;
	}


	/*
	 * This return the value mapped for the key
	 */
	void *GetElementFor(char *key)
	{
		hash_struct *hs=HasKey(key);
		
		if(hs)
		{
			return hs->data;
		}
		else
		{
			return nullptr;
		}

	}

	/*
	 * Returns the hash_struct of a key, if it is in the list
	 */
	hash_struct *HasKey(char *key)
	{
		debug("in has_key");
		if(hashlist==nullptr)
		{
			debug("HASHLIST is nullptr");

		}
		list *posl = hashlist->Enumerate((ENUMERATOR)(keyz), key);
		debug("after enumerating");
		if(posl)
		{
			return (hash_struct*)(posl->inf);
		}
		else
			return nullptr;
	}

private:
	List *hashlist;


};

#endif