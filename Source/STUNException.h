/*
 *  STUNException.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 17/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

class CSTUNException: public CUtilException 
{

public:
	CSTUNException(const CUtilString M): CUtilException (M, EX_STUN) {};
	virtual ~CSTUNException(void){};
};
