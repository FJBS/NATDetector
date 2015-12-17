/*
 *  CSTUNResponse.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 26/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNResponse::CSTUNResponse(CUtilTrace* Trace)
	: CSTUNMsg(Trace)
{
}

void CSTUNResponse::setMessageType(word requestType)
{
	if(!isResponseType(requestType))
		throw new CSTUNException(" Invalid request type.");

	CSTUNMsg::setMessageType(requestType);
}
