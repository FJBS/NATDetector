/*
 *  Exception.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 17/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */
 
#pragma once

class CUtilException 
{
	private:
		int ErrNo;
		CUtilString Msg;

	public:
		CUtilException (const CUtilString M, int Err=0):	Msg(M), ErrNo(Err) 
															{};
		virtual ~CUtilException (void)						{};

	public:
		int GetError(void) const							{ return ErrNo; };
		CUtilString GetMessage(void) const					{ return Msg; };
};


