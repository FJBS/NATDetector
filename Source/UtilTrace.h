/*
 *  UtilTrace.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */
 
#pragma once

class CUtilTrace
{
public:
	CUtilTrace(char* Filename,char* ModType);
	~CUtilTrace();
	
	//void Trace (const char* format, ...);
	//void TraceBinary (const char* Text, const byte* Buf, int Length);

	void Trace (const char * Mask, const char * Func, const char* format, ...);
	void TraceBinary (const char * Mask, const char * Func, const char* Text, const byte* Buf, int Length);

private:
	static int					mCount;

	char						mFilename[512];
	char						mModuleType[512];

	static CUtilCriticalSection	*mCriticalSection;
	
private:

#ifndef WIN32

	void FormatDate(struct timeval date, char* dateFormatted);
	void FormatDateTime(struct timeval dateTime, char* dateTimeFormatted);

#endif

};
