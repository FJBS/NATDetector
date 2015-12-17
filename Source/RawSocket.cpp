/*
 *  RawSocket.cpp
 *  NATDetector
 *
 *  Created by Francisco Jos√© Bl√°zquez S√°nchez on 02/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "RawSocketDefs.h"

extern int errno;

struct psd_udp 
{
	struct in_addr src;
	struct in_addr dst;
	unsigned char pad;
	unsigned char proto;
	unsigned short udp_len;
	struct udphdr udp;
};

CRawSocket::CRawSocket(CUtilTrace* Trace)
{
	this->mTrace = Trace;
	this->mPort = -1;
	mHandle = -1;

}
	
CRawSocket::~CRawSocket()
{
}

bool CRawSocket::CreateSender()
{
	bool		ret = true;
	const int	on = 1;

	mHandle = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if ( mHandle < 0 )
	{
		perror(NULL);
		mTrace->Trace("**ERROR**", __FUNCTION__, "Failed socket(). (errno=%d : %s)", errno, strerror(errno));
		ret = false;
	}
	else
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "HANDLE 0x%06X", mHandle);		

#ifndef WIN32
		if (setsockopt (mHandle, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) != 0)
		{
			perror(NULL);
			mTrace->Trace("**ERROR**",  __FUNCTION__, "Failed setsockopt(). (errno=%d : %s)", errno, strerror(errno));
#else
		if (setsockopt (mHandle, IPPROTO_IP, IP_HDRINCL, (char *)&on, sizeof (on)) != 0)
		{
			int err = WSAGetLastError();
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed setsockopt(). (errno=%d )", err);
#endif
			ret = false;
		}
	}
	 
	 return ret;
}

bool CRawSocket::CreateReceiver()
{
	bool	ret = true;

	mHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ( mHandle < 0 )
	{
		perror(NULL);
		mTrace->Trace("**ERROR**", __FUNCTION__, "Failed socket(). (errno=%d : %s)", errno, strerror(errno));
		ret = false;
	}
	else
	{
		FD_ZERO(&mSet);
		FD_SET(mHandle, &mSet);
		mTrace->Trace("**DEBUG**", __FUNCTION__, "HANDLE 0x%06X", mHandle);		
	}

	 return ret;
}

bool CRawSocket::TerminateReceiver()
{
	bool ret = true;
	
	if ( mHandle > 0 )
	{
#ifndef WIN32
		if ( close(mHandle) != 0 )
#else
		if ( closesocket(mHandle) != 0 )
#endif
		{
			perror(NULL);
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed close(0x%06X). (errno=%d : %s)", mHandle, errno, strerror(errno));
			ret = false;
		}
		else
		{
			mTrace->Trace("**DEBUG**", __FUNCTION__, "HANDLE 0x%06X", mHandle);
			mHandle = NULL;
		}
	}
	
	return ret;
}

bool CRawSocket::TerminateSender()
{
	bool ret = true;
	
	if ( mHandle > 0 )
	{
#ifndef WIN32
		if ( close(mHandle) != 0 )
#else
		if ( closesocket(mHandle) != 0 )
#endif
		{
			perror(NULL);
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed close(0x%06X). (errno=%d : %s)", mHandle, errno, strerror(errno));
			ret = false;
		}
		else
		{
			mTrace->Trace("**DEBUG**", __FUNCTION__, "HANDLE 0x%06X", mHandle);
			mHandle = NULL;
		}		
	}
	
	return ret;
}

bool CRawSocket::Bind(CUtilString & address, word Port)
{
	bool		ret = true;
	
	struct		sockaddr_in sin;

	if ( mHandle > 0 )
	{
		memset(& sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(Port);
		sin.sin_addr.s_addr = inet_addr((const char *)address);

		if ( bind(mHandle, (struct sockaddr *)& sin, sizeof(sin)) < 0 )
		{
			perror(NULL);
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed bind(). (errno=%d : %s)", errno, strerror(errno));
			ret = false;
			mPort = -1;
		}				
		else
		{
			mPort = Port;
			mTrace->Trace("**DEBUG**", __FUNCTION__, "HANDLE 0x%06X IP %s Port %d", mHandle, (const char *)address, Port);	
		}		
	}
	else
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, "Failed. Call CRawSocket::Create() before.");
		ret = false;
		mPort = -1;
	}
	
	return ret;
}

//---------------------------------------------------------------------------
// Envia un paquete UDP hacia el direccion y puerto d_addr-d_port, incluyendo
// en la cabecera del paquete IP la direcci√≥n y puerto origen s_addr-s_port.
//
// PRECON: El socket se debe de haber creado previamente con Create.
//
// RESULT: True si envia, False en otro caso.
//---------------------------------------------------------------------------
bool CRawSocket::Send(const byte *data, int dataLength, const char * saddr, int sport, const char * daddr, int dport) const
{
	bool				ret = true; 
	byte				*datagram;
	struct ip			iph;
	struct udphdr		udph;
	struct sockaddr_in	sin;
	
	mTrace->Trace("**DEBUG**", __FUNCTION__, "HANDLE 0x%06X From %s:%d to %s:%d", mHandle, saddr, sport, daddr, dport);

	/*
	p_hl:	the ip header length in 32bit octets. this means a value of 5 for the hl means 20 bytes (5 * 4). 
			values other than 5 only need to be set it the ip header contains options (mostly used for routing)
	ip_v:	the ip version is always 4 (maybe I'll write a IPv6 tutorial later;)
	ip_tos: type of service controls the priority of the packet. 0x00 is normal. 
			the first 3 bits stand for routing priority, 
			the next 4 bits for the type of service (delay, throughput, reliability and cost).
	ip_len: total length must contain the total length of the ip datagram. 
			this includes ip header, icmp or tcp or udp header and payload size in bytes.
	ip_id:	the id sequence number is mainly used for reassembly of fragmented IP datagrams. 
			when sending single datagrams, each can have an arbitrary ID.
	ip_off: the fragment offset is used for reassembly of fragmented datagrams. 
			the first 3 bits are the fragment flags, 
				the first one always 0, 
				the second the do-not-fragment bit (set by ip_off |= 0x4000) and 
				the third the more-flag or more-fragments-following bit (ip_off |= 0x2000). 
			the following 13 bits is the fragment offset, containing the number of 8-byte big packets already sent.
	ip_ttl: time to live is the amount of hops (routers to pass) before the packet is discarded, and an icmp error message is returned. the maximum is 255.
	ip_p:	the transport layer protocol. can be tcp (6), udp(17), icmp(1), or whatever protocol follows the ip header. 
			look in /etc/protocols for more.
	ip_sum: the datagram checksum for the whole ip datagram. all time anything in the datagram changes, 
			it needs to be recalculated, or the packet will be discarded by the next router. 
	ip_src 
		and 
	ip_dst: source and destination IP address, converted to long format, e.g. by inet_addr(). both can be chosen arbitrarily.
	*/

	try 
	{

		datagram = new byte[dataLength + sizeof(struct ip) + sizeof(struct udphdr)];
		
		if ( mHandle > 0 )
		{
			memset(&iph, 0, sizeof(struct ip));
			memset(&udph, 0, sizeof(struct udphdr));

			iph.ip_v = 0x04;
			iph.ip_hl = 0x05;
			iph.ip_tos = 0x00;
			iph.ip_len = sizeof (struct ip) + sizeof (struct udphdr) + dataLength;
			iph.ip_id =  4321;	
			iph.ip_off = 0x00;
			iph.ip_ttl = 0x64;
			iph.ip_p = IPPROTO_UDP;
			iph.ip_sum = 0x0;
			iph.ip_src.s_addr = inet_addr (saddr);
			iph.ip_dst.s_addr = inet_addr (daddr);
			//iph.ip_sum = in_cksum((word*)&iph, sizeof(struct ip));
			memcpy(datagram, &iph, sizeof(struct ip));

			udph.uh_sport = htons(sport);
			udph.uh_dport = htons(dport);
			udph.uh_ulen = htons((word)(sizeof(struct udphdr) + dataLength));
			udph.uh_sum = 0;
			// udph.uh_sum = in_cksum_udp(iph.ip_src.s_addr, iph.ip_dst.s_addr, (word *)&udph, sizeof(struct udphdr));
			memcpy(datagram +  sizeof(struct ip), &udph, sizeof (struct udphdr));
							
			if ( ret == true )
			{
				memcpy(datagram +  sizeof (struct ip) + sizeof (struct udphdr), data, dataLength);
							
				memset(&sin, 0, sizeof(sin));
				sin.sin_family = AF_INET;
				sin.sin_port = htons (dport);
				sin.sin_addr.s_addr = inet_addr(daddr);

#ifndef WIN32
				if ( sendto(mHandle, datagram, iph.ip_len, 0, (struct sockaddr *) &sin, sizeof (struct sockaddr)) < 0 )	
#else
				if ( sendto(mHandle, (const char *)datagram, iph.ip_len, 0, (struct sockaddr *) &sin, sizeof (struct sockaddr)) < 0 )	
#endif
				{
#ifndef WIN32
					mTrace->Trace("**ERROR**", __FUNCTION__, "Failed sendto(). (errno=%d : %s)", errno, strerror(errno));
#else
					int err = WSAGetLastError();
					mTrace->Trace("**ERROR**", __FUNCTION__, "Failed sendto(). (errno=%d )", err);
#endif
					ret = false;
				}
				else
				{
					mTrace->TraceBinary("**MSGSN**", __FUNCTION__, "%s", datagram, sizeof (struct ip) + sizeof (struct udphdr) + dataLength);
				}		
			}	
		}
		else
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed. Call CRawSocket::Create() before.");
			ret = false;
		}
		
		delete datagram;
	}
	catch ( ... )
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, "Exception caught.");
		ret = false;
	}
	
	return ret;
}


