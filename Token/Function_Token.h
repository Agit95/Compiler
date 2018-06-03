#pragma once

#include "IToken.h"
#include <string>

struct S_FuncToken : S_IToken 
{
public:
	//token value
	std::string  m_FuncName;
	//function argument count
	int			 m_ArgCount;
	//ctor
	S_FuncToken(E_Value_Types Type, std::string val, int count)
		:S_IToken(Type), m_FuncName(val), m_ArgCount(count) {}
	//copy  ctor
	S_FuncToken(const S_FuncToken& object) :S_IToken(object), m_FuncName(object.m_FuncName), m_ArgCount(object.m_ArgCount) {}
	//~ctor
	~S_FuncToken() {}
};