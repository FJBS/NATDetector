/*
 *  STUNSever.h
 *  NATDetector
 *
 *  Created by System Administrator on 22/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

class CSTUNServer
{
public:
	CSTUNServer(CUtilString ConfigFile, CUtilTrace* Trace);
	CSTUNServer(CUtilString ConfigFile, CUtilTrace* Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object );
	~CSTUNServer();
	
public:
	bool Start();
	void Stop();
	
private:

	CArray<CUtilString>		mNATPublicIPs;
	
	CUtilString				mConfigFile;

	CUtilString				mDevice;

	CUtilTrace *			mTrace;
	struct in_addr			mServerAddressA;
	word					mServerAPort;
	struct in_addr			mServerAddressB;
	word					mServerBPort;
	
	pthread_t				mThread;		
	static void*			staticThreadProc(void* arg);
	void*					realThreadProc(void* arg);

	bool					mTerminate;
	
	void					readSTUNServerConfig();

	AdviseSndFunction		mAdviseSndFunction;
	AdviseRcvFunction		mAdviseRcvFunction;
	int						mIndex;
	void*					mObject;

	CUtilCriticalSection	*mCriticalSection;
};

