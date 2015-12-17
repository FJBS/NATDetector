/*
 *  STUNQueue.h
 *  NATDetector
 *
 *  Created by System Administrator on 22/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

class CSTUNQueue : public CQueue<CUtilByteArray>
{
public:
	CSTUNQueue(CUtilTrace* Trace);
	virtual ~CSTUNQueue();
		
private:
	CUtilTrace	*mTrace;
};
