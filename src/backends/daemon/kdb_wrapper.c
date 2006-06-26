/***************************************************************************
                   kdbd.c  -  The server for the daemon backend
                             -------------------
    begin                : Mon Dec 26 2004
    copyright            : (C) 2005 by Yannick Lecaillez
    email                : yl@itioweb.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License (revised).                      *
 *                                                                         *
 ***************************************************************************/


/* Subversion stuff

$Id: kdbd.c 788 2006-05-29 16:30:00Z aviram $

*/


#include <stdlib.h> /* malloc */
#include "kdbbackend.h"

#include "datatype.h"
#include "message.h"

#define	HANDLE_PREALLOC	5

static int storeHandle(KDBHandle *handle);
static int delHandle(int kdbdHandle);
static KDBHandle *getHandle(int kdbdHandle);

static	KDBHandle **handles = NULL;		/* Store KDBHandle opened by client */
static	int handleArraySize = 0;

static int storeHandle(KDBHandle *handle)
{
	KDBHandle	**tmp;
	int		kdbdHandle;
	int		newSize;

	newSize = 0;
	if ( handleArraySize == 0 ) {
		newSize = HANDLE_PREALLOC;
	} else {
		/* Search for a free space ... */
		for(kdbdHandle = 0 ; kdbdHandle < handleArraySize ; kdbdHandle++) {
			if ( handles[kdbdHandle] == NULL )
				break;
		}

		if ( kdbdHandle == handleArraySize ) {
			/* No free space found, extend array */
			newSize = handleArraySize + HANDLE_PREALLOC;
		}
	}

	if ( newSize ) {
		/* No free space found, extend array */
		tmp = realloc(handles, sizeof(KDBHandle *) * (newSize));
		if ( tmp == NULL )
			return -1;
		handles = tmp;
		kdbdHandle = handleArraySize;
		handleArraySize = newSize;
	}

	handles[kdbdHandle] = handle;

	return kdbdHandle;
}

static int delHandle(int kdbdHandle)
{
	if ( kdbdHandle >= handleArraySize )
		return -1;

	handles[kdbdHandle] = NULL;

	return 0;
}

static KDBHandle *getHandle(int kdbdHandle)
{
	if ( kdbdHandle >= handleArraySize )
		return NULL;

	return handles[kdbdHandle];
}

Message *wrapper_kdbOpen(Message *request)
{
	KDBHandle	*handle;
	int		kdbdHandle;
	Message		*reply;
	
	/* Sanity check */
	if ( messageGetNbArgs(request) != 0 ) {
		fprintf(stderr, "wrapper_kdbOpen(): too much args\n");
		return NULL;
	}

	handle = malloc(sizeof(KDBHandle));
	if ( handle == NULL ) {
		perror("wrapper_kdbOpen");
		return NULL;
	}

	kdbOpen(handle);
	kdbdHandle = storeHandle(handle);
	if ( kdbdHandle == -1 ) {
		kdbClose(handle);
		return NULL;
	}

	reply = messageNew(MESSAGE_REPLY, KDB_BE_OPEN, 
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_LAST);

	return reply;	
}

Message *wrapper_kdbClose(Message *request)
{
	KDBHandle	*handle;
	int		kdbdHandle, ret;
	Message		*reply;

	ret = messageExtractArgs(request, 
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_LAST);
	
	if ( ret )
		return NULL;

	if ( (handle = getHandle(kdbdHandle)) )
		ret = kdbClose(handle);
	else
		return NULL;

	reply = messageNew(MESSAGE_REPLY, KDB_BE_CLOSE,
			DATATYPE_INTEGER, &ret, DATATYPE_LAST);

	return reply;
}

