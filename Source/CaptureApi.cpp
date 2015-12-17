/*
 *  CaptureApi.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "CaptureDefs.h"

#ifdef WIN32

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

CCaptureApi::CCaptureApi(CUtilTrace* Trace)
{
	this->mTrace = Trace;

#ifndef WIN32
	this->mThread = NULL;
#else
	this->mThread.p = NULL;
#endif
	mNetDevice = "";

	this->mRcvQueue = new CCaptureQueue(mTrace);

	mCapturing = false;

}
	
CCaptureApi::~CCaptureApi()
{
	Stop();
	delete this->mRcvQueue;
}

void* CCaptureApi::staticThreadProc(void* arg)
{
	CCaptureApi * This = (CCaptureApi*)arg;
	return This->realThreadProc(NULL);
}

void* CCaptureApi::realThreadProc(void* arg)
{
	bool ret = true;

	const  byte			*data=NULL;
	struct pcap_pkthdr	*hdr;

	struct ether_header *eptr;
	struct ip			*ipc;
	struct udphdr		*udph;
	byte				*payload;
	CaptureStruct		*packet;

	char				aux1[50], aux2[50];

	while ( !mTerminate )
	{
		int aux = pcap_next_ex(mCap_t, &hdr, &data);
		if ( aux == 1 )
		{
			if ( hdr->len < sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr) )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Captured a packet too sort.");
				continue;
			}

			eptr = (struct ether_header *)data;
			
			if ( ntohs(eptr->ether_type) == ETHERTYPE_IP )
			{
				ipc = (struct ip*)(data+sizeof(struct ether_header));
				
				udph = (struct udphdr *)(data+sizeof(struct ether_header)+ipc->ip_hl*4);							
				payload = (byte *)(data+sizeof(struct ether_header)+(ipc->ip_hl*4)+sizeof(struct udphdr));

				if ( hdr->len < sizeof(struct ether_header) + ntohs(ipc->ip_len) )
				{
					mTrace->Trace("**ERROR**", __FUNCTION__, "Captured a packet with ip len greater than bytes received %d %d.", hdr->len, sizeof(struct ether_header) + ntohs(ipc->ip_len));
					continue;
				}

				//mTrace->TraceBinary("**DEBUG**", __FUNCTION__, "IP  hdr.  %s", (byte*)ipc, ipc->ip_hl*4);
				//mTrace->TraceBinary("**DEBUG**", __FUNCTION__, "UDP hdr.  %s", (byte*)(ipc)+(ipc->ip_hl*4), 8);
				//mTrace->TraceBinary("**DEBUG**", __FUNCTION__, " %s", payload, ntohs(ipc->ip_len)-(ipc->ip_hl*4)-sizeof(struct udphdr));			
				//mTrace->TraceBinary("**DEBUG**", __FUNCTION__, "MAC Src.  %s", eptr->ether_shost, ETHER_ADDR_LEN);
				//mTrace->TraceBinary("**DEBUG**", __FUNCTION__, "MAC Dst.  %s", eptr->ether_dhost, ETHER_ADDR_LEN);

				strcpy(aux1, inet_ntoa(ipc->ip_src));
				strcpy(aux2, inet_ntoa(ipc->ip_dst));
				mTrace->Trace      ("**DEBUG**", __FUNCTION__, "");
				mTrace->TraceBinary("**MSGRC**", __FUNCTION__, "%s", (byte*)ipc, ntohs(ipc->ip_len));	
				mTrace->Trace      ("**INFO **", __FUNCTION__, "From %s:%d To %s:%d", aux1, ntohs(udph->uh_sport), aux2,ntohs(udph->uh_dport));

				CUtilByteArray byteArray(payload, ntohs(ipc->ip_len)-(ipc->ip_hl*4)-sizeof(struct udphdr));
					
				packet = new CaptureStruct;
				packet->Ip_src = ipc->ip_src;
				packet->Ip_dst = ipc->ip_dst;
				packet->Sport  = ntohs(udph->uh_sport);
				packet->Dport  = ntohs(udph->uh_dport);
				packet->Payload = byteArray;				
				mRcvQueue->Insert(*packet);
				delete packet;
			}
		}
		else if ( aux == 0 )
		{
			// mTrace->Trace("**PCAP:ERROR**: CCaptureApi::realThreadProc() packets are being read from a live capture, and the time-out timeout out expired.");					
		}
		else if ( aux == -1 )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "An error occurred while reading the packet->");					
		}
		else if ( aux == -2 )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Packets are being read from a ``savefile'', and there are no more packets to read from the savefile.");					
		}

	}
					
	return 0;
}

bool CCaptureApi::Start(CUtilString & addressSrc, CUtilString & addressDst, CUtilString & netDevice)
{
	bool				ret = true;
	struct in_addr		in_addrAux;


#ifndef WIN32
	if ( mThread != NULL )
#else
	if ( mThread.p != NULL )
#endif
	{
		mTrace->Trace("**PCAP:DEBUG**", "CCaptureApi", "Start", "Capture yet started.");
		ret = false;
	}
	else
	{
		in_addrAux.s_addr = inet_addr((const char *)addressSrc);
		mInAddressSrc.Add(in_addrAux);

		in_addrAux.s_addr = inet_addr((const char *)addressDst);
		mInAddressDst.Add(in_addrAux);

		mNetDevice = netDevice;
		
		if ( PrepareCapture() )
		{
			mTrace->Trace("**DEBUG**", __FUNCTION__, "Starting capture on device %s. Capturing packes from %s to %s.", 
													(const char *)mNetDevice, 
													addressSrc.length() == 0 ? "every" : (const char *)addressSrc, 
													addressDst.length() == 0 ? "every" : (const char *)addressDst);		
			mTerminate = false;
			if ( pthread_create(&mThread, 0, staticThreadProc, this) != 0 )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for device %s. Packes from %s to %s will not be captured.", 
														(const char *)mNetDevice, 
														addressSrc.length() == 0 ? "all" : (const char *)addressSrc, 
														addressDst.length() == 0 ? "all" : (const char *)addressDst);		
				ret = false;
			}
		}
	}
	
	return ret;
}

bool CCaptureApi::Start(CArray<CUtilString> & addressSrc, CArray<CUtilString> & addressDst, CUtilString & netDevice)
{
	bool			ret = true;
	CUtilString		aux1, aux2;
	struct in_addr	in_addrAux;
	dword			i;

#ifndef WIN32
	if ( mThread != NULL )
#else
	if ( mThread.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Capture yet started.");
		ret = false;
	}
	else
	{
		for ( i = 0; i < addressSrc.GetElements(); i++ )
		{
			in_addrAux.s_addr = inet_addr((const char *)addressSrc[i]);
			mInAddressSrc.Add(in_addrAux);
			aux1 += addressSrc[i] + " ";
		}
			
		for ( i = 0; i < addressDst.GetElements(); i++ )
		{
			in_addrAux.s_addr = inet_addr((const char *)addressDst[i]);
			mInAddressDst.Add(in_addrAux);
			aux2 += addressDst[i] + " ";
		}
		
		mNetDevice = netDevice;
		
		if ( PrepareCapture() )
		{
			mTrace->Trace("**DEBUG**", __FUNCTION__, "Starting capture on device %s. Capturing packes from %s to %s.", 
													(const char *)mNetDevice, 
													aux1.length() == 0 ? "all" : (const char *)aux1, 
													aux2.length() == 0 ? "all" : (const char *)aux2);		
			mTerminate = false;
			if ( pthread_create(&mThread, 0, staticThreadProc, this) != 0 )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for device %s. Packes from %s to %s will not be captured.", 
														(const char *)mNetDevice, 
														aux1.length() == 0 ? "all" : (const char *)aux1, 
														aux2.length() == 0 ? "all" : (const char *)aux2);		
				ret = false;
			}
		}
	}
	
	return ret;
}


bool CCaptureApi::Start(CArray<CUtilString> & addressSrc, CArray<dword> & PortsSrc, CArray<CUtilString> & addressDst, CArray<dword> & PortsDst, CUtilString & netDevice)
{
	bool			ret = true;
	CUtilString		aux1, aux2;
	struct in_addr	in_addrAux;
	dword			i;

#ifndef WIN32
	if ( mThread != NULL )
#else
	if ( mThread.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Capture yet started.");
		ret = false;
	}
	else
	{
		for ( i = 0; i < addressSrc.GetElements(); i++ )
		{
			in_addrAux.s_addr = inet_addr((const char *)addressSrc[i]);
			mInAddressSrc.Add(in_addrAux);
			aux1 += addressSrc[i] + " ";
		}
			
		for ( i = 0; i < addressDst.GetElements(); i++ )
		{
			in_addrAux.s_addr = inet_addr((const char *)addressDst[i]);
			mInAddressDst.Add(in_addrAux);
			aux2 += addressDst[i] + " ";
		}
		
		mPortsSrc = PortsSrc;
		mPortsDst = PortsDst;

		mNetDevice = netDevice;
		
		if ( PrepareCapture() )
		{
			mTrace->Trace("**DEBUG**", __FUNCTION__, "Starting capture on device %s. Capturing packes from %s to %s.", 
													(const char *)mNetDevice, 
													aux1.length() == 0 ? "all" : (const char *)aux1, 
													aux2.length() == 0 ? "all" : (const char *)aux2);	
			mTerminate = false;
			if ( pthread_create(&mThread, 0, staticThreadProc, this) != 0 )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for device %s. Packes from %s to %s will not be captured.", 
														(const char *)mNetDevice,
														aux1.length() == 0 ? "all" : (const char *)aux1, 
														aux2.length() == 0 ? "all" : (const char *)aux2);	
				ret = false;
			}
		}
	}
	
	return ret;
}


void CCaptureApi::Stop()
{
	CUtilString		aux1, aux2;
	dword				i;

#ifndef WIN32
	if ( mThread != NULL )
#else
	if ( mThread.p != NULL )
#endif
	{
		for ( i = 0; i < mInAddressSrc.GetElements(); i++ )
			aux1 += (CUtilString)(inet_ntoa(mInAddressSrc[i])) + " ";
			
		for ( i = 0; i < mInAddressDst.GetElements(); i++ )
			aux2 += (CUtilString)(inet_ntoa(mInAddressDst[i])) + " ";

		mTrace->Trace("**DEBUG**", __FUNCTION__, "Stoping capture from %s to %s.",
												aux1.length() == 0 ? "all" : (const char *)aux1, 
												aux2.length() == 0 ? "all" : (const char *)aux2);		

		mTerminate = true;
		pthread_join(mThread, 0);
		
#ifndef WIN32
		mThread = NULL;
#else
		mThread.p = NULL;
#endif
		mInAddressSrc.RemoveAll();
		mInAddressDst.RemoveAll();
		mNetDevice = "";

		if ( mCapturing )
		{
			pcap_freecode(&mFp);

			if ( mCap_t != NULL )
				pcap_close(mCap_t);
		}

		mCap_t = NULL;

		mCapturing = false;
	}
}

bool CCaptureApi::PrepareCapture()
{
	bool				ret	= true;
	char				errbuf[PCAP_ERRBUF_SIZE];
	
	bpf_u_int32			netp;
	bpf_u_int32			maskp;
	struct in_addr		addr;
	char				*net;
	char				*mask;
	CUtilString			filter;		

	char				buffer[10];

	dword				i=0;

	ret = ( pcap_lookupnet((const char *)mNetDevice, &netp, &maskp, errbuf) != -1 );
	if ( !ret )
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, "%s", errbuf);
	}
	else
	{
		addr.s_addr = netp;
		net = inet_ntoa(addr);
		if ( net == NULL )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed inet_ntoa.");
			ret = false;
		}
		else
			mTrace->Trace("**DEBUG**", __FUNCTION__, "Net: %s.", net);

		addr.s_addr = maskp;
		mask = inet_ntoa(addr);
		if ( mask == NULL )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed inet_ntoa.");
			ret = false;
		}
		else
			mTrace->Trace("**DEBUG**", __FUNCTION__, "Mask: %s.", mask);
	}		

	if ( ret )
	{
		mCap_t = pcap_open_live(mNetDevice, BUFSIZ, 1, 100, errbuf);
		if ( mCap_t == NULL )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pcap_open_live %s.", errbuf);
			ret = false;			
		}	
		
		if ( ret )
		{			
			filter  = "udp";
			if ( mInAddressSrc.GetElements() > 0 )
			{
				filter += " and ( ";	
				for ( i = 0; i < mInAddressSrc.GetElements(); i++ )
				{
					if ( i > 0 )
						filter +=" or ";
					filter += (CUtilString)"src host " + inet_ntoa(mInAddressSrc[i]);
				}	
				filter += " ) ";
			}
			if ( mInAddressDst.GetElements() > 0 )
			{
				filter += " and ( ";
				for ( i = 0; i < mInAddressDst.GetElements(); i++ )
				{
					if ( i > 0 )
						filter +=" or ";
					filter += (CUtilString)"dst host " + inet_ntoa(mInAddressDst[i]);
				}	
				filter += " ) ";
			}

			if ( mPortsSrc.GetElements() > 0 )
			{
				filter += " and ( ";
				for ( i = 0; i < mPortsSrc.GetElements(); i++ )
				{
					sprintf(buffer, "%d", mPortsSrc[i]);
					if ( i > 0 )
						filter +=" or ";
					filter += (CUtilString)"src port " + buffer;
				}	
				filter += " ) ";
			}
			if ( mPortsDst.GetElements() > 0 )
			{
				filter += " and ( ";
				for ( i = 0; i < mPortsDst.GetElements(); i++ )
				{
					sprintf(buffer, "%d", mPortsDst[i]);
					if ( i > 0 )
						filter +=" or ";
					filter += (CUtilString)"dst port " + buffer;
				}	
				filter += " )";
			}

			if ( strcmp((const char *)filter, "udp") == 0 )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Bad Filter.");
				ret = false;			
			}

			
			try
			{
				ret = ( pcap_compile(mCap_t, &mFp, "udp", 0, netp) != -1 );
			}
			catch ( ... )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Exception caught. Failed pcap_compile().");
			}

			ret = ( pcap_compile(mCap_t, &mFp, (char *)(const char *)(filter), 0, netp) != -1 );
			if ( !ret )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pcap_compile().");
				ret = false;			
			}
		}
		
		if ( ret )
		{
			ret = (pcap_setfilter(mCap_t, &mFp) != -1 );
			if ( !ret )
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pcap_setfilter().");
				pcap_freecode(&mFp);
				ret = false;			
			}
			else
			{
				mTrace->Trace("**DEBUG**", __FUNCTION__, "Filter: %s.", (const char *)(filter));
			}			
		}
	}

	mCapturing = ret;

	return ret;
}

bool CCaptureApi::GetMsg(CaptureStruct & msg, dword timeout/*cents*/)
{
	// TODO: implements this function with precision of miliseconds
	
	bool			ret = false;
	
#ifndef WIN32

	struct tms		t_start, t_now;
	clock_t			start, now;	
	long			tps = sysconf(_SC_CLK_TCK);

	start = times(&t_start);
	do
	{
		ret = mRcvQueue->Get(msg);

		if ( ret )
			break;
			
		now =  times(&t_now);			
	}
	while ((now - start)*tps/100 < timeout);
	

#else

	dword		start, now;

	start = GetTickCount();

	timeout *= 10;
	do
	{
		ret = mRcvQueue->Get(msg);
		if ( ret )
			break;
			
		now = GetTickCount();

		Sleep(1);
	}
	while ((now - start) < timeout);

	now = GetTickCount();

#endif

	return ret;	
}

bool CCaptureApi::GetDevices(CArray<CUtilString> *devices, int * numDevs)
{
	bool		ret = true;

	char		errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t	*alldevs, *d;
	int			i=0;

	CUtilString	aux;

	*numDevs = 0;

	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		ret = false;
	}
	
	if ( ret )
	{
		for (d=alldevs; d; d=d->next)
		{
			*numDevs += 1;
		}

		for (d=alldevs, i=0; d; d=d->next, i++)
		{			
			if (d->description)
			{
				aux = d->name;
				(*devices).Add(aux);
			}
			else
			{
				aux = "Unknown name";
				(*devices).Add(aux);
			}
		}
	}

	return ret;
}
