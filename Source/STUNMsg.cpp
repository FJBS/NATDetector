/*
 *  STUN.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 09/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "STUNDefs.h"

const byte CSTUNMsg::HEADER_LENGTH					= 20;

const word CSTUNMsg::NOT_INITIALIZED				= 0x0000;
const word CSTUNMsg::BINDING_REQUEST				= 0x0001;
const word CSTUNMsg::BINDING_RESPONSE				= 0x0101;
const word CSTUNMsg::BINDING_ERROR_RESPONSE			= 0x0111;
const word CSTUNMsg::SHARED_SECRET_REQUEST			= 0x0002;
const word CSTUNMsg::SHARED_SECRET_RESPONSE			= 0x0102;
const word CSTUNMsg::SHARED_SECRET_ERROR_RESPONSE	= 0x0112;

const byte CSTUNMsg::mAttributePresentities[][15] = {
    //                                            Binding   Shared   Shared   Shared
    //                        Binding   Binding   Error     Secret   Secret   Secret
    //  Att.                  Req.      Resp.     Resp.     Req.     Resp.    Error
    //                                                                        Resp.
    //  _______________________________________________________________________
      /*MAPPED-ADDRESS*/    { N_A,      M,        N_A,      N_A,     N_A,     N_A},
      /*RESPONSE-ADDRESS*/  { O,        N_A,      N_A,      N_A,     N_A,     N_A},
      /*CHANGE-REQUEST*/    { O,        N_A,      N_A,      N_A,     N_A,     N_A},
      /*SOURCE-ADDRESS*/    { N_A,      M,        N_A,      N_A,     N_A,     N_A},
      /*CHANGED-ADDRESS*/   { N_A,      M,        N_A,      N_A,     N_A,     N_A},
      /*USERNAME*/          { O,        N_A,      N_A,      N_A,     M,       N_A},
      /*PASSWORD*/          { N_A,      N_A,      N_A,      N_A,     M,       N_A},
      /*MESSAGE-INTEGRITY*/ { O,        O,        N_A,      N_A,     N_A,     N_A},
      /*ERROR-CODE*/        { N_A,      N_A,      M,        N_A,     N_A,     M},
      /*UNKNOWN-ATTRIBUTES*/{ N_A,      N_A,      C,        N_A,     N_A,     C},
      /*REFLECTED-FROM*/    { N_A,      C,        N_A,      N_A,     N_A,     N_A},
      /*XOR-MAPPED-ADDRESS*/{ N_A,      M,        N_A,      N_A,     N_A,     N_A},
      /*XOR-ONLY*/          { O,        N_A,      N_A,      N_A,     N_A,     N_A},
      /*SERVER*/            { N_A,      O,        O,        N_A,     O,       O},
      /*UNKNOWN_OPTIONAL*/  { O,        O,        O,        O,       O,       O}};

const byte CSTUNMsg::N_A											= 0;
const byte CSTUNMsg::C												= 1;
const byte CSTUNMsg::O												= 2;
const byte CSTUNMsg::M												= 3;
	
//Message indices
const byte CSTUNMsg::BINDING_REQUEST_PRESENTITY_INDEX				= 0;
const byte CSTUNMsg::BINDING_RESPONSE_PRESENTITY_INDEX				= 1;
const byte CSTUNMsg::BINDING_ERROR_RESPONSE_PRESENTITY_INDEX		= 2;
const byte CSTUNMsg::SHARED_SECRET_REQUEST_PRESENTITY_INDEX			= 3;
const byte CSTUNMsg::SHARED_SECRET_RESPONSE_PRESENTITY_INDEX		= 4;
const byte CSTUNMsg::SHARED_SECRET_ERROR_RESPONSE_PRESENTITY_INDEX	= 5;

