/*
 *  NATSimAddrId.h
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

class CNATSimAddrId
{

public:

	CNATSimAddrId();
	CNATSimAddrId(CUtilString Ip, long Port);
	~CNATSimAddrId();
	
	CNATSimAddrId & operator = (const CNATSimAddrId & other);

	bool operator == (const CNATSimAddrId & other) const;
	bool operator  < (const CNATSimAddrId & other) const;
	bool operator  > (const CNATSimAddrId & other) const;

	CUtilString ToString();

private:

	CUtilString		mIp;
	long			mPort;
};
