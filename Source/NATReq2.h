/*
 *  NATReq2.h
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

class CNATReq2
{
public:
	CNATReq2(CUtilTrace * Trace);
	CNATReq2(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object );
	~CNATReq2();
	
	bool Test(CUtilString & fromClient, CUtilString & toServer, word numMessages, CUtilString & report);
	
private:
	CUtilTrace				*mTrace;

	int						mIndex;
	AdviseSndFunction		mAdviseSndFunction;
	AdviseRcvFunction		mAdviseRcvFunction;
	void*					mObject;
};