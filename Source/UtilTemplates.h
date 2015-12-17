/*
 *  UtilTemplates.h
 *  NATDetector
 *
 *  Created by Francisco José Blázquez Sánchez on 02/11/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#pragma once

#include "UtilDefs.h"

/************************************************************************/
/* CQueue                                                               */
/************************************************************************/
template <class TYPE> 
class CQueue
{
public:    
	CQueue (void);
	CQueue (const CQueue & other);
    virtual ~CQueue (void);    

	CQueue & operator = (const CQueue & other);

	void Insert (const TYPE & Data);
	bool Get (TYPE &Data);
	void RemoveAll ();
	bool Elements (dword* Num_Elements) const;
	dword GetElements () const;

protected:

	struct TQueue
	{
		TYPE*	pData;
		TQueue* Prev;
		TQueue* Next;
	};

	CUtilCriticalSection	m_CriticalSection;
	TQueue* 			m_First;
	TQueue* 			m_Last;
	dword				m_Elements;
};

template <class TYPE> 
CQueue<TYPE>::CQueue (void)
{
	m_First = NULL;
	m_Last = NULL;
	m_Elements = 0;	
}

template <class TYPE> 
CQueue<TYPE>::~CQueue (void)
{
	RemoveAll();
}

template <class TYPE> 
void CQueue<TYPE>::Insert (const TYPE & Data)
{
	TQueue* pQueue;
	
	m_CriticalSection.Enter();
	
	pQueue = m_Last;
	m_Last = new TQueue;
	m_Last->pData = new TYPE(Data);
	m_Last->Prev = pQueue;
	m_Last->Next = NULL;
	if (pQueue != NULL)
	{
		pQueue->Next = m_Last;
	}
	else
	{
		m_First = m_Last;
	}
	
	m_Elements++;

	m_CriticalSection.Leave();
}

template <class TYPE> 
bool CQueue<TYPE>::Get (TYPE &Data) 
{
	bool 	Result;
	TQueue* pQueue;
	
	m_CriticalSection.Enter();
	
	pQueue = m_First;
	if (pQueue != NULL)
	{
		m_First = pQueue->Next;
		if (m_First != NULL)
		{
			m_First->Prev = NULL;
		}
		else
		{
			m_Last = NULL;
		}
		Data = *(pQueue->pData);
		delete pQueue->pData;
		delete pQueue;
		m_Elements--;
		Result = true;
	}
	else
	{
		Result = false;
	}

	m_CriticalSection.Leave();
	
	return Result;
}

template <class TYPE> 
void CQueue<TYPE>::RemoveAll ()
{
	TQueue* pQueue;
	
	m_CriticalSection.Enter();
	
	while (m_First != NULL)
	{
		pQueue = m_First;
		m_First = pQueue->Next;
		if (m_First != NULL)
		{
			m_First->Prev = NULL;
		}
		else
		{
			m_Last = NULL;
		}
		delete pQueue->pData;
		delete pQueue;
		m_Elements--;
	}
	
	m_CriticalSection.Leave();
}

// Retrieves the number of elements in the queue
template <class TYPE>
bool CQueue<TYPE>::Elements(dword* Num_Elements) const
{
	m_CriticalSection.Enter();
	*Num_Elements = m_Elements;
	m_CriticalSection.Leave();
	return true;
}

// Retrieves the number of elements in the queue
template <class TYPE>
dword CQueue<TYPE>::GetElements() const
{
	dword NumElements;
	m_CriticalSection.Leave();
	NumElements = m_Elements;
	m_CriticalSection.Leave();
	return NumElements;
}

/************************************************************************/
/* CArray                                                               */
/************************************************************************/

static const int ArraySizeStep = 50;

// Class definition
template <class TYPE>
class CArray
{

public:
	CArray ();
	CArray(const CArray & other);
	virtual ~CArray ();

	CArray & operator = (const CArray & other);
	TYPE & operator[] (dword Idx) const;

	bool Add			(const TYPE & Item);
	bool Add			(dword Idx, const TYPE & Item);

	bool Elements		(dword * Num_Elements) const;
	dword GetElements	() const;

	const TYPE & GetItem(dword Idx) const;
	bool GetItem		(dword Idx, TYPE * Item) const;

	bool Modify			(dword Idx, const TYPE & Item);

	bool Remove			(dword Idx);
	bool RemoveAll		();

protected:
	CUtilCriticalSection	m_CriticalSection;
	TYPE**				m_Tbl;
	dword				m_Elements;

private:
	void				IncreaseSize ();
	dword				m_Size;
};

