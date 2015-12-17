/*
 *  UTilByteArray.h
 *  NATDetector
 *
 *  Created by System Administrator on 18/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

class CUtilByteArray
{
public:

	CUtilByteArray ();

	CUtilByteArray (const byte * data, dword length);

	CUtilByteArray (const CUtilByteArray & other);

	CUtilByteArray (const CUtilByteArray * other);

	virtual ~CUtilByteArray ();
	
	bool operator == (const CUtilByteArray &other) const;
	bool operator != (const CUtilByteArray &other) const;
	
	CUtilByteArray & operator = (const CUtilByteArray & other);
		
	operator const byte * () const;
	
	bool is_null() const;

	dword length () const;
	
private:
	byte	*mData;
	dword	mLength;
};
