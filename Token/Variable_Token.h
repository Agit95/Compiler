#pragma once

#include "IToken.h"
#include <string>

struct S_VarToken : S_IToken
{
public:
	//token value
	std::string  m_VariableName;
	//ctor
	S_VarToken(E_Value_Types Type = E_Value_Types::VARIABLE, std::string val = "") :S_IToken(Type), m_VariableName(val) {}
	//copy  ctor
	S_VarToken(const S_VarToken& object) :S_IToken(object), m_VariableName(object.m_VariableName) {}
	//~ctor
	~S_VarToken() {}
};