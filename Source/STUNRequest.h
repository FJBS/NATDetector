/*
 *  CSTUNRequest.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 26/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

class CSTUNRequest : public CSTUNMsg
{
public:
	CSTUNRequest(CUtilTrace *  Trace);
	CSTUNRequest(const CSTUNRequest & other);
	~CSTUNRequest();

	/**
	 * Checks whether requestType is a valid request type and if yes sets it
	 * as the type of the current instance.
	 * @param requestType the type to set
	 * @throws StunException ILLEGAL_ARGUMENT if requestType is not a valid
	 * request type
	 */
	void setMessageType(word requestType);

};
