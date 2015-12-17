/*
 *  UtilConfigFile.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 20/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "UtilDefs.h"

#ifdef WIN32
	#include <windows.h>
#endif 

CUtilConfigFile::CUtilConfigFile(CUtilString fileConfig, CUtilTrace * Trace)
{	
#ifndef WIN32
	char * appPath = NULL;
	appPath = getcwd(NULL, 0);
	mFileConfig = (CUtilString)appPath + "/" +  fileConfig;
	delete appPath;
#else
 	char Filename[512];
	GetModuleFileName(NULL, Filename, sizeof(Filename));
    char *	p = strrchr (Filename, '\\');
	*p = NULL;
	mFileConfig = (CUtilString)Filename + "/" +  fileConfig;
#endif

	mTrace = Trace;

}

CUtilConfigFile::~CUtilConfigFile()
{
}

CUtilString	CUtilConfigFile::GetString(CUtilString section, CUtilString key, const CUtilString byDefault) const
{
	CUtilString ret = byDefault;
	bool		eof = false;
	int			fd;
	CUtilString	line;
	
	if ( (fd = open((const char *)mFileConfig, O_RDONLY)) < 0 )
	{
		CUtilString aux = (CUtilString)("Failed opening ") + mFileConfig + "returning default value " + byDefault;
		mTrace->Trace("**ERROR**", __FUNCTION__, (const char *)aux);
	}
	else
	{
		do {
		
			line = this->ReadLine(fd, eof);
			
			if ( line == ((CUtilString)"#" + section) )
			{
				do {
				
					line = this->ReadLine(fd, eof);
					
					if ( line.substr(0, key.length()) == key )
					{
						ret = line.substr(key.length() + 1);
						eof = true;
						break;
					}
					
				} while ( !eof );
				
			}
			
		} while ( !eof );
		
		close(fd);
	}
	
	return ret;	
}

long CUtilConfigFile::GetLong(CUtilString section, CUtilString key, const long byDefault) const
{
	long	ret = byDefault;
	char	buffer[30];

	sprintf(buffer, "%d", byDefault);
	CUtilString byDefaultStr = (CUtilString)buffer;
	
	CUtilString retString = GetString(section, key, byDefaultStr);
	if ( retString.is_integer() )
		ret = atol((const char *)retString);
	else
		ret = byDefault;	
	
	return ret;
}

CUtilString	CUtilConfigFile::ReadLine(const int fd, bool & eof) const
{
	CUtilString ret = "";
	ssize_t		bytes;
	char		buf;
	
	eof = false;
	
	// TODO: EOF reached ??
	while ( !eof )
	{
		bytes = read(fd, &buf, 1);
	
		if ( bytes > 0 )
		{
			if ( buf == '\n' )
				break;
			else
				ret += buf;
		}
		else
			eof = true;
	}
	
	return ret;
}

CArray<CUtilString> CUtilConfigFile::GetStringList(CUtilString section, CUtilString key) const
{
	CArray<CUtilString> retList;
	int					i = 1;
	char				str[10];
	CUtilString			aux;
	
	sprintf(str, "%d", i++);
	aux = GetString(section, key + str, "");
	while ( aux != "" )
	{
		retList.Add(aux);
		sprintf(str, "%d", i++);
		aux = GetString(section, key + str, "");
	}
	
	return retList;
}


