/*
 *  STUNAddress.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez on 27/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

class CSTUNAddress 
{

public:
	CSTUNAddress();
	CSTUNAddress (const CSTUNAddress & other);
	CSTUNAddress(CUtilString address, word port);
	CSTUNAddress(byte* address, byte addressLength, word port);
	CSTUNAddress(struct in_addr addr, u_short port);
	~CSTUNAddress();

	CUtilByteArray & getAddressBytes();
	word  getPort();
	bool  equals(const CSTUNAddress & other);
	
	CUtilString ToString();
	
	CSTUNAddress & operator = (const CSTUNAddress & other);
	bool operator == (const CSTUNAddress & other);

private:
	byte*		mAddress;
	byte		mAddressLength;
	word		mPort;
};
