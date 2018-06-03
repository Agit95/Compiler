#pragma once

#include "Compile_Error.h"
#include <fstream>
#include <sstream>
#include <queue>

class C_IO_Device
{
private:

	std::string				m_Output_file_path_name;
	std::string				m_Input_file_path_name;


public:
	C_IO_Device();
	C_IO_Device(const C_IO_Device&);
	~C_IO_Device();

	static 	std::stringstream	sm_Write_output_file_straem;
	static  std::stringstream	sm_Input_file_stream;


	void  Read_from_file();
	void  Write_to_out_file();
	void  initializer_input_output_file(const std::string&, const std::string&);
};