/*
 *  STUNTransactionID.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

const byte CSTUNTransactionID::TRANSACTION_ID_LENGTH			= 16;

CSTUNTransactionID::CSTUNTransactionID()
{
	mTransactionID = new byte[TRANSACTION_ID_LENGTH];
	memset(mTransactionID, 0, TRANSACTION_ID_LENGTH);

#ifndef WIN32

	struct		timeval tv;
	gettimeofday (&tv, NULL);
	
	short left  = tv.tv_usec / 1000;
	short right = random();

#else

	srand( (unsigned)time( NULL ) );

	dword left  = GetTickCount();
	dword right = rand();

#endif

	for(int i = 0; i < 8; i++)
	{
		mTransactionID[i]   = (byte)((left  >> (i*8))& 0xFF);
		mTransactionID[i+8] = (byte)((right >> (i*8))& 0xFF);
	}
}

CSTUNTransactionID::CSTUNTransactionID(byte* transactionID)
{
	mTransactionID = new byte[TRANSACTION_ID_LENGTH];
	memcpy(mTransactionID, transactionID, TRANSACTION_ID_LENGTH);
}

CSTUNTransactionID::CSTUNTransactionID(CSTUNTransactionID & other)
{
	mTransactionID = new byte[TRANSACTION_ID_LENGTH];
	memcpy(mTransactionID, (const byte*)other, TRANSACTION_ID_LENGTH);
}

CSTUNTransactionID::~CSTUNTransactionID()
{
	delete 	mTransactionID;
}

bool CSTUNTransactionID::operator == (const CSTUNTransactionID &other) const
{
	bool ret = false;
	ret = memcmp(this->mTransactionID, other.mTransactionID, TRANSACTION_ID_LENGTH) == 0;
	return ret;
}

CSTUNTransactionID & CSTUNTransactionID::operator = (const CSTUNTransactionID &other)
{
	memcpy(this->mTransactionID , other.mTransactionID, TRANSACTION_ID_LENGTH);
	
	return *this;
}

CSTUNTransactionID::operator const byte* () const
{
	return this->mTransactionID;
}

CSTUNTransactionID::operator const CUtilString () const
{
	int  i;
	char Text[(TRANSACTION_ID_LENGTH*3)+1];

	Text[0] = 0;	
	for (i = 0; i < TRANSACTION_ID_LENGTH; i++)
	{
		sprintf (Text+i*3, "%2.2X ", mTransactionID[i] & 0xff);
	}

	return CUtilString(Text);
}
