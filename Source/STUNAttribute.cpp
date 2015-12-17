/*
 *  STUNAttribute.cpp
 *  NATDetector
 *
 *  Created by Francisco Jos√© Bl√°zquez S√°nchez on 13/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

const word CSTUNAttribute::MAPPED_ADDRESS				= 0x0001;
const word CSTUNAttribute::RESPONSE_ADDRESS				= 0x0002;
const word CSTUNAttribute::CHANGE_REQUEST				= 0x0003;
const word CSTUNAttribute::SOURCE_ADDRESS				= 0x0004;
const word CSTUNAttribute::CHANGED_ADDRESS				= 0x0005;
const word CSTUNAttribute::USERNAME						= 0x0006;
const word CSTUNAttribute::PASSWORD						= 0x0007;
const word CSTUNAttribute::MESSAGE_INTEGRITY			= 0x0008;
const word CSTUNAttribute::ERROR_CODE					= 0x0009;
const word CSTUNAttribute::UNKNOWN_ATTRIBUTES			= 0x000a;
const word CSTUNAttribute::REFLECTED_FROM				= 0x000b;
const word CSTUNAttribute::XOR_MAPPED_ADDRESS			= 0x8020;
const word CSTUNAttribute::XOR_ONLY						= 0x0021;
const word CSTUNAttribute::SERVER						= 0x8022;
const word CSTUNAttribute::UNKNOWN_OPTIONAL_ATTRIBUTE	= 0x8000;

const int CSTUNAttribute::HEADER_LENGTH					= 4;

const byte CSTUNAttribute::ADDRESS_FAMILY_IPV4	= 0x01;
const byte CSTUNAttribute::ADDRESS_FAMILY_IPV6	= 0x02;

const byte CSTUNAttribute::DATA_LENGTH_FOR_IPV6	= 20;
const byte CSTUNAttribute::DATA_LENGTH_FOR_IPV4	= 8;

const byte CSTUNAttribute::DATA_LENGTH	= 4;

/**
 * Creates an empty STUN message attribute.
 *
 * @param attributeType the type of the attribute.
 */
CSTUNAttribute::CSTUNAttribute(CUtilTrace* Trace)
{
	mTrace = Trace;
	setAttributeType(UNKNOWN_OPTIONAL_ATTRIBUTE);

    mChangeIpFlag   = false;
    mChangePortFlag = false;
}

/**
 * Creates an empty STUN message attribute.
 *
 * @param attributeType the type of the attribute.
 */
CSTUNAttribute::CSTUNAttribute(CUtilTrace* Trace, word attributeType)
{
	mTrace = Trace;
	setAttributeType(attributeType);

    mChangeIpFlag   = false;
    mChangePortFlag = false;
}

CSTUNAttribute::~CSTUNAttribute()
{
}

/**
 * Sets the attribute's type.
 * @param type the new type of this attribute
 */
void CSTUNAttribute::setAttributeType(word type)
{
	this->mAttributeType = type;
	if (type == MAPPED_ADDRESS )
		mName ="MAPPED_ADDRESS";
	else if ( type == RESPONSE_ADDRESS )
		mName ="RESPONSE_ADDRESS";
	else if ( type == SOURCE_ADDRESS )
		mName ="SOURCE_ADDRESS";
	else if ( type == CHANGED_ADDRESS )
		mName ="CHANGED_ADDRESS";
	else if ( type == REFLECTED_FROM )
		mName ="REFLECTED_FROM";
	else if ( type == XOR_MAPPED_ADDRESS )
		mName ="XOR_MAPPED_ADDRESS";
	else if ( type == CHANGE_REQUEST )
		mName ="CHANGE_REQUEST";
	else
		mName ="UNKNOWN_OPTIONAL_ATTRIBUTE";
	
	return;
}

/**
 * Verifies that type is a valid address attribute type.
 * @param type the type to test
 * @return true if the type is a valid address attribute type and false
 * otherwise
 */
