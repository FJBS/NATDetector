/*
 *  NATSimTrafficId.h
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

class CNATSimTrafficId
{

public:

	CNATSimTrafficId();
	CNATSimTrafficId(const CNATSimTrafficId & other);
	CNATSimTrafficId(CUtilString IpS, dword PortS, CUtilString IpD, dword PortD);
	~CNATSimTrafficId();
	
	CNATSimTrafficId & operator = (const CNATSimTrafficId & other);

	bool operator == (const CNATSimTrafficId & other) const;
	bool operator  < (const CNATSimTrafficId & other) const;
	bool operator  > (const CNATSimTrafficId & other) const;

	CUtilString GetIpS() { return mIpS; };
	dword		GetPortS() { return mPortS; };

	CUtilString GetIpD() { return mIpD; };
	dword		GetPortD() { return mPortD; };
	

private:

	CUtilString		mIpS;
	dword			mPortS;
	CUtilString		mIpD;
	dword			mPortD;
};
