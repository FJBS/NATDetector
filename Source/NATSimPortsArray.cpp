/*
 *  NATSimPortsArray.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 26/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATSimDefs.h"

CNATSimPortsArray::CNATSimPortsArray():CIdTbl<PortDataStruct,CUtilString>()
{
}

CNATSimPortsArray::~CNATSimPortsArray()
{
}

bool CNATSimPortsArray::InsertIP(CUtilString IP)
{
	bool	ret = true;
	dword	Idx = 0;
	
	PortDataStruct PortData;
	PortData.IPId = IP;
	memset(PortData.PortsAvailability, 0, sizeof(PortData.PortsAvailability));
	
	this->m_CriticalSection.Enter();

	if ( !this->FindItem(IP, &Idx) )
		ret = this->Insert(PortData);
	
	this->m_CriticalSection.Leave();

	return ret;
}

bool CNATSimPortsArray::RemoveIP(CUtilString IP)
{
	bool	ret = true;
	dword	Idx;
		
	this->m_CriticalSection.Enter();
	
	if ( this->FindItem(IP, &Idx ) )
	{
		if ( GetFirstReservedPort(IP) > 0 )
			ret = false;
		else		
			ret = this->Remove(Idx);
	}
		
	this->m_CriticalSection.Leave();

	return ret;
}

long CNATSimPortsArray::GetFirstReservedPort(CUtilString IP)
{
	long	ret = MAX_NUM_PORT + 1;
	dword	Idx;
		
	this->m_CriticalSection.Enter();
	
	if ( this->FindItem(IP, &Idx ) )
	{
		PortDataStruct & PortData = *(this->m_Tbl[Idx]);
		for ( long i = 0; i < sizeof(PortData.PortsAvailability); i++ )
		{
			if ( PortData.PortsAvailability[i] != 0x00 )
			{
				ret = ((PortData.PortsAvailability[i]&0x80)!=0x00)?8*i+7:ret;
				ret = ((PortData.PortsAvailability[i]&0x40)!=0x00)?8*i+6:ret;
				ret = ((PortData.PortsAvailability[i]&0x20)!=0x00)?8*i+5:ret;
				ret = ((PortData.PortsAvailability[i]&0x10)!=0x00)?8*i+4:ret;
				ret = ((PortData.PortsAvailability[i]&0x08)!=0x00)?8*i+3:ret;
				ret = ((PortData.PortsAvailability[i]&0x04)!=0x00)?8*i+2:ret;
				ret = ((PortData.PortsAvailability[i]&0x02)!=0x00)?8*i+1:ret;
				ret = ((PortData.PortsAvailability[i]&0x01)!=0x00)?8*i+0:ret;
				break;
			}
		}
	}	
	
	this->m_CriticalSection.Leave();
	
	return ret;
}

long CNATSimPortsArray::ReserveFirstAvailablePort(CUtilString IP)
{
	long	ret = MAX_NUM_PORT + 1;
	dword	Idx;
		
	this->m_CriticalSection.Enter();
	
	if ( this->FindItem(IP, &Idx ) )
	{
		PortDataStruct & PortData = *(this->m_Tbl[Idx]);
		for ( long i = 0; i < sizeof(PortData.PortsAvailability); i++ )
		{
			if ( PortData.PortsAvailability[i] != 0xFF )
			{
				ret = ((PortData.PortsAvailability[i]&0x01)==0x00)?8*i+0:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x01;
				if ( ret < MAX_NUM_PORT )
					break;
				
				ret = ((PortData.PortsAvailability[i]&0x02)==0x00)?8*i+1:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x02;
				if ( ret < MAX_NUM_PORT )
					break;

				ret = ((PortData.PortsAvailability[i]&0x04)==0x00)?8*i+2:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x04;
				if ( ret < MAX_NUM_PORT )
					break;

				ret = ((PortData.PortsAvailability[i]&0x08)==0x00)?8*i+3:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x08;
				if ( ret < MAX_NUM_PORT )
					break;

				ret = ((PortData.PortsAvailability[i]&0x10)==0x00)?8*i+4:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x10;
				if ( ret < MAX_NUM_PORT )
					break;

				ret = ((PortData.PortsAvailability[i]&0x20)==0x00)?8*i+5:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x20;
				if ( ret < MAX_NUM_PORT )
					break;


				ret = ((PortData.PortsAvailability[i]&0x40)==0x00)?8*i+6:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x40;
				if ( ret < MAX_NUM_PORT )
					break;

				ret = ((PortData.PortsAvailability[i]&0x80)==0x00)?8*i+7:ret;
				this->m_Tbl[Idx]->PortsAvailability[i] |= 0x80;
				if ( ret < MAX_NUM_PORT )
					break;
			}
		}
	}	
	
	this->m_CriticalSection.Leave();
	
	return ret;
}

bool CNATSimPortsArray::FreePort(CUtilString IP, long Port)
{
	bool	ret = true;
	dword	Idx;
		
	this->m_CriticalSection.Enter();
	
	if ( this->FindItem(IP, &Idx ) )
	{
		long byt = Port/8;
		byte bit = (byte)(Port%8);
		byte mask = (bit==0?0xFE:(bit==1?0xFD:(bit==2?0xFB:(bit==3?0xF7:(bit==4?0xEF:(bit==5?0xDF:(bit==6?0xBF:(bit==7?0x7F:0xFF))))))));
		this->m_Tbl[Idx]->PortsAvailability[byt] &= mask;
	}	
	
	this->m_CriticalSection.Leave();

	return ret;
}

bool CNATSimPortsArray::IsFree(CUtilString IP, long Port)
{
	bool	ret = false;
	dword	Idx;
		
	this->m_CriticalSection.Enter();
	
	if ( this->FindItem(IP, &Idx ) )
	{
		long byt = Port/8;
		byte bit = (byte)(Port%8);
		byte mask = (bit==0?0x01:(bit==1?0x02:(bit==2?0x04:(bit==3?0x08:(bit==4?0x10:(bit==5?0x20:(bit==6?0x40:(bit==7?0x80:0xFF))))))));
		ret = (this->m_Tbl[Idx]->PortsAvailability[byt] & mask) == 0x00;
	}	
	
	this->m_CriticalSection.Leave();

	return ret;
}

bool CNATSimPortsArray::ReserverPort(CUtilString IP, long Port)
{
	bool	ret = false;
	dword	Idx;
		
	this->m_CriticalSection.Enter();
	
	if ( this->FindItem(IP, &Idx ) )
	{
		long byt = Port/8;
		byte bit = (byte)(Port%8);
		byte mask = (bit==0?0x01:(bit==1?0x02:(bit==2?0x04:(bit==3?0x08:(bit==4?0x10:(bit==5?0x20:(bit==6?0x40:(bit==7?0x80:0xFF))))))));
		if ( (this->m_Tbl[Idx]->PortsAvailability[byt] & mask) == 0x00 )
		{
			this->m_Tbl[Idx]->PortsAvailability[byt] |= mask;
		}
	}	
	
	this->m_CriticalSection.Leave();

	return ret;
}

