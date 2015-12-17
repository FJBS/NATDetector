/*
 *  CaptureQueue.h
 *  NATDetector
 *
 *  Created by System Administrator on 21/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

class CCaptureQueue : public CQueue<CaptureStruct>
{
public:
	CCaptureQueue(CUtilTrace* Trace);
	virtual ~CCaptureQueue();
		
private:
	CUtilTrace	*mTrace;
};
