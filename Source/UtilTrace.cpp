/*
 *  UtilTrace.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "UtilDefs.h"

#ifndef WIN32
	#include <sys/time.h>
#else 
	#include <windows.h>
#endif 

int CUtilTrace::mCount = 0;
CUtilCriticalSection* CUtilTrace::mCriticalSection = NULL;

CUtilTrace::CUtilTrace(char* Filename,char* ModType)
{
	strcpy (mModuleType,ModType);

#ifndef WIN32
	char * appPath = NULL;
	appPath = getcwd(NULL, 0);
	strcpy(mFilename, (const char *)((CUtilString)appPath + "/" +  Filename));
	delete appPath;
#else
 	char appPath[512];
	GetModuleFileName(NULL, appPath, sizeof(appPath));
    char *	p = strrchr (appPath, '\\');
	*p = NULL;
	strcpy(mFilename, (const char *)((CUtilString)appPath + "/" +  Filename));
#endif

	if (mCount++ == 0)
	{
		mCriticalSection = new CUtilCriticalSection();
	}
}

CUtilTrace::~CUtilTrace()
{
	if (--mCount == 0)
	{
		delete mCriticalSection;
	}
}

void CUtilTrace::Trace (const char * Mask, const char * Func, const char* Format, ...) 
{
	va_list		pArg;
	char		Text[20000];
	FILE*		file;
	char		sfecha[100];
	char		filename[512];

#ifndef WIN32
	struct		timeval tv;
#else
    SYSTEMTIME	SystemTime;
#endif

	mCriticalSection->Enter();

	memset(Text, 0, sizeof(Text));
	memset(sfecha, 0, sizeof(sfecha));

	va_start (pArg, Format);
	vsprintf (Text, Format, pArg);

#ifndef WIN32
	gettimeofday (&tv, NULL);
	FormatDate(tv, sfecha);
#else
    GetLocalTime (&SystemTime);
	sprintf(sfecha, "%4d%02d%02d", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay);
#endif

	strcpy(filename, mFilename);
	//strcat(filename, sfecha);
	strcat(filename, ".log");

	file = fopen (filename, "a");
	if (file != NULL)
	{

#ifndef WIN32
		memset(sfecha, 0, sizeof(sfecha));
		FormatDateTime(tv, sfecha);
		fprintf (file, "%s %-10s %30-25s %s\n", sfecha, Mask, Func, Text );
		printf ("%s %-10s %30-25s %s\n", sfecha, Mask, Func, Text );
#else
		fprintf (file, "%2d/%2.2d/%2.2d %2d:%2.2d:%2.2d.%2.2d %08d %-10s %-30s %s\n",
			SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear,
			SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond,
			SystemTime.wMilliseconds/10, GetCurrentThreadId(),
			Mask, Func, 
			Text);
		printf ("%2d/%2.2d/%2.2d %2d:%2.2d:%2.2d.%2.2d %08d %-10s %-30s %s\n",
			SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear,
			SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond,
			SystemTime.wMilliseconds/10, GetCurrentThreadId(),
			Mask, Func, 
			Text);
#endif
		fclose (file);
	}

	mCriticalSection->Leave();

}

void CUtilTrace::TraceBinary (const char * Mask, const char * Func, const char* Text, const byte* Buf, int Length)
{
	int i;
	char Text2[2048];
	int  MaxLength = sizeof(Text2) / 3 - 1;

	Text2[0] = 0;	
	Length = Length<=MaxLength?Length:MaxLength;
	for (i = 0; i < Length; i++)
	{
		sprintf (Text2+i*3, "%2.2X ", Buf[i] & 0xff);
	}
	Trace (Mask, Func, Text, Text2);
}

#ifndef WIN32

void CUtilTrace::FormatDate(struct timeval date, char* dateFormatted)
{
	char	time_string[100];
	struct	tm* ptm;

	ptm = (struct tm*)localtime (&date.tv_sec);
	strftime (time_string, sizeof (time_string), "%Y-%m-%d", ptm);
	sprintf (dateFormatted, "%s", time_string);

	return;
}

void CUtilTrace::FormatDateTime(struct timeval dateTime, char* dateTimeFormatted)
{
	char	time_string[100];
	struct	tm* ptm;
	long	milliseconds;

	ptm = (struct tm*)localtime (&dateTime.tv_sec);
	strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	milliseconds = dateTime.tv_usec / 1000;
	sprintf (dateTimeFormatted, "%s.%03ld ", time_string, milliseconds);

	return;    
}

#endif