//Attribute indices
const byte CSTUNMsg::MAPPED_ADDRESS_PRESENTITY_INDEX				= 0;
const byte CSTUNMsg::RESPONSE_ADDRESS_PRESENTITY_INDEX				= 1;
const byte CSTUNMsg::CHANGE_REQUEST_PRESENTITY_INDEX				= 2;
const byte CSTUNMsg::SOURCE_ADDRESS_PRESENTITY_INDEX				= 3;
const byte CSTUNMsg::CHANGED_ADDRESS_PRESENTITY_INDEX				= 4;
const byte CSTUNMsg::USERNAME_PRESENTITY_INDEX						= 5;
const byte CSTUNMsg::PASSWORD_PRESENTITY_INDEX						= 6;
const byte CSTUNMsg::MESSAGE_INTEGRITY_PRESENTITY_INDEX				= 7;
const byte CSTUNMsg::ERROR_CODE_PRESENTITY_INDEX					= 8;
const byte CSTUNMsg::UNKNOWN_ATTRIBUTES_PRESENTITY_INDEX			= 9;
const byte CSTUNMsg::REFLECTED_FROM_PRESENTITY_INDEX				= 10;
const byte CSTUNMsg::XOR_MAPPED_ADDRESS_PRESENTITY_INDEX			= 11;
const byte CSTUNMsg::XOR_ONLY_PRESENTITY_INDEX						= 12;
const byte CSTUNMsg::SERVER_PRESENTITY_INDEX						= 13;
const byte CSTUNMsg::UNKNOWN_OPTIONAL_ATTRIBUTES_PRESENTITY_INDEX	= 14;

CSTUNMsg::CSTUNMsg(CUtilTrace* Trace)
{
	mTrace = Trace;
	mMessageType = NOT_INITIALIZED;
}

CSTUNMsg::CSTUNMsg(const CSTUNMsg & other)
{
	this->mAttributes = other.mAttributes;
	this->mMessageType = other.mMessageType;
	this->mTrace = other.mTrace;
	this->mTransactionID = other.mTransactionID;
}

CSTUNMsg::~CSTUNMsg()
{
	mAttributes.RemoveAll();
}

CSTUNMsg & CSTUNMsg::operator = (const CSTUNMsg & other)
{
	if (this == &other)
		return *this;

	this->mAttributes = other.mAttributes;
	this->mMessageType = other.mMessageType;
	this->mTrace = other.mTrace;
	this->mTransactionID = other.mTransactionID;

	return *this;
}

/**
 * Returns the length of this message's body.
 * @return the length of the data in this message.
 */
word CSTUNMsg::getDataLength()
{
	int length = 0;
	
	for (dword idx=0; idx < mAttributes.GetElements(); idx++)
	{
		CSTUNAttribute & att = (CSTUNAttribute &)mAttributes.GetItem(idx);
		length += att.getDataLength() + CSTUNAttribute::HEADER_LENGTH;
	}
	
	return length;
}

/**
* Adds the specified attribute to this message. If an attribute with that
 * name was already added, it would be replaced.
 * @param attribute the attribute to add to this message.
 * @throws StunException if the message cannot contain
 * such an attribute.
 */
void CSTUNMsg::addAttribute(CSTUNAttribute attribute) // throws CSTUNException
{	 
	 if (getAttributePresentity(attribute.getAttributeType()) == N_A)
		 throw new CSTUNException((CUtilString)"The attribute "
								 + attribute.getName()
								 + " is not allowed in a "
								 + getName());

	
	mAttributes.Insert(attribute);
}

/**
 * Returns the attribute with the specified type or null if no such
 * attribute exists.
 *
 * @param attributeType the type of the attribute
 * @return the attribute with the specified type or null if no such attribute
 * exists
 */
bool CSTUNMsg::getAttribute(word attributeType, CSTUNAttribute & attribute)
{
	bool ret = false;
	dword idx = 0;
	
	if ( mAttributes.FindItem(attributeType, &idx) )
	{
		attribute = mAttributes[idx];
		ret = true;
	}
	
	return ret;
}

/**
 * Removes the specified attribute.
 * @param attributeType the attribute to remove.
 */
void CSTUNMsg::removeAttribute(word attributeType)
{
	dword idx = 0;
	
	if ( mAttributes.FindItem(attributeType, &idx) )
	{
		mAttributes.Remove(idx);
	}
	
	return;
}

/**
 * Returns the number of attributes, currently contained by the message.
 * @return the number of attributes, currently contained by the message.
 */
dword CSTUNMsg::getAttributeCount()
{
	return  mAttributes.GetElements();
}

/**
 * Sets this message's type to be messageType. Method is package access
 * as it should not permit changing the type of message once it has been
 * initialized (could provoke attribute discrepancies). Called by
 * messageFactory.
 * @param messageType the message type.
 * @throws StunException ILLEGAL_ARGUMENT if message type is not valid in
 * the current context (e.g. when trying to set a Response type to a Request
 * and vice versa)
 */
