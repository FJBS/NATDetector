/*
 *  NATSimAddrTable.h
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

struct AddrDataStruct
{
	const CNATSimAddrId & GetId() const	{ return AddrId; };
	
	dword			Id;
	CNATSimAddrId	AddrId;	
};

class CNATSimAddrTable : public CIdTbl<AddrDataStruct,CNATSimAddrId>
{
public:

	CNATSimAddrTable();
	virtual ~CNATSimAddrTable();
	
	bool InsertAddr(CUtilString & Ip, long Port, dword & Id);
	bool DeleteAddr(CUtilString & Ip, long Port);
	
private:
	dword mNextId;
};
