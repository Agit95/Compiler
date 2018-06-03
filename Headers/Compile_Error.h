#pragma once
#include <exception>
#include <string>

class C_Compile_Error : public std::exception
{
protected:
	const char* m_Error_message;
	int			m_Error_line;

public:
	//initialize  Error_message
	C_Compile_Error(const char * error_message = nullptr , int line_number = 0)
		: m_Error_message(error_message), m_Error_line(line_number)
	{
	}

	//copy  ctor  from   error  message  class
	C_Compile_Error(const C_Compile_Error& Error_object)
		:m_Error_message(Error_object.m_Error_message), m_Error_line(Error_object.m_Error_line)
	{
	}

	virtual ~C_Compile_Error()
	{
	}

	//Show  Error  mesage
	virtual std::string  show_error_message()  const
	{
		if (m_Error_line != 0)
			return std::move(m_Error_message + std::to_string(m_Error_line));
		else
			return std::move(m_Error_message);
	}
};