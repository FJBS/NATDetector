/*
 *  NATSimAddrTable.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATSimDefs.h"

CNATSimAddrTable::CNATSimAddrTable():CIdTbl<AddrDataStruct,CNATSimAddrId>()
{
	mNextId = 0;
}

CNATSimAddrTable::~CNATSimAddrTable()
{
}
	
bool CNATSimAddrTable::InsertAddr(CUtilString & Ip, long Port, dword & Id)
{
	bool			ret = true;
	dword			Idx = 0;
	AddrDataStruct	Address;

	CNATSimAddrId AddrId(Ip, Port);

	Address.Id = mNextId;
	Address.AddrId  = AddrId;
	
	this->m_CriticalSection.Enter();

	if ( !this->FindItem(AddrId, &Idx) )
	{
		ret = this->Insert(Address);
		if ( ret )
		{
			Id = mNextId;
			mNextId++;
		}	
	}
	
	this->m_CriticalSection.Leave();

	return ret;
}

bool CNATSimAddrTable::DeleteAddr(CUtilString & Ip, long Port)
{
	bool			ret = true;
	dword			Idx = 0;
	AddrDataStruct	Address;

	CNATSimAddrId AddrId(Ip, Port);
	Address.AddrId  = AddrId;
	
	this->m_CriticalSection.Enter();

	if ( this->FindItem(AddrId, &Idx) )
	{
		ret = this->Remove(Idx);
	}
	
	this->m_CriticalSection.Leave();

	return ret;
}
