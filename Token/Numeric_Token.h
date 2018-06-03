#pragma once

#include "IToken.h"
#include "Types.h"

struct S_NumToken : S_IToken
{
public:
	//token value
	int		m_Value;
	//value  type
	E_Type  m_Type;
	//ctor
	S_NumToken(E_Value_Types Type = E_Value_Types::NUMBER, E_Type valType = E_Type::DWORD, int val = 0) 
		:S_IToken(Type), m_Value(val) {}
	//copy  ctor
	S_NumToken(const S_NumToken& object) :S_IToken(object), m_Value(object.m_Value), m_Type(object.m_Type) {}
	//~ctor
	~S_NumToken(){}
};
