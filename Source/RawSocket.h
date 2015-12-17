/*
 *  RawSocket.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */
 
#pragma once

class CRawSocket
{
public:
	CRawSocket(CUtilTrace* Trace);
	~CRawSocket();
	
public:
	bool CreateReceiver();
	bool CreateSender();

	bool Bind(CUtilString & address, word Port);

	bool TerminateReceiver();
	bool TerminateSender();
	
	bool Send(CUtilByteArray & data, const char * saddr, int sport, const char * daddr, int dport);
	bool Receive(CUtilByteArray ** data, struct timeval timeout );
	
private:
	CUtilTrace*		mTrace;
	
	int				mPort;	

#ifndef WIN32
	unsigned long	mHandle;
#else
	SOCKET			mHandle;
#endif

	fd_set			mSet;
	
	bool Send(const byte * data, int dataLength, const char * saddr, int sport, const char* daddr, int dport) const;
	bool Receive(byte **rcv, dword *Length, struct timeval timeout);

	void TracePacketInfo(struct sockaddr_in packet_info) const;
	
private:
	word in_cksum_udp(int src, int dst, word *addr, int len);
	word in_cksum(word *addr, int len);
};


