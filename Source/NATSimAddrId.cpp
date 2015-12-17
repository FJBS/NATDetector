/*
 *  NATSimIPId.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATSimDefs.h"

CNATSimAddrId::CNATSimAddrId()
{
	mIp = "";
	mPort = 0;
}

CNATSimAddrId::CNATSimAddrId(CUtilString Ip, long Port)
{
	mIp = Ip;
	mPort = Port;
}

CNATSimAddrId::~CNATSimAddrId()
{
}

CNATSimAddrId & CNATSimAddrId::operator = (const CNATSimAddrId & other)
{
	this->mIp = other.mIp;
	this->mPort = other.mPort;
	
	return *this;
}

bool CNATSimAddrId::operator < (const CNATSimAddrId & other) const
{
	bool res = false;

	res |= (this->mIp  < other.mIp);
	res |= (this->mIp == other.mIp) && (this->mPort  < other.mPort);

	return res;
}

bool CNATSimAddrId::operator == (const CNATSimAddrId & other) const
{
	bool res = false;

	res |= (this->mIp == other.mIp) && (this->mPort  == other.mPort);

	return res;
}

bool CNATSimAddrId::operator > (const CNATSimAddrId & other) const
{
	bool res = false;

	res |= (this->mIp  > other.mIp);
	res |= (this->mIp == other.mIp) && (this->mPort  > other.mPort);

	return res;
}

CUtilString CNATSimAddrId::ToString()
{
	CUtilString ret;
	
	ret = "(";
	ret += mIp + ",";
	ret += ",";
	ret += mPort + ",";
	ret += ")";
	
	return ret;
}
