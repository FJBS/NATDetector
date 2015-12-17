/*
 *  STUNAttributeDecoder.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 16/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNAttribute & CSTUNAttributeDecoder::decode(CUtilTrace *Trace, byte *bytes, byte *offset, word length)
{
	if(bytes == NULL || length - (*offset) < CSTUNAttribute::HEADER_LENGTH)
		throw new CSTUNException("Could not decode the specified binary array.");

	//Discover attribute type
	word attributeType = ntohs((word)(*((word*)(bytes+(*offset)))));
	word attributeLength = ntohs((word)(*((word*)(bytes+(*offset+2)))));

	if(attributeLength >length - (*offset) )
		throw new CSTUNException("The indicated attribute length does not match the length of the passed binary array");

	CSTUNAttribute *decodedAttribute = NULL;

	if ( attributeType == CSTUNAttribute::CHANGE_REQUEST )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::CHANGE_REQUEST);
	else if ( attributeType == CSTUNAttribute::CHANGED_ADDRESS )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::CHANGED_ADDRESS);
	else if ( attributeType == CSTUNAttribute::MAPPED_ADDRESS )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::MAPPED_ADDRESS);
	else if ( attributeType == CSTUNAttribute::ERROR_CODE )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::ERROR_CODE);
	else if ( attributeType == CSTUNAttribute::MESSAGE_INTEGRITY )
		throw new CSTUNException("The MESSAGE-INTEGRITY Attribute is not yet implemented.");
	else if ( attributeType == CSTUNAttribute::PASSWORD )
		throw new CSTUNException("The PASSWORD Attribute is not yet implemented.");
	else if ( attributeType == CSTUNAttribute::REFLECTED_FROM )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::REFLECTED_FROM);
	else if ( attributeType == CSTUNAttribute::RESPONSE_ADDRESS )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::RESPONSE_ADDRESS);
	else if ( attributeType == CSTUNAttribute::SOURCE_ADDRESS )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::SOURCE_ADDRESS);
	else if ( attributeType == CSTUNAttribute::UNKNOWN_ATTRIBUTES )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::UNKNOWN_ATTRIBUTES);
	else if ( attributeType == CSTUNAttribute::XOR_MAPPED_ADDRESS )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::XOR_MAPPED_ADDRESS);
	else if ( attributeType == CSTUNAttribute::XOR_ONLY )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::XOR_ONLY);
	else if ( attributeType == CSTUNAttribute::SERVER )
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::SERVER);
	else if ( attributeType == CSTUNAttribute::USERNAME )
		throw new CSTUNException("The USERNAME Attribute is not yet implemented.");
	//According to rfc3489 we should silently ignore unknown attributes.
	else 
		decodedAttribute = new CSTUNAttribute(Trace, CSTUNAttribute::UNKNOWN_OPTIONAL_ATTRIBUTE);

	decodedAttribute->setAttributeType(attributeType);

	decodedAttribute->decodeAttributeBody(bytes, *offset+4, attributeLength);

	*offset+=CSTUNAttribute::HEADER_LENGTH+attributeLength;

	return *decodedAttribute;
}

