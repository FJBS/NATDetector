/*
 *  NATSimPortsArray.h
 *  NATDetector
 *
 *  Created by System Administrator on 26/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#define MIN_NUM_PORT 0
#define MAX_NUM_PORT 65535

struct PortDataStruct
{
	const CUtilString & GetId() const { return IPId; };
	
	CUtilString	IPId;	
	byte		PortsAvailability[MAX_NUM_PORT/sizeof(byte)];
};

class CNATSimPortsArray : public CIdTbl<PortDataStruct,CUtilString>
{

public:
	CNATSimPortsArray();
	~CNATSimPortsArray();
	
	bool InsertIP(CUtilString IP);
	bool RemoveIP(CUtilString IP);
	
	long GetFirstReservedPort(CUtilString IP);
	long GetFirstAvailablePort(CUtilString IP);
	long ReserveFirstAvailablePort(CUtilString IP);
	bool FreePort(CUtilString IP, long Port);
	bool ReserverPort(CUtilString IP, long Port);

	bool IsFree(CUtilString IP, long Port);
};

