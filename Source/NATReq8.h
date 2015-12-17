/*
 *  NATReq8.h
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

class CNATReq8
{
public:
	CNATReq8(CUtilTrace * Trace);
	CNATReq8(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object );
	~CNATReq8();
	
	bool Test(CUtilString & fromClient, CUtilString & toServer, CUtilString & report);
	
protected:

private:
	CUtilTrace		*mTrace;

	int						mIndex;
	AdviseSndFunction		mAdviseSndFunction;
	AdviseRcvFunction		mAdviseRcvFunction;
	void*					mObject;
};