bool CSTUNAttribute::isTypeValid(dword type)
{
	// TODO
	return (type == MAPPED_ADDRESS || type == RESPONSE_ADDRESS
			|| type == SOURCE_ADDRESS || type == CHANGED_ADDRESS
			|| type == REFLECTED_FROM || type == XOR_MAPPED_ADDRESS);
}

/**
 * Returns the human readable name of this attribute. Attribute names do
 * not really matter from the protocol point of view. They are only used
 * for debugging and readability.
 * @return this attribute's name.
 */
CUtilString CSTUNAttribute::getName()
{
	return mName;
	
	/* TODO
	switch(getAttributeType())
	{
		case MAPPED_ADDRESS:     return CSTUNMappedAddressAttribute::NAME;
		case RESPONSE_ADDRESS:   return CSTUNResponseAddressAttribute::NAME;
		case SOURCE_ADDRESS:     return CSTUNSourceAddressAttribute::NAME;
		case CHANGED_ADDRESS:    return CSTUNChangedAddressAttribute::NAME;
		case REFLECTED_FROM:     return CSTUNReflectedFromAttribute::NAME;
		case XOR_MAPPED_ADDRESS: return CSTUNXorMappedAddressAttribute::NAME;
	}
	*/
	
	return (CUtilString)"UNKNOWN ATTRIBUTE";
}

/**
* Compares two STUN Attributes. Attributeas are considered equal when their
* type, length, and all data are the same.
*
* @param obj the object to compare this attribute with.
* @return true if the attributes are equal and false otherwise.
*/
bool CSTUNAttribute::equals(CSTUNAttribute & other)
{
	
	if (getAttributeType() == MAPPED_ADDRESS   ||
		getAttributeType() == RESPONSE_ADDRESS ||
		getAttributeType() == SOURCE_ADDRESS   ||
		getAttributeType() == CHANGED_ADDRESS  ||
		getAttributeType() == REFLECTED_FROM   ||
		getAttributeType() == XOR_MAPPED_ADDRESS )
	{
		if (other.getAttributeType() != getAttributeType()
			|| other.getDataLength() != getDataLength()
			//compare data
			|| other.getFamily()     != getFamily()
			|| !mAddress.equals(other.getAddress())
			)
			return false;
	} 
	else if ( getAttributeType() == CHANGE_REQUEST )
	{
		 if (other.getAttributeType()		!= getAttributeType()
			 || other.getDataLength()		!= getDataLength()
			 //compare data
			 || other.getChangeIpFlag()		!= getChangeIpFlag()
			 || other.getChangePortFlag()   != getChangePortFlag()
			 )
			 return false;
	}
	
	return true;
}

/**
 * Returns the length of this attribute's body.
 * @return the length of this attribute's value (8 bytes).
 */
int CSTUNAttribute::getDataLength()
{
	if (getAttributeType() == MAPPED_ADDRESS   ||
		getAttributeType() == RESPONSE_ADDRESS ||
		getAttributeType() == SOURCE_ADDRESS   ||
		getAttributeType() == CHANGED_ADDRESS  ||
		getAttributeType() == REFLECTED_FROM   ||
		getAttributeType() == XOR_MAPPED_ADDRESS )
	{
		if (getFamily() == ADDRESS_FAMILY_IPV6)
			return DATA_LENGTH_FOR_IPV6;
		else
			return DATA_LENGTH_FOR_IPV4;
	}
	else if ( getAttributeType() == CHANGE_REQUEST )
	{
		return DATA_LENGTH;
	}
	else
		return 0;
}

/**
 * Sets address to be the address transported by this attribute.
 * @param address that this attribute should encapsulate.
 */
void CSTUNAttribute::setAddress(CSTUNAddress address)
{
	mAddress = address;
}

/**
 * Returns the address encapsulated by this attribute.
 * @return the address encapsulated by this attribute.
 */
CSTUNAddress CSTUNAttribute::getAddress()
{
	return mAddress;
}

CUtilByteArray & CSTUNAttribute::getAddressBytes()
{
	return mAddress.getAddressBytes();
}

/**
 * Returns the family that the this.address belongs to.
 * @return the family that the this.address belongs to.
 */