bool CRawSocket::Receive(byte **rcv, dword *Length, struct timeval timeout ) 
{
	bool				ret = true;
	
	int					len = 0, aux = 0;
	byte				buffer[2048];		// TODO: Max. message length
	struct sockaddr_in	packet_info;
#ifndef WIN32
	socklen_t			packet_info_size = sizeof(packet_info);
#else
	int					packet_info_size = sizeof(packet_info);
#endif
	*rcv = NULL;
	*Length = 0;
	
	try 
	{
		if ( mHandle > 0 )
		{		
			mTrace->Trace("**DEBUG**", __FUNCTION__, "Waiting HANDLE 0x%06X Port %d", mHandle, mPort);
			
#ifndef WIN32

			aux = select(mHandle+1, (fd_set*)(&mSet), 0, 0, &timeout);

#else

			mSet.fd_count = 1;  
			mSet.fd_array[0] = mHandle; 
			aux = select(0, &mSet,NULL,NULL,&timeout);

#endif

			if ( aux > 0 )
			{
#ifndef WIN32
				if((len = recvfrom(mHandle, buffer, sizeof(buffer), 0, (struct sockaddr*)&packet_info, &packet_info_size)) == -1)
#else
				if((len = recvfrom(mHandle, (char *)buffer, sizeof(buffer), 0, (struct sockaddr*)&packet_info, &packet_info_size)) == -1)
#endif
				{
					mTrace->Trace("**ERROR**", __FUNCTION__, "Failed recvfrom(). (errno=%d : %s)", errno, strerror(errno));
					ret = false;
				}
				else
				{
					TracePacketInfo(packet_info);
					mTrace->TraceBinary("**MSGRV**", __FUNCTION__, "%s", buffer, len);
					*rcv = new byte[len];
					memcpy(*rcv, buffer, len);
					*Length = len;
				}
			}
			else
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Failed select() possible timeout receiving. (errno=%d : %s)", errno, strerror(errno));
				ret = false;			
			}
		}
		else
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed. Call CRawSocket::Create() before.");
			ret = false;
		}

