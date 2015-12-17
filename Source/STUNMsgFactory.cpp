/*
 *  STUNMessageFactory.cpp
 *  NATDetector
 *
 *  Created by Francisco Jos‚àö¬© Bl‚àö¬∞zquez S‚àö¬∞nchez on 03/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNRequest & CSTUNMsgFactory::createBindingRequest(CUtilTrace *Trace, bool ChangeIP, bool ChangePort)
{
	CSTUNRequest *bindingRequest = new CSTUNRequest(Trace);
	
	try
	{
		bindingRequest->setMessageType(CSTUNMsg::BINDING_REQUEST);
	}
	catch (CSTUNException & ex)
	{
		Trace->Trace("**STUN:EXCEP**", "CSTUNMessageFactory", "createBindingRequest", "Failed setMessageType(). (errno=%d)", ex.GetError());
	}

	//add a change request attribute
	CSTUNAttribute & attribute = CSTUNAttributeFactory::createChangeRequestAttribute(Trace);
	attribute.setChangeIpFlag(ChangeIP);
	attribute.setChangePortFlag(ChangePort);
	
	try
	{
		bindingRequest->addAttribute(attribute);
	}
	catch (CSTUNException ex)
	{
		throw new CSTUNException("Failed to add a change request attribute to a binding request!");
	}

	delete &attribute;
	
	return *bindingRequest;
}

CSTUNResponse & CSTUNMsgFactory::createBindingResponse(CUtilTrace *Trace, CSTUNAddress mappedAddress, CSTUNAddress sourceAddress, CSTUNAddress changedAddress, CSTUNAddress xorMappedAddress)
{
	CSTUNResponse * bindingResponse = new CSTUNResponse(Trace);
	
	bindingResponse->setMessageType(CSTUNMsg::BINDING_RESPONSE);

	//mapped address
	CSTUNAttribute & mappedAddressAttribute =	CSTUNAttributeFactory::createMappedAddressAttribute(Trace, mappedAddress);

	//source address
	CSTUNAttribute & sourceAddressAttribute =	CSTUNAttributeFactory::createSourceAddressAttribute(Trace, sourceAddress);

	//changed address
	CSTUNAttribute & changedAddressAttribute = CSTUNAttributeFactory::createChangedAddressAttribute(Trace, changedAddress);

	//xor mapped address
	CSTUNAttribute & xorMappedAddressAttribute = CSTUNAttributeFactory::createXORMappedAddressAttribute(Trace, xorMappedAddress);

	try
	{
		bindingResponse->addAttribute(mappedAddressAttribute);
		bindingResponse->addAttribute(sourceAddressAttribute);
		bindingResponse->addAttribute(changedAddressAttribute);
		bindingResponse->addAttribute(xorMappedAddressAttribute);
	}
	catch (CSTUNException ex)
	{
		throw new CSTUNException("Failed to add a mandatory attribute to the binding response.");
	}

	delete &mappedAddressAttribute;
	delete &sourceAddressAttribute;
	delete &changedAddressAttribute;
	delete &xorMappedAddressAttribute;

	return * bindingResponse;
}

void CSTUNMsgFactory::Debug(CUtilTrace *Trace)
{
// TODO: Ver porque encode y decode cambian transaction id del mensaje

	//CSTUNRequest STUNRequest1 = CSTUNMsgFactory::createBindingRequest(Trace, false, false);
	//Sleep(2000);
	//CSTUNRequest STUNRequest2 = CSTUNMsgFactory::createBindingRequest(Trace, false, false);

	//CUtilByteArray b;
	//STUNRequest1.encode(b);
	//Trace->TraceBinary("STUNRequest1", __FUNCTION__, "%s", (const byte *)b, b.length());

	//STUNRequest2.encode(b);
	//Trace->TraceBinary("STUNRequest2", __FUNCTION__, "%s", (const byte *)b, b.length());

	//CSTUNTransactionID	TransactionID = STUNRequest1.getTransactionID();
	//STUNRequest2.setTransactionID(TransactionID);
	//STUNRequest2.encode(b);
	//Trace->TraceBinary("STUNRequest2", __FUNCTION__, "%s", (const byte *)b, b.length());

	//CSTUNTransactionID	TransactionID;
	//TransactionID = STUNRequest1.getTransactionID();

	//STUNRequest.setTransactionID(TransactionID);
	//STUNRequest.encode(b);
	//Trace->TraceBinary("**DEBUG**", __FUNCTION__, "%s", (const byte *)b, b.length());
	//CSTUNMsg STUNResponseDecoded = CSTUNMsg::decode(Trace, b, 0);
	//STUNResponseDecoded.encode(b);
	//Trace->TraceBinary("**DEBUG**", __FUNCTION__, "%s", (const byte *)b, b.length());						

}