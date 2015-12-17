/*
 *  UtilCriticalSection.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 12/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */
 
#pragma once

class CUtilCriticalSection
{
public:
	CUtilCriticalSection();
	~CUtilCriticalSection();

	void Enter() const;
	void Leave() const;
	
private:
	pthread_mutex_t		*mMutex;
	pthread_mutexattr_t	*mMutexattr;
};
