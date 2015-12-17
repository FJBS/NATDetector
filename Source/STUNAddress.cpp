/*
 *  STUNAddress.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 27/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

// TODO: Implementar con struct inet_addr_t
CSTUNAddress::CSTUNAddress()
{
	mPort = 0;
	mAddress = new byte[4];
	mAddress[0] = 127;
	mAddress[1] = 0;
	mAddress[2] = 0;
	mAddress[3] = 1;
	mAddressLength = 4;
}

CSTUNAddress::CSTUNAddress(CUtilString address, word port)
{
	struct in_addr addr;
	addr.s_addr = inet_addr((const char *)address);
	
	// TODO: BIG ENDIAN or LITLE ENDIAN
	mAddress = new byte[4];
	mAddress[3] = (byte)((addr.s_addr & 0xFF000000)>>24);
	mAddress[2] = (byte)((addr.s_addr & 0x00FF0000)>>16);
	mAddress[1] = (byte)((addr.s_addr & 0x0000FF00)>>8);
	mAddress[0] = (byte)((addr.s_addr & 0x000000FF));
	mAddressLength = 4;
	mPort = port;	
}

CSTUNAddress::CSTUNAddress(byte* address, byte addressLength, word port)
{
	mAddress = new byte[addressLength];
	memcpy(mAddress, address, addressLength);
	mAddressLength = addressLength;
	mPort = port;	
}

CSTUNAddress::CSTUNAddress(struct in_addr addr, u_short port)
{
	mAddress = new byte[4];
	mAddress[0] = (byte)((addr.s_addr & 0xFF000000)>>24);
	mAddress[1] = (byte)((addr.s_addr & 0x00FF0000)>>16);
	mAddress[2] = (byte)((addr.s_addr & 0x0000FF00)>>8);
	mAddress[3] = (byte)((addr.s_addr & 0x000000FF));
	mAddressLength = 4;
	mPort = port;	
}

CSTUNAddress::CSTUNAddress (const CSTUNAddress & other)
{
	this->mAddress = new byte[other.mAddressLength];
	this->mAddressLength = other.mAddressLength;
	memcpy (this->mAddress, other.mAddress, other.mAddressLength);
	this->mPort = other.mPort;
}

CSTUNAddress::~CSTUNAddress()
{
	delete mAddress;
}

CUtilByteArray & CSTUNAddress::getAddressBytes()
{
	CUtilByteArray * ret = NULL;
	
	if ( mAddressLength > 0 )
		ret = new CUtilByteArray(mAddress, mAddressLength); 
	else
		ret = new CUtilByteArray();
		
	return *ret;
}

word CSTUNAddress::getPort()
{
	return mPort;
}

bool CSTUNAddress::equals(const CSTUNAddress & other)
{
	bool ret = false;
	
	if (	mPort == other.mPort										&& 
			( memcmp(mAddress, other.mAddress, mAddressLength) == 0)	&& 
			mAddressLength == other.mAddressLength ) 
	{
		ret = true;
	}
	
	return ret;
}

bool CSTUNAddress::operator == (const CSTUNAddress & other)
{
	return this->equals(other);
}

CUtilString CSTUNAddress::ToString()
{
	CUtilString ret;

	if ( mAddressLength > 0 )
	{
		char buffer[4];
		
		for ( int i = 0; i < mAddressLength - 1; i++ )
		{
			sprintf(buffer, "%d", mAddress[i]);
			ret += CUtilString(buffer);
			ret += CUtilString('.');
		}
		sprintf(buffer, "%d", mAddress[mAddressLength-1]);
		ret += CUtilString(buffer);
	}		

	return ret;
}

CSTUNAddress & CSTUNAddress::operator = (const CSTUNAddress & other)
{
	delete []this->mAddress;
	this->mAddress = new byte[other.mAddressLength];
	this->mAddressLength = other.mAddressLength;
	memcpy (this->mAddress, other.mAddress, other.mAddressLength);
	this->mPort = other.mPort;

	return *this;
}

