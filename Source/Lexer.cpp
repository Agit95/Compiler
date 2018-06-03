#define _CRT_SECURE_NO_WARNINGS

#include <stack>
#include "Lexer.h"
#include "Symbole_Table.h"
#include "Operators_and_Brackets.h"


C_Lexer::C_Lexer()
{
}


C_Lexer::~C_Lexer()
{
}


C_Lexer::C_Lexer(const C_Lexer&)
{
}


void C_Lexer::run()
{
	++m_LineNumber;
	static bool first_excequite = false;
	if (!first_excequite)
	{
		C_SymTab::insert_func("In", 0);
		C_SymTab::insert_func("Out", 1);
		first_excequite = true;
	}
	E_State state = E_State::BEGIN;
	E_State prev_state = E_State::BEGIN;

	if (!bracket_validator())
		throw C_LexerError("", "INVALID  BRACKET", m_LineNumber);

	if (!operator_validator())
		throw C_LexerError("", "INVALID  OPERATOR", m_LineNumber);

	if (!semicolon_validator())
		throw C_LexerError("", "INVALID ';'  TOKEN", m_LineNumber);

	std::string token;
	int line_length = m_LexedCode.length();

	state = E_State::DECLARETION;
	int i = 0;
	while (i < line_length)
	{
		//started  DECLARETION  state
		if (state == E_State::DECLARETION)
		{
			if (strchr(m_LexedCode.c_str(), '=') == nullptr)
				throw C_Compile_Error(" DONT  HAVE OPERATOR '=' ", m_LineNumber);

			while (m_LexedCode[i] != E_Operators::equal)
			{
				if (is_Separator(m_LexedCode[i]) && m_LexedCode[i] == ',')
				{
					token.push_back(m_LexedCode[i]);
					++i;
				}
				else if (is_Separator(m_LexedCode[i]))
					++i;
				else
				{
					token.push_back(m_LexedCode[i]);
					++i;
				}
			}

			if (is_Number(token))
				throw C_LexerError(token, "NUMBER IS  NOT  VARIABLE OR  FUNCTION", m_LineNumber);
			if (!tokens_validator(token))
				throw C_LexerError(token, "TOKEN  HAS  A  INVALID  SYMBOLE ", m_LineNumber);
			if (!in_out_validator(token))
				throw C_LexerError(token, "TOKEN  CAN  NOT  BE LVALUE", m_LineNumber);

			if (is_Function(token))
			{
				std::string name = get_function_name(token);

				if (C_SymTab::has_function(name))
					throw C_LexerError(token, "THE FUNCTIONAL  OVERLOUDING  NOT ALLOWED ", m_LineNumber);

				int func_argument = get_function_argument(token);
				S_FuncToken token_obj(E_Value_Types::FUNCTION, name, func_argument);
				C_SymTab::insert_func(name, func_argument);
				m_TokensVector.push_back(token_obj);
				token.clear();
			}
			else
			{
				S_VarToken varToken(E_Value_Types::VARIABLE, token);
				m_TokensVector.push_back(varToken);
				C_SymTab::insert_var(token);
				token.clear();
			}

			S_OperToken equalToken(E_Value_Types::OPERATOR, '=');
			m_TokensVector.push_back(equalToken);
			++i;
			state = E_State::DEFINETION;
		}

		while (!is_Separator(m_LexedCode[i]) && !is_Bracket(m_LexedCode[i])
			&& !is_Operator(m_LexedCode[i]) && i < line_length)
		{
			token.push_back(m_LexedCode[i]);
			++i;
		}

		if (!token.empty())
		{
			if (!tokens_validator(token))
				throw C_LexerError(token, " TOKEN  HAS  A  INVALID  SYMBOLE ", m_LineNumber);
		
			if (m_TokensVector.front().type() == typeid(S_FuncToken))
			{
				S_FuncToken object = std::any_cast<S_FuncToken>(m_TokensVector.front());
				if (is_LocalVariable(token))
				{
					S_VarToken varToken(E_Value_Types::VARIABLE, token);
					m_TokensVector.push_back(varToken);
					token.clear();
				}
				else if (C_SymTab::has_function(token) && token == object.m_FuncName)
					throw C_LexerError(token, " GENERATE  STACK  OVERFLOW ", m_LineNumber);
			}
			if (token.empty())
				continue;
			
			if (C_SymTab::has_function(token))
			{
				if (token == "Out")
					throw  C_LexerError(token, " OUT FUNCTION  IS NOT RVALUE ", m_LineNumber);
				Tfunc_Iterator iter = C_SymTab::get_function_iterator(token);
				S_FuncToken funcToken(E_Value_Types::FUNCTION, token,iter->second);
				token.clear();
				m_TokensVector.push_back(funcToken);
				if (iter->second == 0)
					i += 2;
			}
			else if (C_SymTab::has_variable(token))
			{
				S_VarToken varToken(E_Value_Types::VARIABLE, token);
				m_TokensVector.push_back(varToken);
				token.clear();
			}
			else if (is_Number(token) && get_number_type(atoi(token.c_str())) != E_Type::UNDEFINDE)
			{
				S_NumToken numericToken(E_Value_Types::NUMBER, get_number_type(atoi(token.c_str())), atoi(token.c_str()));
				m_TokensVector.push_back(numericToken);
				token.clear();
			}
			else
				throw  C_LexerError(token, " UNDEFINDE  VARIABLE ", m_LineNumber);
		}

		if (i == line_length)
			continue;

		if (is_Separator(m_LexedCode[i]))
			++i;
		else if (is_Bracket(m_LexedCode[i]))
		{
			S_BracketToken brToken(E_Value_Types::BRACKET, m_LexedCode[i]);
			m_TokensVector.push_back(brToken);
			++i;
		}
		else if (is_Operator(m_LexedCode[i]))
		{
			if (m_LexedCode[i] == '-' && (m_TokensVector.back().type() == typeid(S_BracketToken) 
										 || m_TokensVector.back().type() == typeid(S_OperToken)))
			{
				if (m_TokensVector.back().type() == typeid(S_BracketToken))
				{
					if (std::any_cast<S_BracketToken>(m_TokensVector.back()).m_Bracket == '(')
					{
						token.push_back('-');
						++i;
					}
					else
					{
						S_OperToken operToken(E_Value_Types::OPERATOR, m_LexedCode[i]);
						m_TokensVector.push_back(operToken);
						++i;
					}
				}
				else if (m_TokensVector.back().type() == typeid(S_OperToken))
				{
					if (std::any_cast<S_OperToken>(m_TokensVector.back()).m_Operator == '=')
					{
						token.push_back('-');
						++i;
					}
					else
					{
						S_OperToken operToken(E_Value_Types::OPERATOR, m_LexedCode[i]);
						m_TokensVector.push_back(operToken);
						++i;
					}
				}
			}
			else
			{
				S_OperToken operToken(E_Value_Types::OPERATOR, m_LexedCode[i]);
				m_TokensVector.push_back(operToken);
				++i;
			}
		}
		else
			throw C_Compile_Error(" UNDFINDE SYMBOLE ", m_LineNumber);
	}
	state = E_State::END_OF_STATE;
}

