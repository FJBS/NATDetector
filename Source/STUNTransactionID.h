/*
 *  STUNTransactionID.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

class CSTUNTransactionID 
{
public:
	/**
	 * The length of the transaction id (in bytes).
	 */
	static const byte TRANSACTION_ID_LENGTH;

	CSTUNTransactionID();
	CSTUNTransactionID(byte* transactionID);
	CSTUNTransactionID(CSTUNTransactionID & other);

	virtual ~CSTUNTransactionID();

	bool operator == (const CSTUNTransactionID & other) const;
	CSTUNTransactionID & operator = (const CSTUNTransactionID & other);
	operator const byte * () const;
	operator const CUtilString () const;
			
private:
	byte* mTransactionID;	
};
