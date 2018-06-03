#pragma once
#include "IToken.h"
#include <vector>
#include <any>

struct S_FuncCall : public S_IToken
{
public :
	std::vector<std::vector<std::any>> m_FuncArgs;
	std::string						   m_FuncName;

	S_FuncCall(const E_Value_Types& value_type, const std::string& name) :S_IToken(value_type), m_FuncName(name) {}
	S_FuncCall(const S_FuncCall& obj) :S_IToken(obj), m_FuncArgs(obj.m_FuncArgs), m_FuncName(obj.m_FuncName) {}
	~S_FuncCall() {}
};