// Constructor
template <class TYPE>
CArray<TYPE>::CArray ()
{
	m_Tbl = NULL;
	m_Size = 0;
	m_Elements = 0;	
}

// Copy constructor
template <class TYPE>
CArray<TYPE>::CArray(const CArray<TYPE> & other)
{
	m_Tbl = NULL;
	m_Size = 0;
	m_Elements = 0;	
	*this = other;
}

// Destructor
// Keeps aware of destroying all elements in the array
template <class TYPE>
CArray<TYPE>::~CArray ()
{
	m_CriticalSection.Enter();
	RemoveAll();
	delete []m_Tbl;
	m_CriticalSection.Leave();
}

template <class TYPE>
CArray<TYPE> & CArray<TYPE>::operator = (const CArray<TYPE> & other)
{
	// First, delete old contents
	RemoveAll();
	delete []m_Tbl;
	m_Tbl = NULL;

	// Then, copy new values
	this->m_Size = other.m_Size;
	this->m_Elements = other.m_Elements;
	if (this->m_Size > 0)
	{
		m_Tbl = new TYPE*[m_Size];
		for (dword n = 0; n < other.m_Elements; n++)
		{
			this->m_Tbl[n] = new TYPE(*(other.m_Tbl[n]));
		}
	}
	return *this;
}

template <class TYPE>
TYPE & CArray<TYPE>::operator[](dword Idx) const
{
	TYPE* Item;

	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		Item = m_Tbl[Idx];
	}
	else
		throw;

	m_CriticalSection.Leave();
	
	return *Item;
}

// Appends an element to the end of the array
template <class TYPE>
bool CArray<TYPE>::Add (const TYPE & Item)
{
	bool    Result;
	
	m_CriticalSection.Enter();
	
	Result = Add (m_Elements, Item);
	
	m_CriticalSection.Leave();
	
	return Result;
}

// Inserts an element in the array at the appropiate position
template <class TYPE>
bool CArray<TYPE>::Add (dword Idx, const TYPE & Item)
{
	bool    Result;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx <= m_Elements)
	{
		if (m_Elements == m_Size)
			IncreaseSize ();

		memcpy (&m_Tbl[Idx+1], &m_Tbl[Idx], (m_Elements-Idx) * sizeof(TYPE*));

		m_Tbl[Idx] = new TYPE(Item);

		m_Elements = m_Elements + 1;

		Result = true;
	}
	else 
		Result = false;
	
	m_CriticalSection.Leave();
	
	return Result;
}

// Retrieves the number of elements in the array
template <class TYPE>
bool CArray<TYPE>::Elements(dword* Num_Elements) const
{
	m_CriticalSection.Enter();
	*Num_Elements = m_Elements;
	m_CriticalSection.Leave();
	return true;
}

// Returns the number of elements in the array
template <class TYPE>
dword CArray<TYPE>::GetElements() const
{
	dword res;
	m_CriticalSection.Enter();
	res = m_Elements;
	m_CriticalSection.Leave();
	return res;
}

// Returns a reference from an element from a given position
template <class TYPE>
const TYPE & CArray<TYPE>::GetItem(dword Idx) const
{
	TYPE* Item;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		Item = m_Tbl[Idx];
	}
	else
		throw;

	m_CriticalSection.Leave();
	
	return *Item;
}

// Retrieves a copy of an element from a given position
template <class TYPE>
bool CArray<TYPE>::GetItem(dword Idx, TYPE * Item) const
{
	bool	Result;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		*Item = *(m_Tbl[Idx]);
		Result = true;
	}
	else
		Result = false;

	m_CriticalSection.Leave();
	
	return Result;
}

// Modify an element in the array at the appropiate position
template <class TYPE>
bool CArray<TYPE>::Modify (dword Idx, const TYPE & Item)
{
	bool    Result = false;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		*(m_Tbl[Idx]) = Item;
		Result = true;
	}
	
	m_CriticalSection.Leave();
	
	return Result;
}

// Remove an element in the array at position Idx
template <class TYPE>
bool CArray<TYPE>::Remove (dword Idx)
{
	bool	Result;

	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		m_Elements = m_Elements - 1;
		delete m_Tbl[Idx];
		memcpy (&m_Tbl[Idx], &m_Tbl[Idx+1], (m_Elements-Idx)*sizeof(TYPE*));
		Result = true;
	}
	else
		Result = false;

	m_CriticalSection.Leave();
	
	return Result;
}

// Remove an element in the array at position Idx
template <class TYPE>
bool CArray<TYPE>::RemoveAll ()
{
	bool	Result;

	m_CriticalSection.Enter();
	
	for (dword Idx = 0; Idx < m_Elements; Idx++)
		delete m_Tbl[Idx];
	m_Elements = 0;
	Result = true;

	m_CriticalSection.Leave();
	
	return Result;
}

