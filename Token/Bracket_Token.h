#pragma once


#include "IToken.h"
#include "Types.h"

struct S_BracketToken : S_IToken
{
public:
	
	char  m_Bracket;
	bool  m_isLeftBracket;

	//ctor
	S_BracketToken(E_Value_Types Type, const char br) :S_IToken(Type),m_Bracket(br)
	{
		if (br == '(')
			m_isLeftBracket = true;
		else
			m_isLeftBracket = false;
	}
	//copy  ctor
	S_BracketToken(const S_BracketToken& object) :S_IToken(object) {}
	//~ctor
	~S_BracketToken() {}
};