void C_Lexer::reset() noexcept
{
	m_LexedCode.clear();
	m_TokensVector.clear();
}

//init  lexer   and  set  input  code  from  lexing
void C_Lexer::set(const std::string& input) noexcept
{
	m_LexedCode = input;
}


//returned  tokens vector  
std::vector<std::any> C_Lexer::get_tokens_vector() const noexcept
{
	return std::move(m_TokensVector);
}


//validation  brcket  from  parsed string
bool  C_Lexer::bracket_validator()  const noexcept
{
	std::stack<char> bracket_validator_object;

	for (size_t i = 0; i < m_LexedCode.size(); ++i)
	{
		if (m_LexedCode[i] == ')' && bracket_validator_object.empty())
			return false;
		else if (m_LexedCode[i] == ')' && !bracket_validator_object.empty())
			bracket_validator_object.pop();
		else if (m_LexedCode[i] == '(')
			bracket_validator_object.push(m_LexedCode[i]);
		else if ((i != 0 && (m_LexedCode[i - 1] == '(' && is_Operator(m_LexedCode[i]) && m_LexedCode[i] != '-')) ||
			(i != 0 && (m_LexedCode[i] == ')' && is_Operator(m_LexedCode[i - 1]))))
			return false;
	}

	if (bracket_validator_object.empty())
		return true;
	return false;
}


// validate  operatos  in  expression
bool  C_Lexer::operator_validator() const noexcept
{
	for (size_t i = 0; i < m_LexedCode.length() - 1; ++i)
	{
		if (is_Operator(m_LexedCode[i]) && is_Operator(m_LexedCode[i + 1]))
			return false;
	}
	return true;
}


//validate  the  token  has  a operator or another invalid symbole
bool C_Lexer::tokens_validator(const std::string& token) const noexcept
{
	if (
		token.find('*') != -1 || token.find('/') != -1 || token.find('+') != -1 || token.find('|') != -1 ||
		token.find('$') != -1 || token.find('!') != -1 || token.find('@') != -1 || token.find('~') != -1 ||
		token.find('?') != -1 || token.find('%') != -1 || token.find('^') != -1 || token.find('&') != -1 ||
		token.find('"') != -1 || token.find(':') != -1 || token.find('.') != -1 || token.find('<') != -1 ||
		token.find('>') != -1 || token.find('[') != -1 || token.find(']') != -1 || token.find('{') != -1 ||
		token.find('}') != -1 
	   )
		return false;
	return true;
}