// Increase the size of the table to allocate more elements
template <class TYPE>
void CArray<TYPE>::IncreaseSize ()
{
	TYPE**	OldTbl;

	OldTbl = m_Tbl;
	m_Tbl = new TYPE*[m_Size + ArraySizeStep];
	memcpy (m_Tbl, OldTbl, m_Elements*sizeof(TYPE*));
	m_Size = m_Size + ArraySizeStep;
	delete OldTbl;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Copyright Oberon Sistemas S.L. 2002
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// ObUtils Library
// 
// Version control
// ===============
// $Revision: 4 $
// $Date: 13/06/05 8:56 $
// $Workfile: IdTbl.h $
//
// Description
// ===========
//
// Template definition for an array of Id objetcs
//
// This module defines a template from which to derive the classes
// to hold the objects based on Id. 
//
// Each object must define a struct to contain the attributes of the 
// elements in the table and a class derived from this.
//
// In adition to the methods defined in this template each object 
// can define specific methods to manage its items.
//
// The class holds an array of elements ordered by Id.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static const int IdTblSizeStep = 50;

// Class definition
template <class TYPE, class KEY>
class CIdTbl
{
public:
	CIdTbl ();
	CIdTbl (const CIdTbl & other);
	virtual ~CIdTbl ();

	CIdTbl & operator = (const CIdTbl & other);
	const TYPE & operator [] (dword idx) const;
	
	bool Elements		(dword* NumElements) const;

	bool FindItem		(const KEY & Id, dword* Idx) const;

	dword GetElements	() const;

	KEY GetId			(dword Idx) const;
	bool GetId			(dword Idx, KEY* Id) const;

	const TYPE & GetItem(dword Idx) const;
	bool GetItem		(dword Idx, TYPE* Item) const;

	bool Insert			(const TYPE & Item);

	bool Modify			(dword Idx, const TYPE & Item);

	bool Remove			(dword Idx);
	bool RemoveAll		();

protected:
	CUtilCriticalSection	m_CriticalSection;
	TYPE**					m_Tbl;
	dword					m_Elements;

private:
	void IncreaseSize();
	dword				m_Size;
};

// Constructor
template <class TYPE, class KEY>
CIdTbl<TYPE, KEY>::CIdTbl ()
{
	m_Tbl = NULL;
	m_Size = 0;
	m_Elements = 0;
}

// Copy constructor
template <class TYPE, class KEY>
CIdTbl<TYPE, KEY>::CIdTbl (const CIdTbl<TYPE, KEY> & other)
{
	m_Tbl = NULL;
	m_Size = 0;
	m_Elements = 0;
	*this = other;
}

// Destructor
// Keeps aware of destroying all elements in the array
template <class TYPE, class KEY>
CIdTbl<TYPE, KEY>::~CIdTbl ()
{
	m_CriticalSection.Enter();
	RemoveAll ();
	delete []m_Tbl;
	m_CriticalSection.Leave();
}

template <class TYPE, class KEY>
CIdTbl<TYPE,KEY> & CIdTbl<TYPE, KEY>::operator = (const CIdTbl & other)
{
	// First, delete old contents
	RemoveAll();
	delete []m_Tbl;
	m_Tbl = NULL;

	// Then, copy new values
	this->m_Size = other.m_Size;
	this->m_Elements = other.m_Elements;
	if (this->m_Size > 0)
	{
		m_Tbl = new TYPE*[m_Size];
		for (dword n = 0; n < other.m_Elements; n++)
		{
			this->m_Tbl[n] = new TYPE(*(other.m_Tbl[n]));
		}
	}
	return *this;
}

template <class TYPE, class KEY>
const TYPE & CIdTbl<TYPE, KEY>::operator[](dword Idx) const
{
	TYPE* Item;

	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		Item = m_Tbl[Idx];
	}
	else
		throw;

	m_CriticalSection.Leave();
	
	return *Item;
}

// Retrieves the number of elements in the array
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::Elements(dword* NumElements) const
{
	m_CriticalSection.Enter();
	*NumElements = m_Elements;
	m_CriticalSection.Leave();
	return true;
}

