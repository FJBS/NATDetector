/*
 *  NATSimBindTable.h
 *  NATDetector
 *
 *  Created by System Administrator on 26/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

struct BindDataStruct
{
	const CNATSimAddrId & GetId() const	{ return BindId; };
	
	long			PublicId;
	long			PrivateId;
	CNATSimAddrId	BindId;	
};

class CNATSimBindTable : public CIdTbl<BindDataStruct,CNATSimAddrId>
{
public:

	CNATSimBindTable();
	virtual ~CNATSimBindTable();
	
	bool InsertBind(long PublicId, long PrivateId, CUtilString & Ip, long Port);
	bool DeleteBind(long PublicId, long PrivateId, CUtilString & Ip, long Port);
	bool GetBind(long PublicId, long PrivateId, CUtilString & Ip, long & Port);
};

