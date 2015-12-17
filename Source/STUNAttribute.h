/*
 *  STUNAttribute.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 13/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

/**
 * After the header are 0 or more attributes.  Each attribute is TLV
 * encoded, with a 16 bit type, 16 bit length, and variable value:
 *
 *     0                   1                   2                   3       
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1     
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    
 *    |         Type                  |            Length             |    
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    
 *    |                             Value                             .... 
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    
 *
 *    The following types are defined:
 *
 *    0x0001: MAPPED-ADDRESS                                               
 *    0x0002: RESPONSE-ADDRESS                                             
 *    0x0003: CHANGE-REQUEST                                               
 *    0x0004: SOURCE-ADDRESS                                               
 *    0x0005: CHANGED-ADDRESS                                              
 *    0x0006: USERNAME                                                     
 *    0x0007: PASSWORD                                                     
 *    0x0008: MESSAGE-INTEGRITY                                            
 *    0x0009: ERROR-CODE                                                   
 *    0x000a: UNKNOWN-ATTRIBUTES                                           
 *    0x000b: REFLECTED-FROM                                               
 *
 */
class CSTUNAttribute
{
public:
    static const word MAPPED_ADDRESS;
    static const word RESPONSE_ADDRESS;
    static const word CHANGE_REQUEST;
    static const word SOURCE_ADDRESS;
    static const word CHANGED_ADDRESS;
    static const word USERNAME;
    static const word PASSWORD;
    static const word MESSAGE_INTEGRITY;
    static const word ERROR_CODE;
    static const word UNKNOWN_ATTRIBUTES;
    static const word REFLECTED_FROM;
    static const word XOR_MAPPED_ADDRESS;
    static const word XOR_ONLY;
    static const word SERVER;
    static const word UNKNOWN_OPTIONAL_ATTRIBUTE;

    /**
     * Creates an empty STUN message attribute.
     *
     * @param attributeType the type of the attribute.
     */
    CSTUNAttribute(CUtilTrace* Trace);

    /**
     * Creates an empty STUN message attribute.
     *
     * @param attributeType the type of the attribute.
     */
    CSTUNAttribute(CUtilTrace* Trace, word type);

	~CSTUNAttribute();

    /**
     * The size of an atribute header in bytes = len(TYPE) + len(LENGTH) = 4
     */
    static const int HEADER_LENGTH;

    /**
     * Returns the length of this attribute's body.
     * @return the length of this attribute's value.
     */
    int getDataLength();
	
    /**
     * Returns the human readable name of this attribute. Attribute names do
     * not really matter from the protocol point of view. They are only used
     * for debugging and readability.
     * @return this attribute's name.
     */
    CUtilString getName();

   /**
    * Compares two STUN Attributes. Two attributes are considered equal when they
    * have the same type length and value.
    *
    * @param obj the object to compare this attribute with.
    * @return true if the attributes are equal and false otherwise.
    */

    bool equals(CSTUNAttribute & other);

    /**
     * Returns a binary representation of this attribute.
     * @return a binary representation of this attribute.
     */
    bool encode(byte** binAtt, int *length);

    // TODO bool encode(CUtilByteArray & binAtt);

    /**
     * Returns the attribute's type.
     * @return the type of this attribute.
     */
    word getAttributeType() { return mAttributeType; };
	
	/**
	 * Return the attribute type, nedeed for CIdTbl iplementation
	 */
	dword GetId() const { return mAttributeType; };

    /**
     * Sets the attribute's type.
     * @param type the new type of this attribute
     */
    void setAttributeType(word type);

	
    /**
     * Sets this attribute's fields according to attributeValue array.
     *
     * @param attributeValue a binary array containing this attribute's field
     *                       values and NOT containing the attribute header.
     * @param offset the position where attribute values begin (most often
     * 				 offset is equal to the index of the first byte after
     * 				 length)
     * @param length the length of the binary array.
     * @throws StunException if attrubteValue contains invalid data.
     */
    void decodeAttributeBody(byte* attributeValue, byte offset, word length);
	
    // TODO void decodeAttributeBody(CUtilByteArray & attValue, byte offset);

	void Trace();
	
protected:

	CUtilTrace*	mTrace;
    /**
     * The type of the attribute.
     */
	word mAttributeType;

    CUtilString mName;

 /*
 * MAPPED-ADDRESS 
 * RESPONSE-ADDRESS 
 * SOURCE-ADDRESS 
 * CHANGED-ADDRESS 
 * REFLECTED-FROM 
 *
 * The different attributes are distinguished by the attributeType 
 * Address attributes indicate the mapped IP address and
 * port.  They consist of an eight bit address family, and a sixteen bit
 * port, followed by a fixed length value representing the IP address.
 *
 *  0                   1                   2                   3          
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1        
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+       
 * |x x x x x x x x|    Family     |           Port                |       
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+       
 * |                             Address                           |       
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+       
 *                                                                         
 * The port is a network byte ordered representation of the mapped port.
 * The address family is always 0x01, corresponding to IPv4.  The first
 * 8 bits of the MAPPED-ADDRESS are ignored, for the purposes of
 * aligning parameters on natural boundaries.  The IPv4 address is 32
 * bits.
 */
 public:
 
