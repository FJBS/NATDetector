/*
 *  STUNSender.h
 *  NATDetector
 *
 *  Created by System Administrator on 09/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

class CSTUNClientTransaction
{
public:
	CSTUNClientTransaction(CUtilTrace* Trace);
	~CSTUNClientTransaction();
	
public:
	bool Create(CUtilString	& sourceAddress, word sourcePort, CUtilString & dev, CUtilConfigFile & Config);
	bool Terminate();

	bool SendAndReceive(CUtilByteArray & dataSend, CUtilByteArray ** dataRecv, const char* sourceAddr, word sourcePort, const char* destAddr, word destPort);
	bool Receive(CUtilByteArray ** dataRecv);

private:
	CUtilTrace *	mTrace;
	
	CRawSocket *	mRawSocketReceiver;
	CRawSocket *	mRawSocketSender;

	CCaptureApi	*	mCaptureReceiver;

};

