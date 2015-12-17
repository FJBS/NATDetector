/*
 *  PCAPDefs.h
 *  NATDetector
 *
 *  Created by System Administrator on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
 
#pragma once

#include "UtilDefs.h"

#ifndef WIN32
	#include <sys/times.h>
	#include <sys/socket.h>
	#include <net/ethernet.h>
	#include <netinet/in.h>
	#include <netinet/ip.h>
	#include <netinet/udp.h>
	#include <arpa/inet.h>
#endif

#include <pcap.h>

struct CaptureStruct
{
	struct in_addr Ip_src;
	word	Sport;
		
	struct in_addr Ip_dst;
	word	Dport;
	
	CUtilByteArray Payload;
};

#include "CaptureQueue.h"
#include "CaptureApi.h"
