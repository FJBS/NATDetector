/*
 *  CSTUNRequest.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 26/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNRequest::CSTUNRequest(CUtilTrace *  Trace)
	: CSTUNMsg(Trace)
{
}

CSTUNRequest::CSTUNRequest(const CSTUNRequest & other) : CSTUNMsg(other)
{
}

CSTUNRequest::~CSTUNRequest()
{
}

void CSTUNRequest::setMessageType(word requestType)
{
	if(!isRequestType(requestType))
		throw new CSTUNException(" Invalid request type.");

	CSTUNMsg::setMessageType(requestType);
}
