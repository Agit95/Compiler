#pragma once
#include "Register_Alocator.h"
#include "GeneratorHelper.h"
#include "Tree.h"


class C_CodeGenerator
{
private:
	/*----------------- STD ----------------*/
	std::vector<std::any>    m_PostorderToken;
	std::stack<std::any>	 m_Operand_stack;
	std::stack<E_GPR>		 m_Expectant_GPR;
	std::stringstream		 m_Data_section;
	std::stringstream		 m_Function_section;
	std::stringstream		 m_Variable_section;
	/*------------------- UDT ------------------*/
	C_CommandGeneratorHelper m_Command_generator;
	C_Register_Alocator      m_Alocator;
	unsigned int			 m_Line_number;
	bool                     m_IsFunction;
	bool                     m_Argument_generator;

public:
	C_CodeGenerator();
	~C_CodeGenerator();
	/*-------------------- Public  Interface -------------------------*/
	void  set(const std::vector<std::any>&) noexcept;
	void  run();
	void  initialize_Data_and_Code_sections() noexcept;
	void  generate_output_code() noexcept;

private:
	/*------------------------- Type  Part ----------------------------*/
	const  std::string  get_operation_type(const E_Type&) const noexcept;
	const  E_Type       get_register_type(const E_GPR&) const noexcept;
	/*---------------------- Generation  Part -------------------------*/ 
	void   code_generator_exe() noexcept;
	void   genrate_code_section() noexcept;
	void   load_local_argument(const std::any&, std::stringstream&) noexcept;
	void   function_call(const E_AR&, const std::string&, std::stringstream&) noexcept;
	void   function_argument_push(const std::any&, std::stringstream&) noexcept;
	void   none_Argumental_funcion_call(const char&, const E_AR&,const std::string&, std::stack<E_GPR>&, std::stringstream&) noexcept;
	void   arithmetic_operation_code_generator(const char&, const E_Type&, const E_GPR&,const E_GPR&,std::stringstream&) noexcept;
	/*---------------------- Inspection  Part -------------------------*/
	bool   is_NoneArgumentalFunction(const std::any&) const noexcept;
	bool   is_LocalVariable(const std::any&) const noexcept;
	bool   is_Operator(const std::any&) const noexcept;
	bool   is_Function(const std::any&) const noexcept;
	bool   is_Variable(const std::any&) const noexcept;
	bool   is_Number(const std::any&) const noexcept;
	/*-------------------- Operation Case Part -------------------------*/	
	void   general_case(std::stringstream&) noexcept;
	void   case_operator() noexcept;
	void   case_pop_operand() noexcept;
	void   case_register_operand(const std::any&, const std::any&, std::stringstream&) noexcept;
	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	void   case_number_other_operand(std::stringstream&) noexcept;
	void   case_function_other_operand(std::stringstream&) noexcept;
	void   case_variable_other_operand(std::stringstream&) noexcept;
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	void   case_number_number(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	void   case_number_variable(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	void   case_number_function(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	void   case_variable_number(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	void   case_variable_variable(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	void   case_variable_function(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	void   case_function_number(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	void   case_function_variable(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	void   case_function_function(const std::any&, const std::any&, const std::any&, std::stringstream&) noexcept;
	/*--------------------- Finally Case Part ---------------------------*/
	void   case_register_register(const std::any&, std::stringstream&) noexcept;
	void   case_assigment_operands(std::stringstream&, const bool&);
	void   case_asigment_register(std::stringstream&, const bool&) noexcept;
};

