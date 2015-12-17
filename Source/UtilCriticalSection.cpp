/*
 *  UtilCriticalSection.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 12/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "UtilDefs.h"

CUtilCriticalSection::CUtilCriticalSection()
{
	mMutexattr = new pthread_mutexattr_t;
	mMutex = new pthread_mutex_t;
	pthread_mutexattr_init(mMutexattr);
	pthread_mutexattr_settype((pthread_mutexattr_t*)mMutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init((pthread_mutex_t*)mMutex, mMutexattr);	
}

CUtilCriticalSection::~CUtilCriticalSection()
{
	pthread_mutexattr_destroy((pthread_mutexattr_t*)mMutexattr);
	pthread_mutex_destroy ((pthread_mutex_t*)mMutex);
	delete mMutexattr;
	delete mMutex;
}

void CUtilCriticalSection::Enter() const
{
	pthread_mutex_lock ((pthread_mutex_t*)mMutex);
}

void CUtilCriticalSection::Leave() const
{
	pthread_mutex_unlock ((pthread_mutex_t*)mMutex);
}
