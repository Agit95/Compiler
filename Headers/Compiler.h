#pragma once

#include "IO_Device.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGenerator.h"

class C_Compiler
{
public:
	C_Compiler();
	~C_Compiler();

	void  execute();

private:
	void  set_io_file_name();
	void  get_line_up_to_semicoln();

protected:
	C_IO_Device					m_IO;
	C_Lexer						m_Lexer;
	C_Parser					m_Parser;
	C_CodeGenerator				m_CodeGenerator;
	std::queue<std::string>		m_Line_Queue;
};

