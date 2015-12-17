/*
 *  CaptureQueue.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 21/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CaptureDefs.h"

CCaptureQueue::CCaptureQueue(CUtilTrace* Trace) : CQueue<CaptureStruct>()
{
	mTrace = Trace;
}

CCaptureQueue::~CCaptureQueue()
{
}