// validate  in and  out  function position
bool C_Lexer::in_out_validator(const std::string& token) const noexcept
{
	if (is_Function(token) && get_function_name(token) == "In")
		return false;
	return true;
}


//validate ; in  code  line
bool C_Lexer::semicolon_validator() const noexcept
{
	const char* first_equal = strchr(m_LexedCode.c_str(), '=');
	const char* last_equal = strrchr(m_LexedCode.c_str(), '=');

	if (first_equal != last_equal)
		return false;
	return true;
}


//returned   the  variable  number  or  not
bool  C_Lexer::is_Number(const std::string& symbole)  const  noexcept
{
	size_t i = 0;
	if (symbole[0] == E_Operators::minus)
		++i;
	while (i < symbole.size())
	{
		if (symbole[i] != '0' && symbole[i] != '1' && symbole[i] != '2' && symbole[i] != '3' && symbole[i] != '4' &&
			symbole[i] != '5' && symbole[i] != '6' && symbole[i] != '7' && symbole[i] != '8' && symbole[i] != '9')
			return false;
		++i;
	} 
	return true;
}


//returned   the  operand  is  funktion or  not
bool  C_Lexer::is_Function(const std::string& operand) const noexcept
{
	if (operand.length() >= 3)
	{
		int position_left = operand.find('(');
		int position_right = operand.find(')');
		if (position_right > position_left)
			return true;
	}
	return false;
}


//returned  the  operand  is  variable  or  not
bool  C_Lexer::is_Variable(const std::string& operand) const noexcept
{
	return  C_SymTab::has_variable(operand);
}


//returned  the  variable  is  local  or  global  variable
bool  C_Lexer::is_LocalVariable(const std::string& variable) const noexcept
{
	return C_SymTab::has_locale(variable);
}


//returnde   the  object  is operator  or  not
bool  C_Lexer::is_Operator(const char& symbole) const  noexcept
{
	switch (symbole)
	{
	case E_Operators::multiplication:
	case E_Operators::division:
	case E_Operators::minus:
	case E_Operators::plus:
	case E_Operators::equal: return true;
	default:  return false;
	}
}


//returnde   the  object  is operator  or  not
bool  C_Lexer::is_Operator(const std::string& object) const noexcept
{
	if (*(object.c_str()) == E_Operators::plus)
		return true;
	else  if (*(object.c_str()) == E_Operators::minus)
		return true;
	else  if (*(object.c_str()) == E_Operators::division)
		return true;
	else  if (*(object.c_str()) == E_Operators::multiplication)
		return true;
	else  if (*(object.c_str()) == E_Operators::equal)
		return true;
	return  false;
}


//returned  the  symbole  separator  or  not  
bool C_Lexer::is_Separator(const char& symbole) const noexcept
{
	if (symbole == ',' || symbole == '\t' || symbole == '\n' || symbole == ' ')
		return true;
	return false;
}


//returned   the  symbole  is  bracket  or  not 
bool C_Lexer::is_Bracket(const char& symbole) const noexcept
{
	if (symbole == E_Bracket::left_bracket || symbole == E_Bracket::right_bracket)
		return true;
	return false;
}


//returned  functions  argument  vector
int  C_Lexer::get_function_argument(const std::string& operand) const noexcept
{
	int position_left = operand.find('(');
	int position_right = operand.find(')');
	
	std::vector<std::string> function_argument;
	position_left++;
	std::string  argument;
	int counter = 0;

	while (position_left != position_right)
	{
		if (!is_Separator(operand[position_left]))
		{
			argument += operand[position_left];
			++position_left;
		}
		else if (!argument.empty())
		{
			C_SymTab::isnert_local(argument);
			argument.clear();
			++position_left;
			++counter;
		}
	}
	if (!argument.empty())
	{
		C_SymTab::isnert_local(argument);
		++counter;
	}

	return counter;
}


//returned  function name
std::string  C_Lexer::get_function_name(const std::string& function) const noexcept
{
	std::string func_name;
	int iterator = function.find('(');
	//copy function  name
	for (size_t i = 0; i < iterator; ++i)
	{
		func_name += function[i];
	}
	return std::move(func_name);
}

//returned  the  numbers  asambler type
E_Type C_Lexer::get_number_type(const int& number) const noexcept
{
	if (number > -128 && number < 127)
		return E_Type::BYTE;
	if (number > -8388608 && number < 8388607)
		return E_Type::WORD;
	if (number > -2147483647 && number < 2147483647)
		return E_Type::DWORD;
	if (number > -4294967296 && number < 429496729)
		return E_Type::QWORD;
	return E_Type::UNDEFINDE;
}
