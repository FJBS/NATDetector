/*
 *  UtilString.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 17/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */
 
#pragma once

//! \class CUtilString
//! \brief A class for handing string texts

class CUtilString
{
public:

	///////////////////////////////////////////////////////////////////////////
	// Constructors and destructor

	//! constructor for an empty string
	CUtilString ();

	//! constructor from a char 
	CUtilString (const char text);

	//! constructor from a char array
	CUtilString (const char * text);

	//! copy constructor
	CUtilString (const CUtilString & other);

	//! Destructor
	virtual ~CUtilString ();

	///////////////////////////////////////////////////////////////////////////
	// operators

	bool operator == (const CUtilString &other) const;			//!< Comparation operator
	bool operator == (const char * txt) const;				//!< Comparation operator
	bool operator != (const CUtilString &other) const;			//!< Comparation operator
	bool operator != (const char * txt) const;				//!< Comparation operator
	bool operator <= (const CUtilString &other) const;			//!< Comparation operator
	bool operator <= (const char * txt) const;				//!< Comparation operator
	bool operator >= (const CUtilString &other) const;			//!< Comparation operator
	bool operator >= (const char * txt) const;				//!< Comparation operator
	bool operator <  (const CUtilString &other) const;			//!< Comparation operator
	bool operator <  (const char * txt) const;				//!< Comparation operator
	bool operator >  (const CUtilString &other) const;			//!< Comparation operator
	bool operator >  (const char * txt) const;

	CUtilString & operator = (const CUtilString & other);			//!< Assignement operator
	CUtilString & operator = (const char * text);				//!< Assignement operator
	CUtilString & operator = (const char c);					//!< Assignement operator

	CUtilString operator + (const CUtilString & other) const;		//!< operator +
	CUtilString operator + (const char * text) const;			//!< operator +
	CUtilString operator + (const char c) const;				//!< operator +
	CUtilString & operator += (const CUtilString & other);			//!< operator +=
	CUtilString & operator += (const char * text);				//!< operator +=
	CUtilString & operator += (const char c);					//!< operator +=

	///////////////////////////////////////////////////////////////////////////
	// extractors

	operator const char * () const;							//!< const char* extractor

	///////////////////////////////////////////////////////////////////////////
	// methods

	const char * c_str() const;								//!< get a pointer to the character array of the string

	void empty();											//!< empty the contents of the string

	//! find the first ocurrence of a character in the string
	/*!	returns the position if the character is found, -1 if the character is not found */
	dword find_first_of(char c, dword inipos = 0) const;

	//! find the last ocurrence of a character in the string
	/*!	returns the position if the character is found, -1 if the character is not found */
	dword find_last_of(char c) const;

	//! check if the string content is a well formatted integer number
	bool is_integer() const;

	bool is_null() const;									//!< test if the string content is a null pointer

	dword length () const;									//!< returns the length of the string

	//! check if there are more tokens in the string
	/*! This function checks if there are more tokens in the string starting at the pos
		position. It can be used in combination with the tokenize function. The tokens are 
		the string chunks between sep characters. 
		\param[in] sep Character separating the tokens
		\param[in] pos Starting position to look up for tokens
	*/
	bool more_tokens(char sep, dword & pos) const;

	//! get the next token from the string
	CUtilString tokenize(char sep, dword & pos) const;

	//! 
	CUtilString replace (const char * to_replace, const char * replacer) const;

	//!
	CUtilString substr(dword ini, dword num = 0xFFFF) const;

	//!
	CUtilString toupper() const;

	//!
	CUtilString trim() const;

	///////////////////////////////////////////////////////////////////////////
	// static methods

	//! compare two strings 
	/*! This function compare two CStrings ignoring case
		See the strcmp function in the C documentacion for more information
	*/
	static int compare (const CUtilString & text1, const CUtilString & text2);

	//! compare the first len characters of two strings 
	/*! This function compare the first len characters of two strings 
		See the strncmp function in the C documentacion for more information
	*/
	static int comparen (const CUtilString & text1, const CUtilString & text2, dword len);

	//! build an CUtilString from a list of variables similarly to sprintf function
	static CUtilString format (const char* control, ...);

	//! build an CUtilString with a specified number of space characters
	static CUtilString space(int num);	

private:
	char * mtext;					//!< the pointer to the contents of the string

};