void CSTUNMsg::setMessageType(word messageType)
{
	this->mMessageType = messageType;
}

/**
 * The message type of this message.
 * @return the message type of the message.
 */
word CSTUNMsg::getMessageType()
{
	return mMessageType;
}

/**
 * Copies the specified tranID and sets it as this message's transactionID.
 * @param tranID the transaction id to set in this message.
 * @throws StunException ILLEGAL_ARGUMENT if the transaction id is not valid.
 */
void CSTUNMsg::setTransactionID(CSTUNTransactionID & tranID)
{
	// if(tranID == NULL)
	//	throw new CSTUNException("Invalid transaction id");
	this->mTransactionID = tranID;
}

/**
 * Returns a reference to this message's transaction id.
 * @return a reference to this message's transaction id.
 */
 
CSTUNTransactionID CSTUNMsg::getTransactionID()
{
	return this->mTransactionID;
}

/**
 * Returns whether an attribute could be present in this message.
 * @param attributeType the id of the attribute to check .
 * @return Message.N_A - for not applicable <br/>
 *         Message.C   - for case depending <br/>
 *         Message.N_A - for not applicable <br/>
 */
byte CSTUNMsg::getAttributePresentity(word attributeType)
{
	byte msgIndex = -1;
	byte attributeIndex = -1;

	if (mMessageType == BINDING_REQUEST)
		msgIndex = BINDING_REQUEST_PRESENTITY_INDEX; 
	else if (mMessageType == BINDING_RESPONSE)            
		msgIndex = BINDING_RESPONSE_PRESENTITY_INDEX; 
	else if (mMessageType == BINDING_ERROR_RESPONSE)      
		msgIndex = BINDING_ERROR_RESPONSE_PRESENTITY_INDEX; 
	else if (mMessageType == SHARED_SECRET_REQUEST)       
		msgIndex = SHARED_SECRET_REQUEST_PRESENTITY_INDEX; 
	else if (mMessageType == SHARED_SECRET_RESPONSE)     
		msgIndex = SHARED_SECRET_RESPONSE_PRESENTITY_INDEX; 
	else if (mMessageType == SHARED_SECRET_ERROR_RESPONSE)
		msgIndex = SHARED_SECRET_ERROR_RESPONSE_PRESENTITY_INDEX; 
	
	if (attributeType == CSTUNAttribute::MAPPED_ADDRESS)   
		attributeIndex = MAPPED_ADDRESS_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::RESPONSE_ADDRESS)
		attributeIndex = RESPONSE_ADDRESS_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::CHANGE_REQUEST)    
		attributeIndex = CHANGE_REQUEST_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::SOURCE_ADDRESS)   
		attributeIndex = SOURCE_ADDRESS_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::CHANGED_ADDRESS)   
		attributeIndex = CHANGED_ADDRESS_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::USERNAME)           
		attributeIndex = USERNAME_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::PASSWORD)           
		attributeIndex = PASSWORD_PRESENTITY_INDEX;
	else if (attributeType == CSTUNAttribute::MESSAGE_INTEGRITY)  
		attributeIndex = MESSAGE_INTEGRITY_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::ERROR_CODE)        
		attributeIndex = ERROR_CODE_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::UNKNOWN_ATTRIBUTES)
		attributeIndex = UNKNOWN_ATTRIBUTES_PRESENTITY_INDEX; 
	else if (attributeType ==  CSTUNAttribute::REFLECTED_FROM)     
		attributeIndex = REFLECTED_FROM_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::XOR_MAPPED_ADDRESS)
		attributeIndex = XOR_MAPPED_ADDRESS_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::XOR_ONLY)         
		attributeIndex = XOR_ONLY_PRESENTITY_INDEX; 
	else if (attributeType == CSTUNAttribute::SERVER)            
		attributeIndex = SERVER_PRESENTITY_INDEX; 
	else
		attributeIndex = UNKNOWN_OPTIONAL_ATTRIBUTES_PRESENTITY_INDEX; 

	return mAttributePresentities[ attributeIndex ][ msgIndex ];
}


/**
 * Verify that the message has all obligatory attributes and throw an
 * exception if this is not the case.
 *
 * @return true if the message has all obligatory attributes, false
 * otherwise.
 * @throws StunException (ILLEGAL_STATE)if the message does not have all
 * required attributes.
 */
