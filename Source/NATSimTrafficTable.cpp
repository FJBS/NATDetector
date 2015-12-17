/*
 *  NATSimTrafficTable.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATSimDefs.h"

CNATSimTrafficTable::CNATSimTrafficTable():CIdTbl<TrafficDataStruct,CNATSimTrafficId>()
{
}

CNATSimTrafficTable::~CNATSimTrafficTable()
{
}

bool CNATSimTrafficTable::InsertTraffic(CNATSimTrafficId & TrafficId, CUtilString IP, dword Port, CNATSimTrafficId & ReverseId)
{
	bool				ret = true;
	dword				Idx = 0;
	TrafficDataStruct	TrafficData;

#ifndef WIN32
	struct tms			ticks;
#endif

	TrafficData.TrafficId = TrafficId;
	TrafficData.IP = IP;
	TrafficData.Port  = Port;
	TrafficData.ReverseId = ReverseId;

#ifndef WIN32
	TrafficData.LastTraffic = times(&ticks);
#else
	TrafficData.LastTraffic = GetTickCount();
#endif
	
	this->m_CriticalSection.Enter();

	if ( !this->FindItem(TrafficId, &Idx) )
	{
		ret = this->Insert(TrafficData);
	}
	else
	{
#ifndef WIN32
		this->m_Tbl[Idx]->LastTraffic = times(&ticks);
#else
		this->m_Tbl[Idx]->LastTraffic = GetTickCount();
#endif
	}
	
	this->m_CriticalSection.Leave();

	return ret;
}

bool CNATSimTrafficTable::DeleteTraffic(CNATSimTrafficId & TrafficId)
{
	bool				ret = true;
	dword				Idx = 0;

	this->m_CriticalSection.Enter();

	if ( this->FindItem(TrafficId, &Idx) )
	{
		ret = this->Remove(Idx);
	}
	
	this->m_CriticalSection.Leave();

	return ret;
}

void CNATSimTrafficTable::UpdateLastTrafficTime(dword Idx)
{
#ifndef WIN32
	struct tms ticks;
#endif

	this->m_CriticalSection.Enter();

	if ( this->GetElements() > Idx )
	{
#ifndef WIN32
		this->m_Tbl[Idx]->LastTraffic = times(&ticks);
#else
		this->m_Tbl[Idx]->LastTraffic = GetTickCount();
#endif	
	}
	
	this->m_CriticalSection.Leave();
}

bool CNATSimTrafficTable::Clean(CNATSimTrafficId ReverseId )
{
	bool	ret = true;
	dword	Idx = 0;

	this->m_CriticalSection.Enter();

	for ( dword Idx = 0; Idx < this->GetElements(); Idx++)
	{
		if ( this->m_Tbl[Idx]->ReverseId == ReverseId )
		{
			ret &= this->Remove(Idx);
		}
	}
	
	this->m_CriticalSection.Leave();

	return ret;
}

CUtilString CNATSimTrafficTable::ToString()
{
	CUtilString ret;
	char		buffer[300];

	ret = "\n";
	for ( dword i = 0; i < this->m_Elements; i++ )
	{

		sprintf(buffer, "%90s:%05d - %15s:%05d : %15s:%05d : %15s:%05d - %15s:%05d",	(const char *)(m_Tbl[i]->TrafficId.GetIpS()), m_Tbl[i]->TrafficId.GetPortS(),
																						(const char *)(m_Tbl[i]->TrafficId.GetIpD()), m_Tbl[i]->TrafficId.GetPortD(),
																						(const char *)(m_Tbl[i]->IP), m_Tbl[i]->Port,
																						(const char *)(m_Tbl[i]->ReverseId.GetIpS()), m_Tbl[i]->ReverseId.GetPortS(),
																						(const char *)(m_Tbl[i]->ReverseId.GetIpD()), m_Tbl[i]->ReverseId.GetPortD());

		ret += buffer;
		ret += "\n";
	}

	return ret;
}