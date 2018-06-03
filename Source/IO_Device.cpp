#define _CRT_SECURE_NO_WARNINGS
#include "IO_Device.h"
#include <iostream>
#include <cstring>


C_IO_Device::C_IO_Device()      //ctor by  default
{
}

C_IO_Device::~C_IO_Device()		//~ctor  by  default
{
}

C_IO_Device::C_IO_Device(const C_IO_Device&)    //copy  ctor
{
}

//input  compiling  file  path  
void C_IO_Device::initializer_input_output_file(const std::string& input_file_path, const std::string& out_file_path)
{
	m_Input_file_path_name  = input_file_path;
	m_Output_file_path_name = out_file_path;
}

//read  input file  to  stringstream
void C_IO_Device::Read_from_file()
{
	std::string  t_Read_line;
	std::fstream t_Input_file;
	t_Input_file.open(m_Input_file_path_name, std::ios::in);

	//if  this  file is  not  open  or  invalid retured false
	if (!t_Input_file.is_open())
		throw C_Compile_Error(" CAN  NOT  OPEN INPUT FILE ");

	//read  file
	while (!t_Input_file.eof())
	{
		std::getline(t_Input_file, t_Read_line);
		sm_Input_file_stream << t_Read_line;
	}
	//read  from  file  is  completed

	t_Input_file.close();  //close  input  file         
}



//wiret  assambly  code  in  to  output file
void C_IO_Device::Write_to_out_file()
{
	std::fstream temp_out_file;
	std::string temp_str = sm_Write_output_file_straem.str();
	temp_out_file.open(m_Output_file_path_name, std::ios::out);

	//if this  file  in not  open returned false
	if (!temp_out_file.is_open())
		throw C_Compile_Error(" OUTPUT FILE CAN  NOT  OPEN ");

	//write to  out file
	temp_out_file << temp_str;
	temp_out_file.close();
	std::cout << "COMPILE  IS  COMPLETED , HAS  ALL  REDY  OUTPUT  FILE  " << m_Output_file_path_name << std::endl;
	//write  in  file  is  completed  
}


std::stringstream C_IO_Device::sm_Write_output_file_straem;
std::stringstream C_IO_Device::sm_Input_file_stream;