bool CSTUNMsg::validateAttributePresentity() 
{
	bool ret = true;
	
	CSTUNAttribute attOther(mTrace);
	for(word i = CSTUNAttribute::MAPPED_ADDRESS; i < CSTUNAttribute::REFLECTED_FROM; i++)
	{
		if(getAttributePresentity(i) == M && getAttribute(i, attOther) == false)
		{
			ret = false;
			break;
		}
	}
			
	return ret;
}

/**
 * Returns the human readable name of this message. Message names do
 * not really matter from the protocol point of view. They are only used
 * for debugging and readability.
 * @return this message's name.
 */
CUtilString CSTUNMsg::getName()
{
	switch (mMessageType)
	{
		case BINDING_REQUEST:              return CUtilString("BINDING-REQUEST");
		case BINDING_RESPONSE:             return CUtilString("BINDING-RESPONSE");
		case BINDING_ERROR_RESPONSE:       return CUtilString("BINDING-ERROR-RESPONSE");
		case SHARED_SECRET_REQUEST:        return CUtilString("SHARED-SECRET-REQUEST");
		case SHARED_SECRET_RESPONSE:       return CUtilString("SHARED-SECRET-RESPONSE");
		case SHARED_SECRET_ERROR_RESPONSE: return CUtilString("SHARED-SECRET-ERROR-RESPONSE");
	}

	return CUtilString("UNKNOWN-MESSAGE");
}

/**
 * Compares two STUN Messages. Messages are considered equal when their
 * type, length, and all their attributes are equal.
 *
 * @param obj the object to compare this message with.
 * @return true if the messages are equal and false otherwise.
 */
 
bool CSTUNMsg::equals(CSTUNMsg & other)
{
	bool ret = true;
	CSTUNAttribute attOther(mTrace);
	
	if( other.getMessageType()   != getMessageType())
		ret = false;
		
	if ( ret )
		if(other.getDataLength() != getDataLength())
			ret = false;

	if ( ret )
		if ( getAttributeCount() != other.getAttributeCount() )
			ret = false;
			
	//compare attributes
	if ( ret )
	{
		for (dword idx=0; idx < mAttributes.GetElements(); idx++)
		{
			CSTUNAttribute & att = (CSTUNAttribute &)mAttributes.GetItem(idx);
			ret = other.getAttribute(att.getAttributeType(), attOther);
			if ( !ret )
				break;	
		}
	}
	

	return ret;
}

/**
 * Determines whether type could be the type of a STUN Response (as opposed
 * to STUN Request).
 * @param type the type to test.
 * @return true if type is a valid response type.
 */
bool CSTUNMsg::isResponseType(word type)
{
	return (((type >> 8) & 1) != 0);
}

/**
 * Determines whether type could be the type of a STUN Request (as opposed
 * to STUN Response).
 * @param type the type to test.
 * @return true if type is a valid request type.
 */
bool CSTUNMsg::isRequestType(word type)
{
	return !isResponseType(type);
}

CUtilString CSTUNMsg::toString()
{
	CUtilString ret = "";
	
	/*
	ret  = getName();
	ret += "(";
	ret += getMessageType();
	ret += ")[attrib.count=";
	ret += getAttributeCount();
	ret += " len=";
	ret += getDataLength();
	ret += " tranID=";
	ret += getTransactionID();
	ret += "]";
	*/
	
	return ret;
}


/**
 * Returns a binary representation of this message.
 * @return a binary representation of this message.
 * @throws StunException if the message does not have all required
 * attributes.
 */
bool CSTUNMsg::encode(byte** binMsg, dword *length) 
{
	bool	ret = true;
	int		lengthAtt = 0;
	byte*	attBinValue;
	ret = validateAttributePresentity();
	
	*binMsg = NULL;
	*length = 0;
	
	if ( ret )
	{
		word dataLength = getDataLength();
		
		*binMsg = new byte[HEADER_LENGTH + dataLength];

		(*binMsg)[(*length)++] = (byte)(getMessageType()>>8);
		(*binMsg)[(*length)++] = (byte)(getMessageType()&0xFF);

		(*binMsg)[(*length)++] = (byte)(dataLength >> 8);
		(*binMsg)[(*length)++] = (byte)(dataLength & 0xFF);
		
		memcpy((*binMsg)+(*length), mTransactionID, CSTUNTransactionID::TRANSACTION_ID_LENGTH);
		(*length)+=CSTUNTransactionID::TRANSACTION_ID_LENGTH;
			
		for (dword idx=0; idx < mAttributes.GetElements(); idx++)
		{
			CSTUNAttribute & att = (CSTUNAttribute &)mAttributes.GetItem(idx);
			attBinValue=NULL;
			att.encode(&attBinValue, &lengthAtt);
			memcpy((*binMsg)+(*length), attBinValue, lengthAtt);
			(*length) += lengthAtt;
			delete attBinValue;
		}
	}
	
	return ret;
}

