/*
 *  STUNQueue.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 22/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNQueue::CSTUNQueue(CUtilTrace* Trace) : CQueue<CUtilByteArray>()
{
	mTrace = Trace;
}

CSTUNQueue::~CSTUNQueue()
{
}
