/*
 *  STUNMessageFactory.h
 *  NATDetector
 *
 *  Created by Francisco Jos√© Bl√°zquez S√°nchez on 03/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

class CSTUNMsgFactory
{
public:

    /**
     * Creates a default binding request. The request contains a ChangeReqeust
     * attribute with zero change ip and change port flags.
     * @return a default binding request.
     */
    static CSTUNRequest & createBindingRequest(CUtilTrace *Trace, bool ChangeIP = false, bool ChangePort= false);	
		
	/**
	 * Creates a BindingResponse assigning the specified values to mandatory
	 * headers.
	 *
	 * @param mappedAddress     the address to assign the mappedAddressAttribute
	 * @param sourceAddress     the address to assign the sourceAddressAttribute
	 * @param changedAddress    the address to assign the changedAddressAttribute
	 * @return a BindingResponse assigning the specified values to mandatory
	 *         headers.
	 * @throws StunException ILLEGAL_ARGUMENT
	 */
	static CSTUNResponse & createBindingResponse(CUtilTrace *Trace, CSTUNAddress mappedAddress, CSTUNAddress sourceAddress, CSTUNAddress changedAddress, CSTUNAddress xorMappedAddress);

	static void Debug(CUtilTrace *Trace);
};
