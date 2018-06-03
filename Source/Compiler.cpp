#define _CRT_SECURE_NO_WARNINGS
#include "Compiler.h"
#include "Symbole_Table.h"
#include <iostream>



C_Compiler::C_Compiler()
{
}


C_Compiler::~C_Compiler()
{
}


void  C_Compiler::execute()
{
	try
	{
		set_io_file_name();
		get_line_up_to_semicoln();
		m_CodeGenerator.initialize_Data_and_Code_sections();
		while (!m_Line_Queue.empty())
		{
			m_Lexer.set(m_Line_Queue.front());
			m_Lexer.run();
			m_Parser.set(m_Lexer.get_tokens_vector());
			m_Parser.run();
			m_CodeGenerator.set(m_Parser.get_postordertoken());
			m_CodeGenerator.run();
			m_Line_Queue.pop();
			m_Lexer.reset();
			m_Parser.reset();
			C_SymTab::reset();
		}
		m_CodeGenerator.generate_output_code();
		m_IO.Write_to_out_file();
	}
	catch (const C_LexerError& error)
	{
		std::cout << std::endl;
		std::cout << error.show_error_message() << std::endl;
		std::cout << std::endl;
	}
	catch (const C_Compile_Error& error)
	{
		std::cout << std::endl;
		std::cout << error.show_error_message() << std::endl;
		std::cout << std::endl;
	}
	catch (const std::exception& error)
	{
		std::cout << std::endl;
		std::cout << error.what() << std::endl;
		std::cout << std::endl;
	}
}

// initialse  input  and outut file
void  C_Compiler::set_io_file_name()
{
	std::string input;
	std::string output;

	std::cout << "Pleas  insert input file path and  name: ";
	std::cin >> input;
	std::cout << "Pleas  insert output file path and  name: ";
	std::cin >> output;
	m_IO.initializer_input_output_file(input, output);
}

//geting  code  line  queue
void  C_Compiler::get_line_up_to_semicoln()
{
	m_IO.Read_from_file();
	std::string line = C_IO_Device::sm_Input_file_stream.str();
	char* temp_Code = const_cast<char*>(line.c_str());
	char* temp_str = std::strtok(temp_Code, ";");

	while (temp_str != NULL)
	{
		std::string  obj;
		obj += temp_str;
		m_Line_Queue.push(obj);
		temp_str = strtok(NULL, ";");
		obj.clear();
	}
}
