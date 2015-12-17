/*
 *  NATSimIPId.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATSimDefs.h"

CNATSimTrafficId::CNATSimTrafficId()
{
	mIpS = "";
	mPortS = 0;
	mIpD = "";
	mPortD = 0;
}

CNATSimTrafficId::CNATSimTrafficId(const CNATSimTrafficId & other)
{
	this->mIpS = other.mIpS;
	this->mIpD = other.mIpD;
	this->mPortS = other.mPortS;
	this->mPortD = other.mPortD;
}

CNATSimTrafficId::CNATSimTrafficId(CUtilString IpS, dword PortS, CUtilString IpD, dword PortD)
{
	mIpS = IpS;
	mPortS = PortS;
	mIpD = IpD;
	mPortD = PortD;
}

CNATSimTrafficId::~CNATSimTrafficId()
{
}

CNATSimTrafficId & CNATSimTrafficId::operator = (const CNATSimTrafficId & other)
{
	this->mIpS = other.mIpS;
	this->mPortS = other.mPortS;

	this->mIpD = other.mIpD;
	this->mPortD = other.mPortD;
	
	return *this;
}

bool CNATSimTrafficId::operator < (const CNATSimTrafficId & other) const
{
	bool res = false;

	res |= (this->mIpS  < other.mIpS);
	res |= (this->mIpS == other.mIpS) && (this->mPortS  < other.mPortS);
	res |= (this->mIpS == other.mIpS) && (this->mPortS  == other.mPortS) && (this->mIpD < other.mIpD);
	res |= (this->mIpS == other.mIpS) && (this->mPortS  == other.mPortS) && (this->mIpD == other.mIpD) && (this->mPortD  < other.mPortD);

	return res;
}

bool CNATSimTrafficId::operator == (const CNATSimTrafficId & other) const
{
	bool res = false;

	res |= (this->mIpS == other.mIpS) && (this->mPortS  == other.mPortS) && (this->mIpD == other.mIpD) && (this->mPortD  == other.mPortD);

	return res;
}

bool CNATSimTrafficId::operator > (const CNATSimTrafficId & other) const
{
	bool res = false;

	res |= (this->mIpS  > other.mIpS);
	res |= (this->mIpS == other.mIpS) && (this->mPortS  > other.mPortS);
	res |= (this->mIpS == other.mIpS) && (this->mPortS  == other.mPortS) && (this->mIpD > other.mIpD);
	res |= (this->mIpS == other.mIpS) && (this->mPortS  == other.mPortS) && (this->mIpD == other.mIpD) && (this->mPortD  > other.mPortD);

	return res;
}
