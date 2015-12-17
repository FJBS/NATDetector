/*
 *  STUN.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 09/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once
 
class CSTUNMsg
{
public:

	static const word NOT_INITIALIZED;
	static const word BINDING_REQUEST;
	static const word BINDING_RESPONSE;
	static const word BINDING_ERROR_RESPONSE;
	static const word SHARED_SECRET_REQUEST;
	static const word SHARED_SECRET_RESPONSE;
	static const word SHARED_SECRET_ERROR_RESPONSE;

	CSTUNMsg(CUtilTrace* Trace);
	CSTUNMsg(const CSTUNMsg & other);
	
	virtual ~CSTUNMsg();
	
	word				getDataLength();
	void				addAttribute(CSTUNAttribute attribute);
	bool				getAttribute(word attributeType, CSTUNAttribute & attribute);
	void				removeAttribute(word attributeType);
	dword				getAttributeCount();
	word				getMessageType();
	void				setMessageType(word messageType);
	void				setTransactionID(CSTUNTransactionID & tranID);
	CSTUNTransactionID  getTransactionID();
	CUtilString			getName();
	bool				equals(CSTUNMsg & other);
	static bool			isResponseType(word type);
	static bool			isRequestType(word type);

	bool				encode(CUtilByteArray & msg);
	static CSTUNMsg &	decode(const CUtilTrace *Trace, const CUtilByteArray & msg, byte offset); 
	static CSTUNMsg &	decode(const CUtilTrace *Trace, const CUtilByteArray * msg, byte offset); 

	CUtilString			toString();
	
	void				Trace();

	CSTUNMsg & operator = (const CSTUNMsg & other);

protected:
	byte				getAttributePresentity(word attributeType);
	bool				validateAttributePresentity();

	CUtilTrace*	mTrace;
	
	/**
	 * The length of Stun Message Headers in byres
	 * = len(Type) + len(DataLength) + len(Transaction ID).
	 */
	static const byte HEADER_LENGTH;

    /**
     * Indicates the type of the message. The message type can be Binding Request,
     * Binding Response, Binding Error Response, Shared Secret Request, Shared
     * Secret Response, or Shared Secret Error Response.
     */
	word	mMessageType;
	
	/**
	 * The transaction ID is used to correlate requests and responses.
	 */
    CSTUNTransactionID	mTransactionID;

    /**
     * The list of attributes contained by the message. We are using a hastable
     * rather than a uni-dimensional list, in order to facilitate attribute
     * search (even though it introduces some redundancies). Order is important
     * so we'll be using a LinkedHashMap
     */
    CIdTbl<CSTUNAttribute,dword> mAttributes;
	
	/**
	 * Desribes which attributes are present in which messages.  An
	 * M indicates that inclusion of the attribute in the message is
	 * mandatory, O means its optional, C means it's conditional based on
	 * some other aspect of the message, and N/A means that the attribute is
	 * not applicable to that message type.
	 *
	 *
	 *                                         Binding  Shared  Shared  Shared
	 *                       Binding  Binding  Error    Secret  Secret  Secret
	 *   Att.                Req.     Resp.    Resp.    Req.    Resp.   Error
	 *                                                                  Resp.
	 *   _____________________________________________________________________
	 *   MAPPED-ADDRESS      N/A      M        N/A      N/A     N/A     N/A
	 *   RESPONSE-ADDRESS    O        N/A      N/A      N/A     N/A     N/A
	 *   CHANGE-REQUEST      O        N/A      N/A      N/A     N/A     N/A
	 *   SOURCE-ADDRESS      N/A      M        N/A      N/A     N/A     N/A
	 *   CHANGED-ADDRESS     N/A      M        N/A      N/A     N/A     N/A
	 *   USERNAME            O        N/A      N/A      N/A     M       N/A
	 *   PASSWORD            N/A      N/A      N/A      N/A     M       N/A
	 *   MESSAGE-INTEGRITY   O        O        N/A      N/A     N/A     N/A
	 *   ERROR-CODE          N/A      N/A      M        N/A     N/A     M
	 *   UNKNOWN-ATTRIBUTES  N/A      N/A      C        N/A     N/A     C
	 *   REFLECTED-FROM      N/A      C        N/A      N/A     N/A     N/A
	 *   XOR-MAPPED-ADDRESS  N/A      M        N/A      N/A     N/A     N/A
	 *   XOR-ONLY            O        N/A      N/A      N/A     N/A     N/A
	 *   SERVER              N/A      O        O        N/A     O       O
	 *
	 */
	static const byte N_A;
	static const byte C;
	static const byte O;
	static const byte M;
	static const byte mAttributePresentities[][15];
	
	//Message indices
	static const byte BINDING_REQUEST_PRESENTITY_INDEX;
	static const byte BINDING_RESPONSE_PRESENTITY_INDEX;
	static const byte BINDING_ERROR_RESPONSE_PRESENTITY_INDEX;
	static const byte SHARED_SECRET_REQUEST_PRESENTITY_INDEX;
	static const byte SHARED_SECRET_RESPONSE_PRESENTITY_INDEX;
	static const byte SHARED_SECRET_ERROR_RESPONSE_PRESENTITY_INDEX;
 
	//Attribute indices
	static const byte MAPPED_ADDRESS_PRESENTITY_INDEX;
	static const byte RESPONSE_ADDRESS_PRESENTITY_INDEX;
	static const byte CHANGE_REQUEST_PRESENTITY_INDEX;
	static const byte SOURCE_ADDRESS_PRESENTITY_INDEX;
	static const byte CHANGED_ADDRESS_PRESENTITY_INDEX;
	static const byte USERNAME_PRESENTITY_INDEX;
	static const byte PASSWORD_PRESENTITY_INDEX;
	static const byte MESSAGE_INTEGRITY_PRESENTITY_INDEX;
	static const byte ERROR_CODE_PRESENTITY_INDEX;
	static const byte UNKNOWN_ATTRIBUTES_PRESENTITY_INDEX;
	static const byte REFLECTED_FROM_PRESENTITY_INDEX;
	static const byte XOR_MAPPED_ADDRESS_PRESENTITY_INDEX;
	static const byte XOR_ONLY_PRESENTITY_INDEX;
	static const byte SERVER_PRESENTITY_INDEX;
	static const byte UNKNOWN_OPTIONAL_ATTRIBUTES_PRESENTITY_INDEX;

	bool				encode(byte **binMsg, dword *length);
	static CSTUNMsg &	decode(const CUtilTrace *Trace, const byte* binMessage, byte offset, dword arrayLen); 

};