    static const byte ADDRESS_FAMILY_IPV4;
    static const byte ADDRESS_FAMILY_IPV6;

    /**
     * Sets address to be the address transported by this attribute.
     * @param address that this attribute should encapsulate.
     */
    void setAddress(CSTUNAddress address);
	
    /**
     * Returns the address encapsulated by this attribute.
     * @return the address encapsulated by this attribute.
     */
    CSTUNAddress getAddress();

    CUtilByteArray & getAddressBytes();

    /**
     * Returns the family that the this.address belongs to.
     * @return the family that the this.address belongs to.
     */
    byte getFamily();

    /**
     * Returns the port associated with the address contained by the attribute.
     * @return the port associated with the address contained by the attribute.
     */
    word getPort();

protected:
     /**
      * The address represented by this message;
      */
     CSTUNAddress mAddress;

private:

     /**
      * The length of the data contained by this attribute in the case of an
      * IPv6 address.
      */
     static const byte DATA_LENGTH_FOR_IPV6;

     /**
      * The length of the data contained by this attribute in the case of an
      * IPv4 address.
      */
     static const byte DATA_LENGTH_FOR_IPV4;

    /**
     * Verifies that type is a valid address attribute type.
     * @param type the type to test
     * @return true if the type is a valid address attribute type and false
     * otherwise
     */
    bool isTypeValid(dword type);
	
/*
 * This class represents the STUN CHANGE-REQUEST attribute. The CHANGE-REQUEST
 * attribute is used by the client to request that the server use a different
 * address and/or port when sending the response.  The attribute is 32 bits
 * long, although only two bits (A and B) are used:
 *
 * 0                   1                   2                   3           <br/>
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1         <br/>
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+       <br/>
 * |0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 A B 0|       <br/>
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+       <br/>
 *
 * The meaning of the flags is:
 *
 * A: This is the "change IP" flag.  If true, it requests the server
 *    to send the Binding Response with a different IP address than the
 *    one the Binding Request was received on.
 *
 * B: This is the "change port" flag.  If true, it requests the
 *    server to send the Binding Response with a different port than the
 *    one the Binding Request was received on.
 */
public:

    /**
     * The length of the data contained by this attribute.
     */
    static const byte DATA_LENGTH;

    /**
     * Sets the value of the changeIpFlag. The "change IP" flag,  if true,
     * requests the server to send the Binding Response with a different IP
     * address than the one the Binding Request was received on.
     *
     * @param changeIP the new value of the changeIpFlag.
     */
    void setChangeIpFlag(bool changeIP);

    /**
     * Returns tha value of the changeIpFlag. The "change IP" flag,  if true,
     * requests the server to send the Binding Response with a different IP
     * address than the one the Binding Request was received on.
     *
     * @return the value of the changeIpFlag.
     */
    bool getChangeIpFlag();

    /**
     * Sets the value of the changePortFlag. The "change port" flag.  If true,
     * requests the server to send the Binding Response with a different port
     * than the one the Binding Request was received on.
     *
     * @param changePort the new value of the changePort flag.
     */
    void setChangePortFlag(bool changePort);
	
    /**
     * Returns the value of the changePortFlag. The "change port" flag.  If true,
     * requests the server to send the Binding Response with a different port
     * than the one the Binding Request was received on.
     *
     * @return the value of the changePort flag.
     */
    bool getChangePortFlag();
	
/*
 * he XOR-MAPPED-ADDRESS attribute is only present in Binding
 * Responses.  It provides the same information that is present in the
 * MAPPED-ADDRESS attribute.  However, the information is encoded by
 * performing an exclusive or (XOR) operation between the mapped address
 * and the transaction ID.  Unfortunately, some NAT devices have been
 * found to rewrite binary encoded IP addresses and ports that are
 * present in protocol payloads.  This behavior interferes with the
 * operation of STUN.  By providing the mapped address in an obfuscated
 * form, STUN can continue to operate through these devices.
 *
 * The format of the XOR-MAPPED-ADDRESS is:
 *
 *
 * 0                   1                   2                   3
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |x x x x x x x x|    Family     |         X-Port                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                X-Address (Variable)
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The Family represents the IP address family, and is encoded
 * identically to the Family in MAPPED-ADDRESS.
 *
 * X-Port is equal to the port in MAPPED-ADDRESS, exclusive or'ed with
 * most significant 16 bits of the transaction ID.  If the IP address
 * family is IPv4, X-Address is equal to the IP address in MAPPED-
 * ADDRESS, exclusive or'ed with the most significant 32 bits of the
 * transaction ID.  If the IP address family is IPv6, the X-Address is
 * equal to the IP address in MAPPED-ADDRESS, exclusive or'ed with the
 * entire 128 bit transaction ID.
 */
	static CSTUNAddress applyXor(CSTUNAddress & address, CSTUNTransactionID & transactionID);

private:

    /**
     * This is the "change IP" flag.  If true, it requests the server
     * to send the Binding Response with a different IP address than the
     * one the Binding Request was received on.
     */
    bool mChangeIpFlag;

    /**
     * This is the "change port" flag.  If true, it requests the
     * server to send the Binding Response with a different port than the
     * one the Binding Request was received on.
     */
    bool mChangePortFlag;

};
