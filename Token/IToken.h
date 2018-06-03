#pragma once

#include "Value_Types.h"

struct S_IToken
{
public:
	//Type the  saving value
	E_Value_Types m_ValueType;
	//ctor
	S_IToken(E_Value_Types Type) :m_ValueType(Type) {}
	//copy  ctor
	S_IToken(const S_IToken& object) :m_ValueType(object.m_ValueType) {}
	//vitrual ~ctor
	~S_IToken() {}
};


