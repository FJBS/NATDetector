/*
 *  CaptureApi.h
 *  NATDetector
 *
 *  Created by System Administrator on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

 
#pragma once

class CCaptureApi
{
public:
	CCaptureApi(CUtilTrace* Trace);
	~CCaptureApi();
	
	static bool GetDevices(CArray<CUtilString> *devices, int * numDevs);

	bool Start(CUtilString & addressFrom, CUtilString & addressTo, CUtilString & netDevice);
	bool Start(CArray<CUtilString> & addressFrom, CArray<CUtilString> & addressTo, CUtilString & netDevice);
	bool Start(CArray<CUtilString> & addressSrc, CArray<dword> & PortsSrc, CArray<CUtilString> & addressDst, CArray<dword> & PortsDst, CUtilString & netDevice);

	void Stop();
	
	bool GetMsg(CaptureStruct & msg, dword timeout/*cents*/);
	
private:

	CUtilTrace				*mTrace;
	CCaptureQueue			*mRcvQueue;

	pcap_t					*mCap_t;
	struct bpf_program		mFp;

	CUtilString				mNetDevice;
	CArray<struct in_addr>	mInAddressSrc;
	CArray<dword>			mPortsSrc;
	CArray<struct in_addr>	mInAddressDst;
	CArray<dword>			mPortsDst;
	
	bool					mTerminate;
	pthread_t				mThread;	
	
	static void*			staticThreadProc(void* arg);
	void*					realThreadProc(void* arg);

	bool					PrepareCapture();
	bool					mCapturing;

};
