/*
 *  UTilByteArray.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 18/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "UTilDefs.h"

CUtilByteArray::CUtilByteArray ()
{
	mData = NULL;
	mLength = 0;
}

CUtilByteArray::CUtilByteArray (const byte * data, dword length)
{
	if (data != NULL )
	{
		this->mData = new byte[length];
		memcpy(this->mData, data, length);
		mLength = length;
	}
	else
	{
		mData = NULL;
		mLength = 0;
	}
}

CUtilByteArray::CUtilByteArray (const CUtilByteArray & other)
{
	if (other.mData != NULL )
	{
		this->mData = new byte[other.mLength];
		memcpy(this->mData, other.mData, other.mLength);
		this->mLength = other.mLength;
	}
	else
	{
		mData = NULL;
		mLength = 0;
	}
}

CUtilByteArray::CUtilByteArray (const CUtilByteArray * other)
{
	if (other->mData != NULL )
	{
		this->mData = new byte[other->mLength];
		memcpy(this->mData, other->mData, other->mLength);
		this->mLength = other->mLength;
	}
	else
	{
		mData = NULL;
		mLength = 0;
	}
}

CUtilByteArray::~CUtilByteArray ()
{
	delete []this->mData;
}

bool CUtilByteArray::operator == (const CUtilByteArray &other) const
{
	if (this->mData == NULL  &&  other.mData == NULL)
		return true;
	if (this->mData == NULL  &&  other.mData != NULL)
		return false;
	if (this->mData != NULL  &&  other.mData == NULL)
		return false;
	if (this->mLength != other.mLength )
		return false;
	return memcmp (this->mData, other.mData, this->mLength) == 0;
}

bool CUtilByteArray::operator != (const CUtilByteArray &other) const
{
	return ! (*this == other);
}

CUtilByteArray &CUtilByteArray::operator = (const CUtilByteArray & other) 
{
	if (this == &other)
		return *this;

	delete []this->mData;
	if (other.mData != NULL)
	{
		this->mData = new byte[other.mLength];
		this->mLength = other.mLength;
		memcpy (this->mData, other.mData, other.mLength);
		
	}
	else
	{
		this->mData = NULL;
		this->mLength = 0;
	}
	return *this;
}
	
CUtilByteArray::operator const byte * () const
{
	return this->mData;
}

bool CUtilByteArray::is_null() const
{
	return this->mData == NULL;
}

dword CUtilByteArray::length () const
{
	return this->mLength;
}
