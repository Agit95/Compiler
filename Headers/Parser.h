#pragma once

#include "Operators_and_Brackets.h"
#include "Function_Call.h"
#include <vector>
#include <stack>
#include <any>

class C_Parser
{
private:
	std::vector<std::any>	m_Tokens;
	std::vector<std::any>   m_PostOrder_Vector;
	enum E_ParserState { Expretion = 1, Function_Call = 2 };
	std::stack<E_ParserState> m_State;

public:
	/*-------------------- Public  Interface -------------------------*/
	void  set(const std::vector<std::any>&) noexcept;
	void  run();
	void  reset() noexcept;
	const std::vector<std::any> get_postordertoken() const noexcept;

private:
	void	PostOrder_Builder() noexcept;
	void    PostOrder_part_1(std::vector<std::any>&, std::stack<std::any>&, int&) noexcept;
	void    PostOrder_part_2(std::vector<std::any>&, std::stack<std::any>&) noexcept;
	void    PreParser(S_FuncCall& ,int&) noexcept;
	//helper  parts
	size_t  get_operator_prioritet(const char&) const noexcept;
	bool    is_operand(const std::any&) const noexcept;
	bool    is_none_Argumental_function(const std::any&) const noexcept;
	bool    is_function(const std::any&) const noexcept;

public:
	C_Parser();
	~C_Parser();
};

