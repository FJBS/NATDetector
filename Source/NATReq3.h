
/*
 *  NATReq3.h
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

class CNATReq3
{
public:
	CNATReq3(CUtilTrace * Trace);
	CNATReq3(CUtilTrace * Trace, int indexA, int indexB, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object );
	~CNATReq3();
	
	bool Test(CUtilString & fromClientA, CUtilString & fromClientB, CUtilString & toServer, int numMessages, CUtilString & report);
	
protected:

private:
	CUtilTrace		*mTrace;

	pthread_t		mThreadClientA;		
	static void*	staticThreadClientAProc(void* arg);
	void*			realThreadClientAProc(void* arg);

	CSTUNAttribute *mMappedAddressA;
	bool			mReceivedInA;

	pthread_t		mThreadClientB;		
	static void*	staticThreadClientBProc(void* arg);
	void*			realThreadClientBProc(void* arg);

	CSTUNAttribute *mMappedAddressB;
	bool			mReceivedInB;

	int						mIndexA;
	int						mIndexB;
	AdviseSndFunction		mAdviseSndFunction;
	AdviseRcvFunction		mAdviseRcvFunction;
	void*					mObject;

	CUtilString				mFromClientA;
	CUtilString				mFromClientB;
	CUtilString				mToServer;

};