Message *wrapper_kdbStatKey(Message *request)
{
	KDBHandle       *handle;
	Key		*key;
	int             ret, kdbdHandle;
	Message		*reply;
	
	key = keyNew(KEY_SWITCH_END);
	ret = messageExtractArgs(request,
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEY, key,
			DATATYPE_LAST);
	
	if ( ret ) {
		fprintf(stderr, "wrapper_kdbStatKey: bad args.\n");
		keyDel(key);
		return NULL;
	}
	
	if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbStatKey(*handle, key);
	} else {
		keyDel(key);
		return NULL;
	}
	
	reply = messageNew(MESSAGE_REPLY, KDB_BE_STATKEY,
			DATATYPE_INTEGER, ret,
			DATATYPE_KEY, key,
			DATATYPE_LAST);
	keyDel(key);
	
	return reply;
}

Message *wrapper_kdbGetKey(Message *request)
{
	KDBHandle       *handle;
	Key		*key;
	int		ret, kdbdHandle;
	Message		*reply;

	key = keyNew(KEY_SWITCH_END);
	ret = messageExtractArgs(request,
				DATATYPE_INTEGER, &kdbdHandle,
				DATATYPE_KEY, key,
				DATATYPE_LAST);
	
	if ( ret ) {
		fprintf(stderr, "wrapper_kdbGetKey(): wrong args\n");
		keyDel(key);
		return NULL;
	}
	
	if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbGetKey(*handle, key);
	} else  {
		keyDel(key);
		return NULL;
	}

	reply = messageNew(MESSAGE_REPLY, KDB_BE_GETKEY,
			DATATYPE_INTEGER, &ret,
			DATATYPE_KEY, key,
			DATATYPE_LAST);
	keyDel(key);
        
	return reply;
}

Message *wrapper_kdbSetKey(void *request)
{
	KDBHandle       *handle;
	Key             *key;
	int             ret, kdbdHandle;
	Message         *reply;
	
	key = keyNew(KEY_SWITCH_END);
	ret = messageExtractArgs(request,
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEY, key,
			DATATYPE_LAST);
	
	if ( ret ) {
		fprintf(stderr, "wrapper_kdbGetKey(): wrong args\n");
		keyDel(key);
		return NULL;
	}
	
	if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbGetKey(*handle, key);
	} else  {
		keyDel(key);
		return NULL;
	}
	
	reply = messageNew(MESSAGE_REPLY, KDB_BE_SETKEY,
			DATATYPE_INTEGER, &ret,
			DATATYPE_KEY, key,
			DATATYPE_LAST);
	keyDel(key);
	
	return reply;
}

Message *wrapper_kdbSetKeys(void *request)
{
	KDBHandle       *handle;
	KeySet		*ks;
	int             ret, kdbdHandle;
	Message         *reply;
	
	ks = ksNew();
	ret = messageExtractArgs(request, 
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEYSET, ks,
			DATATYPE_LAST);

	if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbSetKeys(*handle, ks);
	} else  {
		ksDel(ks);
		return NULL;
	}

	reply = messageNew(MESSAGE_REPLY, KDB_BE_SETKEYS,
			DATATYPE_INTEGER, &ret,
			DATATYPE_KEYSET, ks,
			DATATYPE_LAST);
	ksDel(ks);
	
	return reply;
}

Message *wrapper_kdbRename(void *request)
{
	KDBHandle       *handle;
	Key             *key;
	char		*newKeyName;
	int             ret, kdbdHandle;
	Message         *reply;
	
	key = keyNew(KEY_SWITCH_END);
	ret = messageExtractArgs(request,
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEY, key,
			DATATYPE_STRING, &newKeyName,
			DATATYPE_LAST);

	if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbRename(*handle, key, newKeyName);
		free(newKeyName);
	} else  {
		keyDel(key);
		free(newKeyName);
		return NULL;
	}

	reply = messageNew(MESSAGE_REPLY, KDB_BE_RENAME,
			DATATYPE_INTEGER, &ret,
			DATATYPE_LAST);
	keyDel(key);

	return reply;
}

