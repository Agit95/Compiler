#pragma once

#include "IToken.h"

struct S_OperToken : S_IToken
{
public:
	//token value
	char  m_Operator;
	//ctor
	S_OperToken(E_Value_Types Type = E_Value_Types::OPERATOR, char val = '+') :S_IToken(Type), m_Operator(val) {}
	//copy  ctor
	S_OperToken(const S_OperToken& object) :S_IToken(object), m_Operator(object.m_Operator) {}
	//~ctor
	~S_OperToken() {}
};