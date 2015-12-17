/*
 *  NATReq6.h
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

class CNATReq6
{
public:
	CNATReq6(CUtilTrace * Trace);
	CNATReq6(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object );
	~CNATReq6();
	
	bool Test(CUtilString & fromClient, CUtilString & toServer, unsigned int milisecs, CUtilString & report);
	
private:
	CUtilTrace		*mTrace;

	pthread_t		mThreadClientA;		
	static void*	staticThreadClientAProc(void* arg);
	void*			realThreadClientAProc(void* arg);

	pthread_t		mThreadClientB;		
	static void*	staticThreadClientBProc(void* arg);
	void*			realThreadClientBProc(void* arg);

	CSTUNClientTransaction	* mSTUNTransactionS;
	CSTUNClientTransaction	* mSTUNTransactionP;

	CUtilByteArray			* mMsgToSendS;

	CUtilString				* mSourceAddress;
	word					* mSourcePort;

	CUtilString				* mAddressSTUNServer;
	word					* mPortSTUNServer;

	bool					mReceivedInA;
	bool					mReceivedInB;

	int						mIndex;
	AdviseSndFunction		mAdviseSndFunction;
	AdviseRcvFunction		mAdviseRcvFunction;
	void*					mObject;
};
