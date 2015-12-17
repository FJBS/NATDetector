/*
 *  NATReq1.h
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

class CNATReq1
{
public:
	CNATReq1(CUtilTrace * Trace);
	CNATReq1(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object );

	~CNATReq1();
	
	bool Test(CUtilString & fromClient, CUtilString & toServer, CUtilString & report);

private:
	CUtilTrace		*mTrace;

	int						mIndex;
	AdviseSndFunction		mAdviseSndFunction;
	AdviseRcvFunction		mAdviseRcvFunction;
	void*					mObject;
};
