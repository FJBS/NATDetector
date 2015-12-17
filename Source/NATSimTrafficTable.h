/*
 *  NATSimTrafficTable.h
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

struct TrafficDataStruct
{
	const CNATSimTrafficId & GetId() const	{ return TrafficId; };
	
	CNATSimTrafficId	TrafficId;	
	
	CUtilString			IP;
	dword				Port;
	clock_t				LastTraffic;

	CNATSimTrafficId	ReverseId;
};

class CNATSimTrafficTable : public CIdTbl<TrafficDataStruct,CNATSimTrafficId>
{
public:

	CNATSimTrafficTable();
	virtual ~CNATSimTrafficTable();
		
	bool InsertTraffic(CNATSimTrafficId & TrafficId, CUtilString IP, dword Port, CNATSimTrafficId & ReverseId );
	bool DeleteTraffic(CNATSimTrafficId & TrafficId);

	bool Clean(CNATSimTrafficId ReverseId );

	void UpdateLastTrafficTime(dword Idx);

	CUtilString ToString();
};
