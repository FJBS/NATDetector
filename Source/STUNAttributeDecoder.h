/*
 *  STUNAttributeDecoder.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 16/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

class CSTUNAttributeDecoder
{
public:
	/**
	 * Decodes the specified binary array and returns the corresponding
	 * attribute object.
	 * @param bytes the binary array that should be decoded.
	 * @param offset the index where the message starts.
	 * @param length the number of bytes that the message is long.
	 * @return An object representing the attribute encoded in bytes or null if
	 *         the attribute was not recognized.
	 * @throws StunException if bytes does is not a valid STUN attribute.
	 */
	static CSTUNAttribute & decode(CUtilTrace *Trace, byte *bytes, byte *offset, word length); //	throws StunException

};