#ifndef WIN32
		FD_ZERO(&mSet);
		FD_SET(mHandle, &mSet);
#endif
	}
	catch ( ... )
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, "Exception caught.");
		ret = false;
	}

	return ret;
}


bool CRawSocket::Send(CUtilByteArray & data, const char * saddr, int sport, const char * daddr, int dport)
{
	bool ret = true;
	
	try
	{
		ret = Send((const byte *)data, data.length(), saddr, sport, daddr, dport);
	}
	catch ( ... )
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, "Exception caught.");
		ret = false;
	}
	
	return ret;
}


bool CRawSocket::Receive(CUtilByteArray ** data, struct timeval timeout)
{
	bool	ret = true;
	byte	* dataAux=NULL;
	dword	length = 0;
	
	*data = NULL;
	
	try
	{
		ret = Receive(&dataAux, &length, timeout);
		
		if ( ret )
			*data = new CUtilByteArray(dataAux, length);

		delete dataAux;
	}
	catch ( ... )
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, "Exception caught.");
		ret = false;
	}
	
	return ret;
}

void CRawSocket::TracePacketInfo(struct sockaddr_in packet_info) const
{
	int  Port = 0; 
	char Address[100];
	
	Port = ntohs(packet_info.sin_port);
	strcpy(Address, inet_ntoa(packet_info.sin_addr));
	
	mTrace->Trace("**INFO **", __FUNCTION__, "Packet from %s.%d", Address, Port);	
}

//---------------------------------------------------------------------------
// Checksum
//---------------------------------------------------------------------------
word CRawSocket::in_cksum(word *addr, int len)
{
	int nleft = len;
	int sum = 0;
	word *w = addr;
	word answer = 0;

	while (nleft > 1) {
		sum += *w;
		w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(byte *) (&answer) = *(byte *) w;
		sum += answer;
	}
	
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	
	return (answer);
}

word CRawSocket::in_cksum_udp(int src, int dst, word *addr, int len)
{
	struct psd_udp buf;

	memset(&buf, 0, sizeof(buf));
	buf.src.s_addr = src;
	buf.dst.s_addr = dst;
	buf.pad = 0;
	buf.proto = IPPROTO_UDP;
	buf.udp_len = htons(len);
	memcpy(&(buf.udp), addr, len);
	
	return in_cksum((word *)&buf, 12 + len);
}
//---------------------------------------------------------------------------