bool CSTUNMsg::encode(CUtilByteArray & msg)
{
	bool	ret = true;
	
	byte	*msgAux = NULL;
	dword	length = 0;
	
	ret = encode(&msgAux, &length);
	
	if ( ret )
	{
		CUtilByteArray * auxByteArray = new CUtilByteArray(msgAux, length);
		msg = *auxByteArray;
		delete auxByteArray;
	}

	delete msgAux;
	
	return ret;
}

/**
 * Constructs a message from its binary representation.
 * @param binMessage the binary array that contains the encoded message
 * @param offset the index where the message starts.
 * @param arrayLen the length of the message
 * @return a Message object constructed from the binMessage array
 * @throws StunException ILLEGAL_ARGUMENT if one or more of the arguments
 * have invalid values.
 */
CSTUNMsg & CSTUNMsg::decode(const CUtilTrace *Trace, const byte* binMessage, byte offset, dword arrayLen) // throws StunException
{
	CSTUNMsg*			message = NULL;
	CSTUNTransactionID	tranIDAux;
	byte*				tranID;
	
	if(binMessage == NULL || arrayLen - offset < CSTUNMsg::HEADER_LENGTH)
		throw new CSTUNException("The given binary array is not a valid CSTUNMsg");
	
	word messageType = ntohs((word)(*((word*)(binMessage+offset))));
	offset+=2;
	
	if (CSTUNMsg::isResponseType(messageType))
	{
		message = new CSTUNResponse((CUtilTrace*)Trace);
	}
	else
	{
		message = new CSTUNRequest((CUtilTrace*)Trace);
	}
	message->setMessageType(messageType);

	word lengthAux = ntohs((word)(*((word*)(binMessage+offset))));
	offset+=2;

	if(arrayLen - offset - CSTUNTransactionID::TRANSACTION_ID_LENGTH < lengthAux)
		throw new CSTUNException("The given binary array does not seem to contain a whole CSTUNMsg");

	tranID = new byte[CSTUNTransactionID::TRANSACTION_ID_LENGTH];
	memcpy(tranID, binMessage+offset, CSTUNTransactionID::TRANSACTION_ID_LENGTH);
	tranIDAux = tranID;
	message->setTransactionID(tranIDAux);
	offset+=CSTUNTransactionID::TRANSACTION_ID_LENGTH;
	delete tranID;
	
	while(offset < HEADER_LENGTH + lengthAux)
	{
		CSTUNAttribute & att = CSTUNAttributeDecoder::decode((CUtilTrace*)Trace, (byte*)binMessage, &offset, HEADER_LENGTH + lengthAux);
		message->addAttribute(att);
		delete & att;
	}

	return *message;
}


CSTUNMsg & CSTUNMsg::decode(const CUtilTrace *Trace, const CUtilByteArray & msg, byte offset)
{
	return decode(Trace, (const byte *)msg, offset, msg.length());
}

CSTUNMsg & CSTUNMsg::decode(const CUtilTrace *Trace, const CUtilByteArray * msg, byte offset)
{
	CUtilByteArray aux(*msg);
	return decode(Trace, (const byte *)aux, offset, msg->length());
}

void CSTUNMsg::Trace()
{
	mTrace->Trace("**DEBUG**", __FUNCTION__, "****************************");
	mTrace->Trace("**DEBUG**", __FUNCTION__, "%s", (const char *)getName());	
	for (dword idx=0; idx < mAttributes.GetElements(); idx++)
	{
		CSTUNAttribute & att = (CSTUNAttribute &)mAttributes.GetItem(idx);
		att.Trace();	
	}
	mTrace->Trace("**DEBUG**", __FUNCTION__, "****************************");
}
