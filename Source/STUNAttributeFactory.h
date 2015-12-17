/*
 *  STUNAttributeFactory.h
 *  NATDetector
 *
 *  Created by Francisco Jos√© Bl√°zquez S√°nchez on 04/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

class CSTUNAttributeFactory
{
public:

    /**
     * Creates a MappedAddressAttribute of the specified type and with the
     * specified address and port
     * @param address the address value of the address attribute
     * @return the newly created address attribute.
     */
    static CSTUNAttribute & createMappedAddressAttribute(CUtilTrace *Trace, CSTUNAddress address);
	
    /**
     * Creates a XORMappedAddressAttribute of the specified type and with the
     * specified address and port
     * @param address the address value of the address attribute
     * @return the newly created address attribute.
     */
	static CSTUNAttribute & createXORMappedAddressAttribute(CUtilTrace *Trace, CSTUNAddress address);

    /**
     * Creates a ChangeRequestAttribute with "false" values for the changeIP and
     * changePort flags.
     * @return the newly created ChangeRequestAttribute.
     */
    static CSTUNAttribute & createChangeRequestAttribute(CUtilTrace *Trace);
	
	/**
     * Creates a ChangeRequestAttribute with the specified flag values.
     * @param changeIP   the value of the changeIP flag.
     * @param changePort the value of the changePort flag.
     * @return the newly created ChangeRequestAttribute.
     */
    static CSTUNAttribute & createChangeRequestAttribute(CUtilTrace *Trace, bool changeIP, bool changePort);
	
	/**
	 * Creates a changedAddressAttribute of the specified type and with the
	 * specified address and port
	 * @param address the address value of the address attribute
	 * @return the newly created address attribute.
	 */
	static CSTUNAttribute & createChangedAddressAttribute(CUtilTrace *Trace, CSTUNAddress address);
	
    /**
     * Creates a SourceFromAddressAttribute of the specified type and with
     * the specified address and port
     * @param address the address value of the address attribute
     * @return the newly created address attribute.
     */
    static CSTUNAttribute & createSourceAddressAttribute(CUtilTrace *Trace, CSTUNAddress address);

    /**
     * Creates a ResponseAddressAttribute of the specified type and with
     * the specified address and port
     * @param address the address value of the address attribute
     * @return the newly created address attribute.
     */
    static CSTUNAttribute & createResponseAddressAttribute(CUtilTrace *Trace, CSTUNAddress address);

};