byte CSTUNAttribute::getFamily()
{
	/* TODO
	if ( mAddress.getSocketAddress().getAddress() instanceof Inet6Address )
		return ADDRESS_FAMILY_IPV6;
	else
		return ADDRESS_FAMILY_IPV4;
	*/
	return ADDRESS_FAMILY_IPV4;
}

/**
 * Returns the port associated with the address contained by the attribute.
 * @return the port associated with the address contained by the attribute.
 */
word CSTUNAttribute::getPort()
{
	return mAddress.getPort();
}

bool CSTUNAttribute::encode(byte** binAtt, int *length)
{
	bool	ret = true;
	int		len=0;
	
	*binAtt = NULL;
	*length = 0;
	
	byte *binAttAux = NULL;
	
	if (getAttributeType() == MAPPED_ADDRESS   ||
		getAttributeType() == RESPONSE_ADDRESS ||
		getAttributeType() == SOURCE_ADDRESS   ||
		getAttributeType() == CHANGED_ADDRESS  ||
		getAttributeType() == REFLECTED_FROM   ||
		getAttributeType() == XOR_MAPPED_ADDRESS )
	{
		dword type = getAttributeType();
		CUtilByteArray & address = getAddressBytes();

		if (!isTypeValid(type))
		{
			mTrace->Trace("**DEBUG**", __FUNCTION__, "Invalid attribute!");
			ret = false;
		}
		
		if ( ret )
		{
			binAttAux= new byte [8 + getDataLength()];

			//Type
			binAttAux[len++] = (byte)(type>>8);
			binAttAux[len++] = (byte)(type&0x00FF);
			//Length
			binAttAux[len++] = (byte)(getDataLength()>>8);
			binAttAux[len++] = (byte)(getDataLength()&0x00FF);
			//Not used
			binAttAux[len++] = 0x00;
			//Family
			binAttAux[len++] = getFamily();
			//port
			binAttAux[len++] = (byte)(getPort()>>8);
			binAttAux[len++] = (byte)(getPort()&0x00FF);

			//address
			if(getFamily() == ADDRESS_FAMILY_IPV6)
			{
				memcpy(binAttAux+8, address, 16);
				len+=16;
			}
			else
			{
				memcpy(binAttAux+8, address, 4);
				len+=4;
			}
		}

		delete address;
	}
	else if ( getAttributeType() == CHANGE_REQUEST )
	{
		binAttAux = new byte[HEADER_LENGTH + DATA_LENGTH];

		//Type
		binAttAux[len++] = (byte)(getAttributeType()>>8);
		binAttAux[len++] = (byte)(getAttributeType()&0x00FF);
		//Length
		binAttAux[len++] = (byte)(getDataLength()>>8);
		binAttAux[len++] = (byte)(getDataLength()&0x00FF);
		//Data
		binAttAux[len++] = 0x00;
		binAttAux[len++] = 0x00;
		binAttAux[len++] = 0x00;
		binAttAux[len++] = (byte)((getChangeIpFlag()?4:0) + (getChangePortFlag()?2:0));
	}

	*binAtt = binAttAux;
	*length = len;
	
	return ret;
}

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
void CSTUNAttribute::decodeAttributeBody(byte* attributeValue, byte offset, word length)
{
	if (getAttributeType() == MAPPED_ADDRESS   ||
		getAttributeType() == RESPONSE_ADDRESS ||
		getAttributeType() == SOURCE_ADDRESS   ||
		getAttributeType() == CHANGED_ADDRESS  ||
		getAttributeType() == REFLECTED_FROM   ||
		getAttributeType() == XOR_MAPPED_ADDRESS )
	{
		//skip through padding
		offset ++;

		//get family
		byte family = attributeValue[offset++];

		word port = ntohs((word)(*((word*)(attributeValue+offset))));
		offset+=2;

		//address
		byte * address = NULL;
		if(family == ADDRESS_FAMILY_IPV6){
			address = new byte[16];
		}
		else{
			//ipv4
			address = new byte[4];
		}

		memcpy(address, attributeValue+offset, family == ADDRESS_FAMILY_IPV6?16:4);
		setAddress(CSTUNAddress(address, family == ADDRESS_FAMILY_IPV6?16:4, port));
		delete address;
	}
	else if ( getAttributeType() == CHANGE_REQUEST )
	{
		offset += 3; // first three bytes of change req att are not used
		setChangeIpFlag((attributeValue[offset]&4)>0);
		setChangePortFlag((attributeValue[offset] & 0x2)>0);
	}

	return;
}

