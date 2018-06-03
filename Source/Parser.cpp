#include "Parser.h"
#include "Bracket_Token.h"
#include "Function_Token.h"
#include "Numeric_Token.h"
#include "Operator_Token.h"
#include "Variable_Token.h"


C_Parser::C_Parser()
{
}


C_Parser::~C_Parser()
{
}


//init   parser's  tokens  vector 
void C_Parser::set(const std::vector<std::any>& tokens) noexcept
{
	m_Tokens = tokens;
}


// run  the work  of  parser
void C_Parser::run()
{
	if (m_Tokens.size() == 3)
	{
		m_PostOrder_Vector.push_back(m_Tokens[0]);
		m_PostOrder_Vector.push_back(m_Tokens[2]);
		m_PostOrder_Vector.push_back(m_Tokens[1]);
	}
	else
		PostOrder_Builder();
}

void  C_Parser::reset() noexcept
{
	m_Tokens.clear();
	m_PostOrder_Vector.clear();
}

const std::vector<std::any> C_Parser::get_postordertoken() const noexcept
{
	return m_PostOrder_Vector;
}

//bulid  abstract syntax tree  second  part
void  C_Parser::PostOrder_Builder() noexcept
{
	std::stack<std::any> Operation_stack;
	size_t token_count = m_Tokens.size();

	m_PostOrder_Vector.push_back(m_Tokens.front());

	for (int i = 2; i < token_count; ++i)
	{
		PostOrder_part_1(m_PostOrder_Vector, Operation_stack,i);
	}
	
	PostOrder_part_2(m_PostOrder_Vector, Operation_stack);
	m_PostOrder_Vector.push_back(m_Tokens[1]);
}


// bulider  part  one
void  C_Parser::PostOrder_part_1(std::vector<std::any>& PostOrder_Vector, std::stack<std::any>& Operation_stack, int& i) noexcept
{
	if (m_Tokens[i].type() == typeid(S_BracketToken) && (std::any_cast<S_BracketToken>(m_Tokens[i]).m_Bracket == '('))
	{
		Operation_stack.push(m_Tokens[i]);
	}
	else  if (is_operand(m_Tokens[i]))
	{
		if (is_function(m_Tokens[i]))
		{
			S_FuncCall Func_call(E_Value_Types::FUNCTION_CALL, std::any_cast<S_FuncToken>(m_Tokens[i]).m_FuncName);
			if (!is_none_Argumental_function(m_Tokens[i]))
			{
				++i;
				PreParser(Func_call, i);
				PostOrder_Vector.push_back(Func_call);
			}
			else
				PostOrder_Vector.push_back(Func_call);
		}
		else
			PostOrder_Vector.push_back(m_Tokens[i]);
	}
	else  if (m_Tokens[i].type() == typeid(S_OperToken))
	{
		while (
			!Operation_stack.empty() && Operation_stack.top().type() != typeid(S_BracketToken)
			&& (get_operator_prioritet(std::any_cast<S_OperToken>(m_Tokens[i]).m_Operator)
				< get_operator_prioritet(std::any_cast<S_OperToken>(Operation_stack.top()).m_Operator))
			)
		{
			PostOrder_Vector.push_back(Operation_stack.top());
			Operation_stack.pop();
		}
		Operation_stack.push(m_Tokens[i]);
	}
	else if (m_Tokens[i].type() == typeid(S_BracketToken) && (std::any_cast<S_BracketToken>(m_Tokens[i]).m_Bracket == ')'))
	{
		while (!Operation_stack.empty() && (Operation_stack.top().type() != typeid(S_BracketToken)))
		{
			PostOrder_Vector.push_back(Operation_stack.top());
			Operation_stack.pop();
		}
		//pop '(' token
		Operation_stack.pop();
	}
}


// postorder  build part  2
void  C_Parser::PostOrder_part_2(std::vector<std::any>& PostOrder_Vector, std::stack<std::any>& Operation_stack) noexcept
{
	while (!Operation_stack.empty() && (Operation_stack.top().type() != typeid(S_BracketToken)))
	{
		PostOrder_Vector.push_back(Operation_stack.top());
		Operation_stack.pop();
	}
	if (!Operation_stack.empty())
		Operation_stack.pop();
}


void  C_Parser::PreParser(S_FuncCall& Func_call, int& i) noexcept
{
	m_State.push(E_ParserState::Function_Call);
	std::vector<std::any>  temp_postorder;
	std::stack<std::any> temp_operation;

	temp_operation.push(m_Tokens[i]);
	++i;
	while (!m_State.empty())
	{
		while (m_Tokens[i].type() != typeid(S_BracketToken))
		{
			while (m_Tokens[i].type() != typeid(S_BracketToken) && m_Tokens[i].type() != m_Tokens[i + 1].type())
			{
				PostOrder_part_1(temp_postorder, temp_operation, i);
				++i;
			}
			PostOrder_part_2(temp_postorder, temp_operation);
			/////////////////////////////////////////
			Func_call.m_FuncArgs.push_back(temp_postorder);
			////////////////////////////////////////////
			temp_postorder.clear();
		}
		m_State.pop();
	}
}


// returned  operator  prioritet
size_t C_Parser::get_operator_prioritet(const char& Operator) const noexcept
{
	switch (Operator)
	{
	case E_Operators::minus: return 1;
	case E_Operators::plus:	return 2;
	case E_Operators::division: return 3;
	case E_Operators::multiplication: return 4;

	default:
		break;
	}
}


// returned   the  token  is  operand  or  not;
bool  C_Parser::is_operand(const std::any& object) const noexcept
{
	return object.type() == typeid(S_FuncToken) || object.type() == typeid(S_VarToken) || object.type() == typeid(S_NumToken);
}


//returned  the   operand is  noneargumental function or  not
bool  C_Parser::is_none_Argumental_function(const std::any& object) const noexcept
{
	return std::any_cast<S_FuncToken>(object).m_ArgCount == 0;
}


//returned  operand  is  function  or  not
bool  C_Parser::is_function(const std::any& object) const noexcept
{
	return  object.type() == typeid(S_FuncToken);
}





