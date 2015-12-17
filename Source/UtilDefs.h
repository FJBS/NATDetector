/*
 *  Defs.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */
 
#pragma once

#include <iostream>

#ifndef WIN32
#   include <unistd.h>
#else
#   define ssize_t int
#   include <io.h>
#   include <sys/types.h>
#endif

#ifndef WIN32
#	include <pthread.h>
#else /* POSIX-like */
#	include <pthread.h>
#endif 

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <fcntl.h>

typedef unsigned char 	byte;
typedef unsigned short	word;
typedef unsigned long	dword;

#include "UtilCriticalSection.h"
#include "UtilTrace.h"
#include "UtilTemplates.h"
#include "UtilString.h"
#include "UtilException.h"
#include "UtilByteArray.h"
#include "UtilConfigFile.h"

typedef void (__stdcall *AdviseSndFunction)(int index, const char * FromIP, dword FromPort, const  char * ToIP, dword ToPort, CUtilByteArray message, void *object);
typedef void (__stdcall *AdviseRcvFunction)(int index, const char * FromIP, dword FromPort, const  char * ToIP, dword ToPort, CUtilByteArray message, void *object);
	