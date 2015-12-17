/*
 *  RawDefs.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

#ifndef WIN32

	#include "UtilDefs.h"
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/ip.h>
	#include <netinet/udp.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <net/if.h>

#else

	#include "UtilDefs.h"

	#include <Winsock2.h>
	#include "ws2tcpip.h"  //IP_HDRINCL is here

	#define ETHER_ADDR_LEN	6
	#define	ETHERTYPE_IP	0x0800

	struct ether_header
	{
		byte ether_dhost[ETHER_ADDR_LEN];
		byte ether_shost[ETHER_ADDR_LEN];
		word ether_type;
	};

	struct udphdr
	{
		word uh_sport;
		word uh_dport;
		word uh_ulen;
		word uh_sum;
	};

	struct ip
	{
		byte	ip_hl:4;
		byte	ip_v:4;

		byte	ip_tos;
		word	ip_len;
		word	ip_id;
		word	ip_off;

		byte	ip_ttl;
		byte	ip_p;
		word	ip_sum;

		struct	in_addr ip_src;
		struct	in_addr ip_dst;
	};

#endif

const int ETHERNET_LENGTH = 14;
const int MIN_IP_HEADER_LENGTH = 20;

#include "RawSocket.h"
