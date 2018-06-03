#pragma once

#include <string>
#include <vector>
#include <any>
#include "Compile_Error.h"
#include "IToken.h"
#include "Numeric_Token.h"
#include "Operator_Token.h"
#include "Function_Token.h"
#include "Variable_Token.h"
#include "Bracket_Token.h"

class C_Lexer
{
protected:
	std::vector<std::any>  m_TokensVector;
	std::string			   m_LexedCode;
	unsigned int		   m_LineNumber;

public:
	C_Lexer();
	C_Lexer(const C_Lexer&);
	~C_Lexer();
	/*-------------------- Public  Interface -------------------------*/
	void set(const std::string&) noexcept;
	void run();
	void reset() noexcept;
	std::vector<std::any> get_tokens_vector() const noexcept;

protected:
	/*---------------------- Inspection  Part -------------------------*/
	bool bracket_validator() const noexcept;
	bool operator_validator() const noexcept;
	bool semicolon_validator() const noexcept;
	bool tokens_validator(const std::string&) const noexcept;
	bool in_out_validator(const std::string&) const noexcept;
	bool is_Number(const std::string&) const noexcept;
	bool is_Function(const std::string&) const noexcept;
	bool is_Variable(const std::string&) const noexcept;
	bool is_LocalVariable(const std::string&) const noexcept;
	bool is_Operator(const std::string&) const noexcept;
	bool is_Operator(const char&) const  noexcept;
	bool is_Separator(const char&) const noexcept;
	bool is_Bracket(const char&) const noexcept;
	/*------------------------------- Type  Part ----------------------------------*/
	int get_function_argument(const std::string&) const noexcept;
	std::string  get_function_name(const std::string&) const noexcept;
	E_Type get_number_type(const int&) const noexcept;

protected:
	typedef enum
	{
		BEGIN = 0,	DECLARETION = 1,  DEFINETION = 2,	END_OF_STATE = 4
	} E_State;

};


class C_LexerError : public C_Compile_Error
{
public:
	C_LexerError(const std::string& token = " ", const std::string& error_description = " " , const int i = 0)
		:m_Token(token), m_Error_Description(error_description), m_error_line(i)
	{ 
	}

	virtual std::string show_error_message() const override
	{
		return std::move(m_Token + "  IN  LINE " + std::to_string(m_error_line) + m_Error_Description);
	}
	
protected:
	std::string  m_Token;
	std::string  m_Error_Description;
	unsigned int m_error_line;
};
