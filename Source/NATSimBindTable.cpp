/*
 *  NATSimBindTable.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 26/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATSimDefs.h"

CNATSimBindTable::CNATSimBindTable():CIdTbl<BindDataStruct,CNATSimAddrId>()
{
}

CNATSimBindTable::~CNATSimBindTable()
{
}
	
bool CNATSimBindTable::InsertBind(long PublicId, long PrivateId, CUtilString & Ip, long Port)
{
	bool			ret = true;
	dword			Idx = 0;
	BindDataStruct	Binding;

	CNATSimAddrId	BindId(Ip, Port);

	Binding.PublicId = PublicId;
	Binding.PrivateId = PrivateId;
	Binding.BindId  = BindId;
	
	this->m_CriticalSection.Enter();

	if ( !this->FindItem(BindId, &Idx) )
	{
		ret = this->Insert(Binding);
	}
	
	this->m_CriticalSection.Leave();

	return ret;
}

bool CNATSimBindTable::DeleteBind(long PublicId, long PrivateId, CUtilString & Ip, long Port)
{
	bool			ret = true;
	dword			Idx = 0;

	CNATSimAddrId	BindId(Ip, Port);
	
	this->m_CriticalSection.Enter();

	if ( this->FindItem(BindId, &Idx) )
	{
		ret = this->Remove(Idx);
	}
	
	this->m_CriticalSection.Leave();

	return ret;
}