void CSTUNAttribute::setChangeIpFlag(bool changeIP)
{
	this->mChangeIpFlag = changeIP;
}

/**
 * Returns tha value of the changeIpFlag. The "change IP" flag,  if true,
 * requests the server to send the Binding Response with a different IP
 * address than the one the Binding Request was received on.
 *
 * @return the value of the changeIpFlag.
 */
bool CSTUNAttribute::getChangeIpFlag()
{
	return mChangeIpFlag;
}

/**
 * Sets the value of the changePortFlag. The "change port" flag.  If true,
 * requests the server to send the Binding Response with a different port
 * than the one the Binding Request was received on.
 *
 * @param changePort the new value of the changePort flag.
 */
void CSTUNAttribute::setChangePortFlag(bool changePort)
{
	this->mChangePortFlag = changePort;
}

/**
 * Returns the value of the changePortFlag. The "change port" flag.  If true,
 * requests the server to send the Binding Response with a different port
 * than the one the Binding Request was received on.
 *
 * @return the value of the changePort flag.
 */
bool CSTUNAttribute::getChangePortFlag()
{
	return mChangePortFlag;
}

/**
 * Returns the result of applying XOR on the specified attribute's address.
 * The method may be used for both encoding and decoding XorMappedAddresses.
 * @param address the address on which XOR should be applied
 * @param transactionID the transaction id to use for the XOR
 * @return the XOR-ed address.
 */
CSTUNAddress CSTUNAttribute::applyXor(CSTUNAddress & address, CSTUNTransactionID & transactionID)
{
	CUtilByteArray & addressBytes = address.getAddressBytes();
	word port = address.getPort();

	word portModifier = (word)( (transactionID[1] << 8 & 0x0000FF00) | (transactionID[0] & 0x000000FF));

	port ^= portModifier;

	byte * xorAddressBytes = new byte[addressBytes.length()];

	for(dword i = 0; i < addressBytes.length(); i++)
		xorAddressBytes[i] = addressBytes[i] ^ transactionID[i];

	CSTUNAddress xoredAdd(xorAddressBytes, (byte)addressBytes.length(), port);

	delete xorAddressBytes;
	delete & addressBytes;
	
	return xoredAdd;
}


/**
 * Returns the length of this attribute's body.
 * @return the length of this attribute's value (8 bytes).
 */
void CSTUNAttribute::Trace()
{
	if (getAttributeType() == MAPPED_ADDRESS   ||
		getAttributeType() == RESPONSE_ADDRESS ||
		getAttributeType() == SOURCE_ADDRESS   ||
		getAttributeType() == CHANGED_ADDRESS  ||
		getAttributeType() == REFLECTED_FROM   ||
		getAttributeType() == XOR_MAPPED_ADDRESS )
	{
		CUtilByteArray & addr =  mAddress.getAddressBytes();
		mTrace->Trace("**DEBUG**", __FUNCTION__, "---------------------------");
		mTrace->Trace("**DEBUG**", __FUNCTION__, "%s", (const char *)mName);
		mTrace->Trace("**DEBUG**", __FUNCTION__, "%d.%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3], mAddress.getPort());
		delete & addr;
	}
	else if ( getAttributeType() == CHANGE_REQUEST )
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "---------------------------");
		mTrace->Trace("**DEBUG**", __FUNCTION__, "%s", (const char *)mName);
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Change IP: %d Change Port: %d", (int)mChangeIpFlag, (int)mChangePortFlag);
	}
	else
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "---------------------------");
		mTrace->Trace("**DEBUG**", __FUNCTION__, "%s", (const char *)mName);
	}
	
	return;
}


