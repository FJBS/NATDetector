/*
 *  UtilString.cpp
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 17/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include "UtilDefs.h"

CUtilString::CUtilString()
{
	mtext = NULL;
}

CUtilString::~CUtilString()
{
	delete []this->mtext;
}

CUtilString::CUtilString(const char text)
{
	this->mtext = new char[2];
	this->mtext[0] = text;
	this->mtext[1] = '\0';
}

CUtilString::CUtilString(const char * text)
{
	if (text != NULL)
	{
		this->mtext = new char[strlen(text)+1];
		strcpy (this->mtext, text);
	}
	else
		this->mtext = NULL;
}

CUtilString::CUtilString (const CUtilString & other)
{
	if (other.mtext != NULL)
	{
		this->mtext = new char[strlen(other.mtext)+1];
		strcpy (this->mtext, other.mtext);
	}
	else
		this->mtext = NULL;
}

bool CUtilString::operator == (const CUtilString &other) const
{
	if (this->mtext == NULL  &&  other.mtext == NULL)
		return true;
	if (this->mtext == NULL  &&  other.mtext != NULL)
		return false;
	if (this->mtext != NULL  &&  other.mtext == NULL)
		return false;
	return strcmp (this->mtext, other.mtext) == 0;
}

bool CUtilString::operator == (const char* text) const
{
	return (*this == CUtilString(text));
}

bool CUtilString::operator != (const CUtilString &other) const
{
	return ! (*this == other);
}

bool CUtilString::operator != (const char* text) const
{
	return ! (*this == CUtilString(text));
}

bool CUtilString::operator >= (const CUtilString &other) const
{
	if (this->mtext == NULL  &&  other.mtext == NULL)
		return true;
	if (this->mtext == NULL  &&  other.mtext != NULL)
		return false;
	if (this->mtext != NULL  &&  other.mtext == NULL)
		return true;
	return strcmp (this->mtext, other.mtext) >= 0;
}

bool CUtilString::operator >= (const char* text) const
{
	return (*this >= CUtilString(text));
}

bool CUtilString::operator <= (const CUtilString &other) const
{
	if (this->mtext == NULL  &&  other.mtext == NULL)
		return true;
	if (this->mtext == NULL  &&  other.mtext != NULL)
		return true;
	if (this->mtext != NULL  &&  other.mtext == NULL)
		return false;
	return strcmp (this->mtext, other.mtext) <= 0;
}

bool CUtilString::operator <= (const char* text) const
{
	return (*this <= CUtilString(text));
}

bool CUtilString::operator >  (const CUtilString &other) const
{
	if (this->mtext == NULL  &&  other.mtext == NULL)
		return false;
	if (this->mtext == NULL  &&  other.mtext != NULL)
		return false;
	if (this->mtext != NULL  &&  other.mtext == NULL)
		return true;
	return strcmp (this->mtext, other.mtext) > 0;
}

bool CUtilString::operator >  (const char* text) const
{
	return (*this > CUtilString(text));
}

bool CUtilString::operator < (const CUtilString &other) const
{
	if (this->mtext == NULL  &&  other.mtext == NULL)
		return false;
	if (this->mtext == NULL  &&  other.mtext != NULL)
		return true;
	if (this->mtext != NULL  &&  other.mtext == NULL)
		return false;
	return strcmp (this->mtext, other.mtext) < 0;
}

bool CUtilString::operator <  (const char* text) const
{
	return (*this < CUtilString(text));
}

CUtilString & CUtilString::operator = (const CUtilString & other)
{
	if (this == &other)
		return *this;

	delete []this->mtext;
	if (other.mtext != NULL)
	{
		this->mtext = new char[strlen(other.mtext)+1];
		strcpy (this->mtext, other.mtext);
	}
	else
		this->mtext = NULL;
	return *this;
}

CUtilString & CUtilString::operator = (const char * text)
{		
	*this = CUtilString(text);
	return *this;
}

CUtilString & CUtilString::operator = (const char text)
{
	*this = CUtilString(text);
	return *this;
}

CUtilString CUtilString::operator + (const CUtilString & other) const
{
	CUtilString str;

	if (this->mtext == NULL  &&  other.mtext == NULL)
		return str;
	if (this->mtext != NULL  &&  other.mtext == NULL)
		return *this;
	if (this->mtext == NULL  &&  other.mtext != NULL)
		return other;
	str.mtext = new char[strlen(this->mtext) + strlen(other.mtext) + 1];
	strcpy(str.mtext, this->mtext);
	strcat(str.mtext, other.mtext);
	return str;
}

CUtilString CUtilString::operator + (const char * text) const 
{
	return *this + CUtilString(text);
}

CUtilString CUtilString::operator + (const char text) const 
{
	return *this + CUtilString(text);
}

CUtilString & CUtilString::operator += (const CUtilString & other)
{
	*this = *this + other;

	return *this;
}

CUtilString & CUtilString::operator += (const char * text)
{
	*this = *this + CUtilString(text);

	return *this;
}

CUtilString & CUtilString::operator += (const char text)
{
	*this = *this + CUtilString(text);

	return *this;
}

CUtilString::operator const char* () const
{
	if (this->mtext != NULL)
		return this->mtext;
	else
		return "";
}

const char* CUtilString::c_str() const
{
	if (this->mtext != NULL)
		return this->mtext;
	else
		return "";
}

void CUtilString::empty()
{
	delete []this->mtext;
	this->mtext = NULL;
}

dword CUtilString::find_first_of(char c, dword inipos) const
{
	dword pos;
	char* p;

	if (this->mtext != NULL)
	{
		p = strchr(this->mtext+inipos, c);
		if (p != NULL)
			pos = (dword)(p - this->mtext);
		else
			pos = -1;
	}
	else
		pos = -1;

	return pos;
}

dword CUtilString::find_last_of(char c) const
{
	dword pos;
	char* p;

	if (this->mtext != NULL)
	{
		p = strrchr(this->mtext, c);
		if (p != NULL)
			pos = (dword)(p - this->mtext);
		else
			pos = -1;
	}
	else
		pos = -1;

	return pos;
}

bool CUtilString::is_null() const
{
	return this->mtext == NULL;
}

bool CUtilString::is_integer() const
{
	int 	pos    = 0;
	bool	digits = false;	
	int		length;

	if (mtext == NULL)
		length = 0;
	else 
		length = (int)strlen(mtext);

	if (length == 0)
		return false;

	// Se examina la existencia de signo positivo o negativo
	if (mtext[0] == '-'  ||  mtext[0] == '+')
	{
	   pos = 1;
	}
	
	// Se examinan el resto de las cifras
	while (pos < length)
	{
		if (strchr ("0123456789", mtext[pos]) != 0)
			digits = true;
		else if (mtext[pos] == ' ')		// El n˙mero ha terminado
			break;		 		
		else
			return false;
		pos = pos + 1;
	}
	
	if (! digits)						// Si no ha habido dÌgitos
		return false;
	
	if (pos < length)					// Si despues de blanco hay mas caracteres
		return false;

	return true;
}

dword CUtilString::length() const
{
	if (this->mtext == NULL)
		return 0;
	else
		return (dword) strlen(mtext);
}

bool CUtilString::more_tokens(char sep, dword & pos) const
{
	bool res;
	size_t len;

	if (this->mtext != NULL)
	{
		len = strlen(this->mtext);
		res = (pos < len) ? true : false;
	}
	else
		res = false;

	return res;
}

CUtilString CUtilString::replace(const char * to_replace, const char * replacer) const
{
	size_t count;
	int num;
	size_t len_to_replace = strlen(to_replace);
	size_t len_replacer = strlen(replacer);
	char *p;
	const char *p_ini;
	char *p_end;
	CUtilString result;

	if (mtext != NULL  &&  to_replace != NULL  &&  replacer != NULL)
	{
		count = 0;
		p = strstr(mtext, to_replace);
		while (p != NULL)
		{
			count += len_replacer - len_to_replace;
			p = strstr(p+len_to_replace, to_replace);
		}

		result.mtext = new char[strlen(mtext)+count+1];

		memset (result.mtext, 0, strlen(mtext)+count+1);

		p_ini = mtext;
		p_end = strstr (mtext, to_replace);
		while (p_end != NULL)
		{
			num = (int)(p_end - p_ini);
			strncat (result.mtext, p_ini, num);
			strncat (result.mtext, replacer, len_replacer);
			p_ini = p_end + len_to_replace;
			p_end = strstr(p_ini, to_replace);
		}
		strcat (result.mtext, p_ini);
	}

	return result;
}

CUtilString CUtilString::substr(dword ini, dword num) const
{
	size_t n;
	CUtilString str;
	if (this->mtext != NULL)
	{
		n = strlen(this->mtext);
		if (n > ini)
		{
			num = num<(dword(n - ini))?num:dword(n - ini);
			str.mtext = new char[num + 1];
			strncpy(str.mtext, this->mtext + ini, num);
			str.mtext[num] = 0;
		}
	}
	return str;
}

CUtilString CUtilString::tokenize(char sep, dword & pos) const
{
	int ini, num;
	size_t len;
	CUtilString str;

	if (this->mtext != NULL)
	{
		len = strlen(this->mtext);
		if (pos < len)
		{
			ini = pos;
			num = 0;
			while (pos < len  &&  this->mtext[pos] != sep) 
			{
				num++;
				pos++;
			}
			str = substr(ini, num);
			if (this->mtext[pos] == sep)
				pos++;
		}
		else 
		{
			str = "";
		}
	}

	return str;
}

CUtilString CUtilString::toupper() const
{
	CUtilString str;
	if (this->mtext != NULL)
	{
		str = *this;
		//strupr(str.mtext);
	}
	return str;
}

CUtilString CUtilString::trim() const
{
	CUtilString str;
	unsigned int first, last;

	if (this->mtext != NULL)
	{
		for (first = 0; first < strlen(this->mtext); first++)
			if (this->mtext[first] != ' ')
				break;
		for (last = (unsigned int)strlen(this->mtext); last > 0; last--)
			if (this->mtext[last-1] != ' ')
				break;
		if (last > first)
		{
			str.mtext = new char[last - first + 1];
			strncpy(str.mtext, &this->mtext[first], last-first);
			str.mtext[last-first] = 0;
		}
	}

	return str;
}

int CUtilString::compare (const CUtilString & text1, const CUtilString & text2)
{
	if (text1.mtext == NULL  &&  text2.mtext == NULL)
		return 0;
	if (text1.mtext == NULL  &&  text2.mtext != NULL)
		return -1;
	if (text1.mtext != NULL  &&  text2.mtext == NULL)
		return 1;
	return strcmp (text1.mtext, text2.mtext);
}

int CUtilString::comparen (const CUtilString & text1, const CUtilString & text2, dword len)
{
	if (text1.mtext == NULL  &&  text2.mtext == NULL)
		return 0;
	if (text1.mtext == NULL  &&  text2.mtext != NULL)
		return -1;
	if (text1.mtext != NULL  &&  text2.mtext == NULL)
		return 1;
	return strncmp (text1.mtext, text2.mtext, len);
}


CUtilString CUtilString::format(const char* control, ...)
{
	int len;
	va_list		pArg;
	CUtilString    str;

	if (control != NULL)
	{
		va_start(pArg, control);
		len = vprintf(control, pArg);
		str.mtext = new char[len+1];
		vsprintf(str.mtext, control, pArg);
		va_end(pArg);
	}

	return str;
}

CUtilString CUtilString::space(int num)
{
	CUtilString str;
	str.mtext = new char[num+1];
	memset(str.mtext, ' ', num);
	str.mtext[num] = 0;
	return str;
}