// Finds the position of an element giving its Id
// Searching is done using a half-splitting algorithm
// If the element is not found returns the position at which 
// a new element with that Id can be placed
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::FindItem(const KEY & Id, dword* Idx) const
{

	bool	Result = false;
	int		First;
	int		Last;
	int		Middle;

	m_CriticalSection.Enter();
	
	if (m_Elements > 0)
	{
		First = 0;
		Last = m_Elements - 1;
		do
		{
			Middle = (First + Last) / 2;
			if (m_Tbl[Middle]->GetId() < Id)
			{
				First = Middle + 1;
				*Idx = First;
			}
			if (m_Tbl[Middle]->GetId() == Id)
			{
				*Idx = Middle;
				Result = true;
				break;
			}
			if (m_Tbl[Middle]->GetId() > Id)
			{
				Last = Middle;
				*Idx = Last;
			}
		}
		while (First < Middle  ||  Middle < Last);
	}
	else
		*Idx = 0;

	m_CriticalSection.Leave();
	
	return Result;
}

// Retrieves the number of elements in the array
template <class TYPE, class KEY>
dword CIdTbl<TYPE, KEY>::GetElements() const
{
	dword NumElements;
	m_CriticalSection.Enter();
	NumElements = m_Elements;
	m_CriticalSection.Leave();
	return NumElements;
}

// Returns the Id of the element found at the given position
template <class TYPE, class KEY>
KEY CIdTbl<TYPE, KEY>::GetId(dword Idx) const
{
	KEY     Id;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		Id = m_Tbl[Idx]->GetId();
	}
	else
		throw;
	
	m_CriticalSection.Leave();
	
	return Id;	
}

// Retrieves the Id of the element found at the given position
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::GetId(dword Idx, KEY* Id) const
{
	bool	Result;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		*Id = m_Tbl[Idx]->GetId();
		Result = true;
	}
	else
		Result = false;
	
	m_CriticalSection.Leave();
	
	return Result;	
}

// Returns a reference to an element from a given position
template <class TYPE, class KEY>
const TYPE & CIdTbl<TYPE, KEY>::GetItem(dword Idx) const
{
	TYPE* Item;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		Item = m_Tbl[Idx];
	}
	else
		throw;

	m_CriticalSection.Leave();
	
	return *Item;
}

// Retrieves a copy of an element from a given position
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::GetItem(dword Idx, TYPE* Item) const
{
	bool	Result;
	
	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		*Item = *(m_Tbl[Idx]);
		Result = true;
	}
	else
		Result = false;

	m_CriticalSection.Leave();
	
	return Result;
}

// Insert an element in the array at the apropiate position
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::Insert (const TYPE &Item)
{
	dword	Idx;
	bool    Result;
	
	m_CriticalSection.Enter();
	
	Result = FindItem (Item.GetId(), &Idx);
	if (!Result)
	{
		if (m_Elements == m_Size)
			IncreaseSize ();

		memmove (&m_Tbl[Idx+1], &m_Tbl[Idx], (m_Elements-Idx) * sizeof(TYPE*));

		m_Tbl[Idx] = new TYPE(Item);

		m_Elements = m_Elements + 1;

		Result = true;
	}
	else 
		Result = false;
	
	m_CriticalSection.Leave();
	
	return Result;
}

// Modifies the element at indicated position
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::Modify (dword Idx, const TYPE & Item)
{
	bool    Result = false;

	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		if (m_Tbl[Idx]->GetId() == Item.GetId())
		{
			*m_Tbl[Idx] = Item;
			Result = true;
		}
	}

	m_CriticalSection.Leave();
	
	return Result;
}

// Remove an element in the array at position Idx
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::Remove (dword Idx)
{
	bool	Result;

	m_CriticalSection.Enter();
	
	if (Idx >= 0  &&  Idx < m_Elements)
	{
		m_Elements = m_Elements - 1;
		delete m_Tbl[Idx];
		memmove (&m_Tbl[Idx], &m_Tbl[Idx+1], (m_Elements-Idx)*sizeof(TYPE*));
		Result = true;
	}
	else
		Result = false;

	m_CriticalSection.Leave();
	
	return Result;
}

// Remove an element in the array at position Idx
template <class TYPE, class KEY>
bool CIdTbl<TYPE, KEY>::RemoveAll ()
{
	bool	Result;

	m_CriticalSection.Enter();
	
	for (dword Idx = 0; Idx < m_Elements; Idx++)
		delete m_Tbl[Idx];
	m_Elements = 0;
	Result = true;

	m_CriticalSection.Leave();
	
	return Result;
}

// Increase the size of the table to allocate more elements
template <class TYPE, class KEY>
void CIdTbl<TYPE, KEY>::IncreaseSize ()
{
	TYPE**	OldTbl;

	OldTbl = m_Tbl;
	m_Tbl = new TYPE*[m_Size + IdTblSizeStep];
	memcpy (m_Tbl, OldTbl, m_Elements*sizeof(TYPE*));
	m_Size = m_Size + IdTblSizeStep;
	delete OldTbl;
}