Message *wrapper_kdbRemoveKey(Message *request)
{
	KDBHandle       *handle;
	Key             *key;
	int             ret, kdbdHandle;
	Message         *reply;
	
	key = keyNew(KEY_SWITCH_END);
	ret = messageExtractArgs(request,
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEY, key,
			DATATYPE_LAST);
	
	if ( ret ) {
		fprintf(stderr, "wrapper_kdbGetKey(): wrong args\n");
		keyDel(key);
		return NULL;
	}
	
	if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbRemoveKey(*handle, key);
		keyDel(key);
	} else  {
		keyDel(key);
		return NULL;
	}
	
	reply = messageNew(MESSAGE_REPLY, KDB_BE_REMOVEKEY,
			DATATYPE_INTEGER, &ret,
			DATATYPE_LAST);
	keyDel(key);
	
	return reply;
}

Message *wrapper_kdbGetChild(Message *request)
{
	KDBHandle       *handle;
	Key             *parentKey;
	KeySet		*ks;
	int             ret, kdbdHandle;
	unsigned long	options;
	Message		*reply;
	
	fprintf(stderr, "kdbGetChild()\n");	
	parentKey = keyNew(KEY_SWITCH_END);
	ret = messageExtractArgs(request,
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEY, parentKey,
			DATATYPE_ULONG, &options,
			DATATYPE_LAST);
	
	if ( ret ) {
		reply = NULL;
		
	} else if ( (handle = getHandle(kdbdHandle)) ) {
		ks = ksNew();
		fprintf(stderr, "kdbGetChild(%s)\n", keyStealName(parentKey));
		ret = kdbGetKeyChildKeys(*handle, parentKey, ks, options);

		reply = messageNew(MESSAGE_REPLY, KDB_BE_GETCHILD,
				DATATYPE_INTEGER, &ret,
				DATATYPE_KEYSET, ks,
				DATATYPE_LAST);
		ksDel(ks);
		
	} else {
		reply = NULL;
	}

	return reply;
}

Message *wrapper_kdbMonitorKey(Message *request)
{
	KDBHandle       *handle;
	Key		*key;
	int             kdbdHandle;
	unsigned long   diffMask, iterations, sleep, ret;
	Message         *reply;
	
	key = keyNew(KEY_SWITCH_END);
	ret = messageExtractArgs(request,
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEY, key,
			DATATYPE_ULONG, &diffMask,
			DATATYPE_ULONG, &iterations,
			DATATYPE_ULONG, &sleep,
			DATATYPE_LAST);
	
	if ( ret ) {
		reply = NULL;
		
	} else if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbMonitorKey(*handle, key, diffMask, iterations, sleep);
		
		reply = messageNew(MESSAGE_REPLY, KDB_BE_MONITORKEY,
				DATATYPE_ULONG, &ret,
				DATATYPE_KEY, key,
				DATATYPE_LAST);
	} else {
		reply = NULL;
	}
	
	keyDel(key);
	
	return reply;
}

Message *wrapper_kdbMonitorKeys(Message *request)
{
	KDBHandle	*handle;
	KeySet		*ks;
	int             kdbdHandle;
	unsigned long	diffMask, iterations, sleep, ret;
	Message         *reply;
	
	ks = ksNew();
	ret = messageExtractArgs(request,
			DATATYPE_INTEGER, &kdbdHandle,
			DATATYPE_KEYSET, ks,
			DATATYPE_ULONG, &diffMask,
			DATATYPE_ULONG, &iterations,
			DATATYPE_ULONG, &sleep,
			DATATYPE_LAST);

	if ( ret ) {
		reply = NULL;
		
	} else if ( (handle = getHandle(kdbdHandle)) ) {
		ret = kdbMonitorKeys(*handle, ks, diffMask, iterations, sleep);
		
		reply = messageNew(MESSAGE_REPLY, KDB_BE_MONITORKEYS,
				DATATYPE_ULONG, &ret,
				DATATYPE_KEYSET, ks,
				DATATYPE_LAST);
	} else {
		reply = NULL;
	}
	ksDel(ks);
	
	return reply;	
	
}
