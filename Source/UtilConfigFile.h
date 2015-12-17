/*
 *  UtilConfigFile.h
 *  NATDetector
 *
 *  Created by System Administrator on 20/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

class CUtilConfigFile
{

public:

	CUtilConfigFile(CUtilString fileConfig, CUtilTrace * Trace);
	~CUtilConfigFile();
	
	CUtilString GetString(CUtilString section, CUtilString key, const CUtilString byDefault) const;
	long GetLong(CUtilString section, CUtilString key, const long byDefault) const;
	
	CArray<CUtilString> GetStringList(CUtilString section, CUtilString key) const;

private:

	CUtilTrace	*	mTrace;
	CUtilString		mFileConfig;


	CUtilString	ReadLine(const int fd, bool & eof) const;
};
