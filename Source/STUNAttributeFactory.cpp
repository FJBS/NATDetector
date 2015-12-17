/*
 *  STUNAttributeFactory.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 04/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNAttribute & CSTUNAttributeFactory::createMappedAddressAttribute(CUtilTrace *Trace, CSTUNAddress address)
{
	CSTUNAttribute * attribute = new CSTUNAttribute(Trace, CSTUNAttribute::MAPPED_ADDRESS);

	attribute->setAddress(address);

	return *attribute;
}

CSTUNAttribute & CSTUNAttributeFactory::createXORMappedAddressAttribute(CUtilTrace *Trace, CSTUNAddress address)
{
	CSTUNAttribute * attribute = new CSTUNAttribute(Trace, CSTUNAttribute::XOR_MAPPED_ADDRESS);

	attribute->setAddress(address);

	return *attribute;
}

CSTUNAttribute & CSTUNAttributeFactory::createChangeRequestAttribute(CUtilTrace *Trace)
{
	return createChangeRequestAttribute(Trace, false, false);
}
	
CSTUNAttribute & CSTUNAttributeFactory::createChangeRequestAttribute(CUtilTrace *Trace, bool changeIP, bool changePort)
{
	CSTUNAttribute * attribute = new CSTUNAttribute(Trace, CSTUNAttribute::CHANGE_REQUEST);

	attribute->setChangeIpFlag(changeIP);
	attribute->setChangePortFlag(changePort);

	return *attribute;
}

CSTUNAttribute & CSTUNAttributeFactory::createChangedAddressAttribute(CUtilTrace *Trace, CSTUNAddress address)
{
	CSTUNAttribute * attribute = new CSTUNAttribute(Trace, CSTUNAttribute::CHANGED_ADDRESS);

	attribute->setAddress(address);

	return *attribute;
}

CSTUNAttribute & CSTUNAttributeFactory::createSourceAddressAttribute(CUtilTrace *Trace, CSTUNAddress address)
{
	CSTUNAttribute * attribute = new CSTUNAttribute(Trace, CSTUNAttribute::SOURCE_ADDRESS);

	attribute->setAddress(address);

	return *attribute;
}

CSTUNAttribute & CSTUNAttributeFactory::createResponseAddressAttribute(CUtilTrace *Trace, CSTUNAddress address)
{
	CSTUNAttribute * attribute = new CSTUNAttribute(Trace, CSTUNAttribute::RESPONSE_ADDRESS);

	attribute->setAddress(address);

	return *attribute;
}
