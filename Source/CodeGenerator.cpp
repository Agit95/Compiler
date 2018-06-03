#include "CodeGenerator.h"
#include "IO_Device.h"
#include "Numeric_Token.h"
#include "Operator_Token.h"
#include "Variable_Token.h"
#include "Function_Token.h"
#include "Function_Call.h"
#include "Operators_and_Brackets.h"
#include "Types.h"
#include "Symbole_Table.h"


C_CodeGenerator::C_CodeGenerator()
{
}


C_CodeGenerator::~C_CodeGenerator()
{
}


/*############################ BEGIN PUBLIC INTERFACE ############################*/

//initialize  code  generator
void  C_CodeGenerator::set(const std::vector<std::any>& postorderToken) noexcept
{
	m_PostorderToken = postorderToken;
}
	

// runing  code generator
void  C_CodeGenerator::run()
{
	code_generator_exe();
}

/*############################# END PUBLIC INTERFACE #############################*/

/*############################### BEGIN TYPE PART ################################*/

// returned   operation type 
const  std::string  C_CodeGenerator::get_operation_type(const E_Type& type) const noexcept
{
	switch (type)
	{
	case E_Type::BYTE:  return " B ";
	case E_Type::WORD:  return " W ";
	case E_Type::DWORD: return " DW ";
	case E_Type::QWORD: return " QW ";
	default: return " DW ";
	}
}


// returned  register type
const  E_Type  C_CodeGenerator::get_register_type(const E_GPR& reg) const noexcept
{
	if (reg % 8 == 0)
		return E_Type::QWORD;
	if (reg % 2 == 0 && reg % 4 != 0)
		return E_Type::WORD;
	if (reg % 4)
		return E_Type::DWORD;
	return E_Type::BYTE;
}

/*################################ END TYPE PART #################################*/

/*############################ BEGIN GENERATION PART #############################*/

void  C_CodeGenerator::code_generator_exe() noexcept
{
	std::stringstream       current_stream;

	std::any node = m_PostorderToken.front();
	is_Function(node) ? m_IsFunction = true : m_IsFunction = false;
	if (!m_Argument_generator)
	{
		m_Line_number++;

		if (is_Function(node))
		{
			std::any func = node;
			std::string name = std::any_cast<S_FuncToken>(func).m_FuncName;
			current_stream << m_Command_generator.label(name) << std::endl;
			current_stream << m_Command_generator.push() << std::endl;
			current_stream << m_Command_generator.move() << std::endl;
		}
		else
		{
			m_Data_section << m_Command_generator.dword(std::any_cast<S_VarToken>(node).m_VariableName) << std::endl;
		}
		if (m_PostorderToken.size() == 3)
		{
			if (std::any_cast<S_OperToken>(m_PostorderToken.back()).m_Operator == E_Operators::equal)
			{
				case_assigment_operands(current_stream, is_Function(node));
			}
		}
	}

	general_case(current_stream);

	while (!m_Operand_stack.empty())
	{
		if (m_Expectant_GPR.size() == 1 && m_Operand_stack.size() == 2)
		{
			m_PostorderToken.push_back(m_Operand_stack.top());
			m_Operand_stack.pop();
			case_asigment_register(current_stream, is_Function(node));
		}
	}

	if (!m_Argument_generator)
	{
		if (is_Function(node))
			m_Function_section << current_stream.str() << std::endl << std::endl;
		else
			m_Variable_section << current_stream.str() << std::endl;
		current_stream.clear();
	}

	m_Alocator.update_GPA_and_AR();
}


// generate  assabler  code  
void  C_CodeGenerator::generate_output_code() noexcept
{
	genrate_code_section();
	C_IO_Device::sm_Write_output_file_straem << m_Data_section.str() << std::endl << 
				  m_Function_section.str() << m_Variable_section.str() << std::endl;
}


//generate  aasambelr  code  in  our  merber
void  C_CodeGenerator::genrate_code_section() noexcept
{
	m_Variable_section << m_Command_generator.pop() << std::endl;
	m_Variable_section << m_Command_generator.ret() << std::endl;
	m_Variable_section << m_Command_generator.exit() << std::endl;
}

void  C_CodeGenerator::load_local_argument(const std::any& variable , std::stringstream& code_stream) noexcept
{
	E_Type type = E_Type::DWORD;
	E_GPR reg = m_Alocator.find_empty_GPR_in_input_type(type);
	std::string var = std::any_cast<S_VarToken>(variable).m_VariableName;

	code_stream << "MOVE  R" + std::to_string(reg) + " , A1" << std::endl;
	
	int i = 1;
	for (const auto& element : C_SymTab::m_LocalVariable)
	{
		if (element != var)
			++i;
		else
			break;
	}
	code_stream << m_Command_generator.assign("DW", E_GPR(reg + 4), (i+2)*4) << std::endl;
	code_stream << "SUB  R" + std::to_string(reg) + " , R" + std::to_string(E_GPR(reg + 4)) + " , R" + std::to_string(reg) << std::endl;
	code_stream << "LOAD  R" + std::to_string(E_GPR(reg + 4)) + " , R" + std::to_string(reg) << std::endl;
}


void  C_CodeGenerator::function_call(const E_AR& address, const std::string& function, std::stringstream& code_stream) noexcept
{
	code_stream << m_Command_generator.assign(address, function) << std::endl;
	code_stream << m_Command_generator.call(address) << std::endl;
}


void  C_CodeGenerator::function_argument_push(const std::any& Function, std::stringstream& code_stream) noexcept
{
	std::vector<std::any> temp = std::move(m_PostorderToken);
	std::stack<std::any> temp_stack = std::move(m_Operand_stack);
	std::stack<E_GPR> temp_GPR = std::move(m_Expectant_GPR);
	while (!m_Operand_stack.empty())
	{
		m_Operand_stack.pop();
	}
	while (!m_Expectant_GPR.empty())
	{
		m_Expectant_GPR.pop();
	}
	m_Argument_generator = true;
	for (auto& element : std::any_cast<S_FuncCall>(Function).m_FuncArgs)
	{
		m_PostorderToken = element;
		if (m_PostorderToken.size() == 1)
		{
			if (is_Function(m_PostorderToken[0]))
				function_call(E_AR::A2, std::any_cast<S_FuncCall>(m_PostorderToken[0]).m_FuncName, code_stream);
			else if (is_Variable(m_PostorderToken[0]))
			{
				if (m_IsFunction && is_LocalVariable(m_PostorderToken[0]))
				{
					load_local_argument(m_PostorderToken[0], code_stream);
				}
				else
				{
					code_stream << m_Command_generator.assign(E_AR::A2, std::any_cast<S_VarToken>(m_PostorderToken[0]).m_VariableName) << std::endl;
					code_stream << m_Command_generator.load(E_GPR::R0, E_AR::A2) << std::endl;
				}
			}
			else
			{
				code_stream << m_Command_generator.assign("DW", E_GPR::R0, std::any_cast<S_NumToken>(m_PostorderToken[0]).m_Value) << std::endl;
			}
		}
		else
		{
			code_generator_exe();
		}
		code_stream << m_Command_generator.push(E_GPR::R0) << std::endl;
		if (!m_Expectant_GPR.empty())
			m_Expectant_GPR.pop();
	}
	m_Argument_generator = false;
	m_PostorderToken = std::move(temp);
	m_Expectant_GPR = std::move(temp_GPR);
	m_Operand_stack = std::move(temp_stack);
}



// init  data  and  code   section
void  C_CodeGenerator::initialize_Data_and_Code_sections() noexcept
{
	m_Data_section << m_Command_generator.data() << std::endl;
	m_Function_section << m_Command_generator.code() << std::endl << std::endl;
	m_Function_section << m_Command_generator.start() << std::endl << std::endl;
	m_Variable_section << m_Command_generator.label("START") << std::endl;
	m_Variable_section << m_Command_generator.push() << std::endl;
	m_Variable_section << m_Command_generator.move() << std::endl;
}


void  C_CodeGenerator::none_Argumental_funcion_call(const char& _operator, const E_AR& address, const std::string& func,
													std::stack<E_GPR>& temp_gpr, std::stringstream& code_stream) noexcept
{
	//call function and  reserving R0
	function_call(address, func, code_stream);
	m_Alocator.change_GPR_state(E_GPR::R0);
	/*------------------------------------*/
	if (temp_gpr.top() == E_GPR::R0)
	{
		code_stream << m_Command_generator.move(E_GPR::R4, E_GPR::R0) << std::endl;
	}
	code_stream << m_Command_generator.pop(temp_gpr.top()) << std::endl;
	temp_gpr.pop();
	temp_gpr.push(E_GPR::R4);
	arithmetic_operation_code_generator(_operator, E_Type::DWORD, temp_gpr.top(), E_GPR::R0, code_stream);
	code_stream << m_Command_generator.move(temp_gpr.top(), E_GPR::R0) << std::endl;
	m_Expectant_GPR.pop();
	m_Expectant_GPR.push(temp_gpr.top());
	m_Alocator.change_GPR_state(temp_gpr.top());
	temp_gpr.pop();
}


// generate arithmetic operation code
void  C_CodeGenerator::arithmetic_operation_code_generator(const char& oper, const E_Type& type, const E_GPR& left,
														   const E_GPR& right,std::stringstream& code_stream) noexcept
{
	switch (oper)
	{
	case E_Operators::plus:
		code_stream << m_Command_generator.add(get_operation_type(type), left, right) << std::endl; break;
	case E_Operators::minus:
		code_stream << m_Command_generator.sub(get_operation_type(type), left, right) << std::endl; break;
	case E_Operators::division:
		code_stream << m_Command_generator.div(get_operation_type(type), left, right) << std::endl; break;
	case E_Operators::multiplication:
		code_stream << m_Command_generator.mul(get_operation_type(type), left, right) << std::endl; break;
	default:
		break;
	}
	E_GPR reg = left > right ? left : right;
	E_GPR reg_1 = left < right ? left : right;
	m_Alocator.change_GPR_state(reg);
	m_Expectant_GPR.push(reg_1);

	if (m_Expectant_GPR.size() >= 2)
	{
		std::any oper = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_register_register(oper, code_stream);
	}
	else if (m_PostorderToken.size() == 1 && m_Expectant_GPR.size() == 1 && m_Operand_stack.size() == 1)
	{
		if (std::any_cast<S_OperToken>(m_Operand_stack.top()).m_Operator == E_Operators::equal)
		{
			case_asigment_register(code_stream, is_Function(m_PostorderToken.back()));
		}
		else
		{
			case_assigment_operands(code_stream, is_Function(m_PostorderToken.back()));
		}
	}
	else if (m_PostorderToken.size() == 1 && m_Expectant_GPR.size() == 1 && m_Operand_stack.size() >= 3)
	{
		std::any operand = m_Operand_stack.top();
		m_Operand_stack.pop();
		std::any operator_1 = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_register_operand(operator_1, operand, code_stream);
	}
}

/*############################# END GENERATION PART ##############################*/

/*############################ BEGIN INSPECTION PART #############################*/

// returned   function has  a argument  or  not
bool  C_CodeGenerator::is_NoneArgumentalFunction(const std::any& token) const noexcept
{
	return std::any_cast<S_FuncCall>(token).m_FuncArgs.size() == 0;
}


bool  C_CodeGenerator::is_LocalVariable(const std::any& variable) const noexcept
{
	std::string var = std::any_cast<S_VarToken>(variable).m_VariableName;
	return C_SymTab::has_locale(var);
}

//returned  token is operator  or  not
bool  C_CodeGenerator::is_Operator(const std::any& token) const noexcept
{
	return token.type() == typeid(S_OperToken);
}


//returned  token is function  or  not
bool  C_CodeGenerator::is_Function(const std::any& token) const noexcept
{
	return token.type() == typeid(S_FuncToken) || token.type() == typeid(S_FuncCall);
}


//returned  token is variable  or  not
bool  C_CodeGenerator::is_Variable(const std::any& token) const noexcept
{
	return token.type() == typeid(S_VarToken);
}


//returned  token is number  or  not
bool  C_CodeGenerator::is_Number(const std::any& token) const noexcept
{
	return token.type() == typeid(S_NumToken);
}

/*############################# END INSPECTION PART #############################*/


/*########################## BEGIN OPERAION CASE PART ###########################*/

void  C_CodeGenerator::general_case(std::stringstream& current_stream) noexcept
{
	while (!m_PostorderToken.empty())
	{
		if (!m_PostorderToken.empty() && m_PostorderToken.size() != 1)
		{
			if (is_Operator(m_PostorderToken.back()))
			{
				case_operator();
			}
			else if (is_Number(m_PostorderToken.back()))
			{
				case_number_other_operand(current_stream);
			}
			else if (is_Function(m_PostorderToken.back()))
			{
				case_function_other_operand(current_stream);
			}
			else if (is_Variable(m_PostorderToken.back()))
			{
				case_variable_other_operand(current_stream);
			}
		}
		else if (m_Expectant_GPR.size() == 1 && m_Operand_stack.size() == 1)
		{
			case_asigment_register(current_stream, m_IsFunction);
		}
		else if (m_Expectant_GPR.size() == 1 && m_Operand_stack.size() >= 3)
		{
			std::any operand = m_Operand_stack.top();
			m_Operand_stack.pop();
			std::any operator_1 = m_Operand_stack.top();
			m_Operand_stack.pop();
			case_register_operand(operator_1, operand, current_stream);
		}
	}
}


// working  fromoperator
void  C_CodeGenerator::case_operator() noexcept
{
	m_Operand_stack.push(m_PostorderToken.back());
	m_PostorderToken.pop_back();
}


// pop  variables
void  C_CodeGenerator::case_pop_operand() noexcept
{
	if (!m_Operand_stack.empty())
		m_Operand_stack.pop();
	if (!m_PostorderToken.empty())
		m_PostorderToken.pop_back();
}

//case  from register and operand
void  C_CodeGenerator::case_register_operand(const std::any& oper, const std::any& node, std::stringstream& code_stream) noexcept
{
	char oper_1 = std::any_cast<S_OperToken>(oper).m_Operator;
	if (is_Number(node))
	{
		int  num = std::any_cast<S_NumToken>(node).m_Value;
		E_Type type = std::any_cast<S_NumToken>(node).m_Type;
		//
		E_GPR reg = m_Alocator.find_empty_GPR_in_input_type(type);
		m_Alocator.change_GPR_state(reg);
		//
		E_GPR right = m_Expectant_GPR.top();
		m_Expectant_GPR.pop();
		arithmetic_operation_code_generator(oper_1, type, reg, right, code_stream);
	}
	else if (is_Variable(node))
	{
		std::string var = std::any_cast<S_VarToken>(node).m_VariableName;
		E_Type type = E_Type::DWORD;
		code_stream << m_Command_generator.assign(m_Alocator.find_empty_AR(), var) << std::endl;
		code_stream << m_Command_generator.load(m_Alocator.find_empty_GPR_in_input_type(type), m_Alocator.find_empty_AR()) << std::endl;
		E_GPR left = m_Expectant_GPR.top();
		E_GPR right = m_Alocator.find_empty_GPR_in_input_type(type);
		m_Alocator.change_GPR_state(left);
		m_Expectant_GPR.pop();
		arithmetic_operation_code_generator(oper_1, type, left, right, code_stream);
	}
	else if (is_Function(node))
	{
		std::stack<E_AR> temp_ar;
		std::stack<E_GPR> temp_gpr;
		m_Alocator.push_register(temp_gpr, temp_ar,code_stream);
		std::string name = std::any_cast<S_FuncCall>(node).m_FuncName;
		function_call(m_Alocator.find_empty_AR(),name,code_stream);
		E_GPR left = m_Expectant_GPR.top();
		m_Expectant_GPR.pop();
		arithmetic_operation_code_generator(oper_1, E_Type::DWORD, left, E_GPR::R0, code_stream);
		code_stream << m_Command_generator.move(E_GPR(temp_gpr.top() + 4), E_GPR::R0) << std::endl;
		m_Alocator.pop_register(temp_gpr, temp_ar, code_stream);
	}
}


// case from number and oter operands
void  C_CodeGenerator::case_number_other_operand(std::stringstream& current_stream) noexcept
{
	if (is_Operator(m_Operand_stack.top()))
	{
		case_operator();
	}
	else if (is_Function(m_Operand_stack.top()))
	{
		std::any function = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_number_function(m_Operand_stack.top(), m_PostorderToken.back(), function, current_stream);
		case_pop_operand();
	}
	else if (is_Variable(m_Operand_stack.top()))
	{
		std::any variable = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_number_variable(m_Operand_stack.top(), m_PostorderToken.back(), variable, current_stream);
		case_pop_operand();
	}
	else if (is_Number(m_Operand_stack.top()))
	{
		std::any number = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_number_number(m_Operand_stack.top(), m_PostorderToken.back(), number, current_stream);
		case_pop_operand();
	}
}

// case from function and other  opernads
void  C_CodeGenerator::case_function_other_operand(std::stringstream& current_stream) noexcept
{
	if (is_Operator(m_Operand_stack.top()))
	{
		case_operator();
	}
	else if (is_Function(m_Operand_stack.top()))
	{
		std::any function = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_function_function(m_Operand_stack.top(), m_PostorderToken.back(), function, current_stream);
		case_pop_operand();
	}
	else if (is_Variable(m_Operand_stack.top()))
	{
		std::any variable = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_function_variable(m_Operand_stack.top(), m_PostorderToken.back(), variable, current_stream);
		case_pop_operand();
	}
	else if (is_Number(m_Operand_stack.top()))
	{
		std::any number = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_function_number(m_Operand_stack.top(), m_PostorderToken.back(), number, current_stream);
		case_pop_operand();
	}
}

// case  from variable  and other  opernads
void  C_CodeGenerator::case_variable_other_operand(std::stringstream& current_stream) noexcept
{
	if (is_Operator(m_Operand_stack.top()))
	{
		case_operator();
	}
	else if (is_Function(m_Operand_stack.top()))
	{
		std::any function = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_variable_function(m_Operand_stack.top(), m_PostorderToken.back(), function, current_stream);
		case_pop_operand();
	}
	else if (is_Variable(m_Operand_stack.top()))
	{
		std::any variable = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_variable_variable(m_Operand_stack.top(), m_PostorderToken.back(), variable, current_stream);
		case_pop_operand();
	}
	else if (is_Number(m_Operand_stack.top()))
	{
		std::any number = m_Operand_stack.top();
		m_Operand_stack.pop();
		case_variable_number(m_Operand_stack.top(), m_PostorderToken.back(), number, current_stream);
		case_pop_operand();
	}
}

// case  from  two number operation
void  C_CodeGenerator::case_number_number(const std::any& oper, const std::any& left, 
										  const std::any& right, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//get  operators  left  value and  value  type
	int left_num = std::any_cast<S_NumToken>(left).m_Value;
	E_Type left_num_type = std::any_cast<S_NumToken>(left).m_Type;
	//get  operators  right value and value type
	int right_num = std::any_cast<S_NumToken>(right).m_Value;
	E_Type right_num_type = std::any_cast<S_NumToken>(right).m_Type;
	// alocate GPR from  work
	E_GPR left_reg = m_Alocator.find_empty_GPR_in_input_type(left_num_type);
	m_Alocator.change_GPR_state(left_reg);
	E_GPR right_reg = m_Alocator.find_empty_GPR_in_input_type(right_num_type);
	m_Alocator.change_GPR_state(right_reg);
	//generat register assignment operation code
	code_stream << m_Command_generator.assign(get_operation_type(left_num_type), left_reg, left_num) << std::endl;
	code_stream << m_Command_generator.assign(get_operation_type(right_num_type), right_reg, right_num) << std::endl;
	// select  big type  from work
	E_Type type = left_num_type < right_num_type ? right_num_type : left_num_type;
	// generate  finale  operation code
	arithmetic_operation_code_generator(_operator, type, left_reg, right_reg, code_stream);
}


// case from number  and  variable
void  C_CodeGenerator::case_number_variable(const std::any& oper, const std::any& number, 
											const std::any& variable, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//get  operators  left  value and  value  type
	int num = std::any_cast<S_NumToken>(number).m_Value;
	E_Type type = std::any_cast<S_NumToken>(number).m_Type;
	E_Type type_1 = E_Type::DWORD;
	//alocate  GPR from work
	E_GPR  left_reg = m_Alocator.find_empty_GPR_in_input_type(type);
	E_GPR  right_reg = m_Alocator.find_empty_GPR_in_input_type(type_1);
	//reserving AR and GPR
	m_Alocator.change_GPR_state(left_reg);
	m_Alocator.change_GPR_state(right_reg);
	code_stream << m_Command_generator.assign(get_operation_type(type), left_reg, num) << std::endl;
	if (m_IsFunction && is_LocalVariable(variable))
	{
		load_local_argument(variable, code_stream);
	}
	else
	{
		//get  operators  right value 
		std::string var = std::any_cast<S_VarToken>(variable).m_VariableName;
		//alocate AR from loading
		E_AR   address = m_Alocator.find_empty_AR();
		code_stream << m_Command_generator.assign(address, var) << std::endl;
		code_stream << m_Command_generator.load(right_reg, address) << std::endl;
	}
	arithmetic_operation_code_generator(_operator, type_1, left_reg, right_reg, code_stream);
}


void  C_CodeGenerator::case_number_function(const std::any& oper, const std::any& number,
											const std::any& function, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//get  operators  left  value and  value  type
	int num = std::any_cast<S_NumToken>(number).m_Value;
	E_Type type = std::any_cast<S_NumToken>(number).m_Type;
	//get  operators  right value 
	std::string func = std::any_cast<S_FuncCall>(function).m_FuncName;
	/*-------------------------*/
	std::stack<E_GPR> temp_gpr;
	std::stack<E_AR> temp_ar;
	E_GPR left_reg = m_Alocator.find_empty_GPR_in_input_type(type);
	E_AR address = m_Alocator.find_empty_AR();
	/*-------------------------------------------------------*/
	m_Alocator.push_register(temp_gpr, temp_ar, code_stream);
		
	code_stream << m_Command_generator.assign(get_operation_type(type), left_reg, num) << std::endl;
	code_stream << m_Command_generator.push(left_reg) << std::endl;
	temp_gpr.push(left_reg);

	if (is_NoneArgumentalFunction(function))
	{
		none_Argumental_funcion_call(_operator, address, func, temp_gpr, code_stream);
	}
	else
	{
		function_argument_push(function, code_stream);
		none_Argumental_funcion_call(_operator, address, func, temp_gpr, code_stream);
		int  sum = std::any_cast<S_FuncCall>(function).m_FuncArgs.size();
		E_Type tp = E_Type::DWORD;
		code_stream << m_Command_generator.assign("DW", m_Alocator.find_empty_GPR_in_input_type(tp), sum) << std::endl;
		m_Alocator.change_GPR_state(m_Alocator.find_empty_GPR_in_input_type(tp));
	}
	m_Alocator.pop_register(temp_gpr, temp_ar, code_stream);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

// case  from variable  and number
void  C_CodeGenerator::case_variable_number(const std::any& oper, const std::any& variable,
											const std::any& number, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//get  operators  right value 
	int num = std::any_cast<S_NumToken>(number).m_Value;
	E_Type type = std::any_cast<S_NumToken>(number).m_Type;
	E_Type type_1 = E_Type::DWORD;
	E_GPR  right_reg = m_Alocator.find_empty_GPR_in_input_type(type);
	m_Alocator.change_GPR_state(right_reg);
	code_stream << m_Command_generator.assign(get_operation_type(type_1), right_reg, num) << std::endl;
	E_GPR  left_reg = m_Alocator.find_empty_GPR_in_input_type(type_1);
	m_Alocator.change_GPR_state(left_reg);
	//get  operators  left  value and  value  type
	if (m_IsFunction && is_LocalVariable(variable))
	{
		load_local_argument(variable, code_stream);
	}
	else
	{
		std::string var = std::any_cast<S_VarToken>(variable).m_VariableName;
		//alocate AR from loading
		E_AR   address = m_Alocator.find_empty_AR();
		//reserving and GPR
		code_stream << m_Command_generator.assign(address, var) << std::endl;
		code_stream << m_Command_generator.load(left_reg, address) << std::endl;
	}
	arithmetic_operation_code_generator(_operator, type_1, left_reg, right_reg, code_stream);
}


// case  from  two  variable
void  C_CodeGenerator::case_variable_variable(const std::any& oper, const std::any& variable_1,
											  const std::any& variable_2, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//get  operators  left  value and  value  type
	std::string var_1 = std::any_cast<S_VarToken>(variable_1).m_VariableName;
	E_Type type_1 = E_Type::DWORD;
	//get  operators  right value 
	std::string var_2 = std::any_cast<S_VarToken>(variable_2).m_VariableName;
	E_Type type_2 = E_Type::DWORD;
	//alocate  GPR from work
	E_GPR  left_reg = m_Alocator.find_empty_GPR_in_input_type(type_1);
	m_Alocator.change_GPR_state(left_reg);
	E_GPR  right_reg = m_Alocator.find_empty_GPR_in_input_type(type_2);
	m_Alocator.change_GPR_state(right_reg);
	//alocate AR from loading
	E_AR   address_1 = m_Alocator.find_empty_AR();
	m_Alocator.change_AR_state(address_1);
	E_AR   address_2 = m_Alocator.find_empty_AR();
	if (m_IsFunction  && is_LocalVariable(variable_1))
	{
		load_local_argument(variable_1, code_stream);
	}
	else
	{
		code_stream << m_Command_generator.assign(address_1, var_1) << std::endl;
		code_stream << m_Command_generator.load(left_reg, address_1) << std::endl;
	}
	if (m_IsFunction  && is_LocalVariable(variable_2))
	{
		load_local_argument(variable_2, code_stream);
	}
	else 
	{
		code_stream << m_Command_generator.assign(address_2, var_2) << std::endl;
		code_stream << m_Command_generator.load(right_reg, address_2) << std::endl;
	}
	m_Alocator.change_AR_state(address_1);
	arithmetic_operation_code_generator(_operator, type_1, left_reg, right_reg, code_stream);
}


// generate  code  from variable  and function
void  C_CodeGenerator::case_variable_function(const std::any& oper, const std::any& variable,
	const std::any& function, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	E_AR address = m_Alocator.find_empty_AR();
	E_Type type = E_Type::DWORD;
	//get  operators  left  value and  value  type
	if (m_IsFunction  && is_LocalVariable(variable))
	{
		load_local_argument(variable, code_stream);
	}
	else
	{
		std::string var = std::any_cast<S_VarToken>(variable).m_VariableName;
		//get function name and other atributs
		code_stream << m_Command_generator.assign(address, var) << std::endl;
		E_GPR var_value = m_Alocator.find_empty_GPR_in_input_type(type);
		code_stream << m_Command_generator.load(var_value, address);
		m_Alocator.change_GPR_state(var_value);
	}
	std::string func = std::any_cast<S_FuncCall>(function).m_FuncName;
	// alocate  registers  from work
	std::stack<E_GPR> temp_gpr;
	std::stack<E_AR> temp_ar;
	m_Alocator.push_register(temp_gpr, temp_ar, code_stream);
	/*-------------------------------------------------------------*/
	if (is_NoneArgumentalFunction(function))
	{
		none_Argumental_funcion_call(_operator, address, func, temp_gpr, code_stream);
	}
	else
	{
		function_argument_push(function, code_stream);
		none_Argumental_funcion_call(_operator, address, func, temp_gpr, code_stream);
		int sum = std::any_cast<S_FuncCall>(function).m_FuncArgs.size();
		code_stream << m_Command_generator.assign("DW", m_Alocator.find_empty_GPR_in_input_type(type), sum) << std::endl;
		code_stream << "SUB  A0 , R" + std::to_string(m_Alocator.find_empty_GPR_in_input_type(type)) + " , A0 " << std::endl;
	}
	m_Alocator.pop_register(temp_gpr, temp_ar, code_stream);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

// generate code from function and number
void  C_CodeGenerator::case_function_number(const std::any& oper, const std::any& function, 
											const std::any& number, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//push registers
	std::stack<E_GPR> temp_gpr;
	std::stack<E_AR> temp_ar;
	///////////////////////////////////////////
	E_Type type = std::any_cast<S_NumToken>(number).m_Type;
	E_GPR reg = m_Alocator.find_empty_GPR_in_input_type(type);
	m_Alocator.push_register(temp_gpr, temp_ar, code_stream);
	//get function name and other atributs
	std::string func = std::any_cast<S_FuncCall>(function).m_FuncName;
	E_AR address = m_Alocator.find_empty_AR();
	//get number 
	int num = std::any_cast<S_NumToken>(number).m_Value;
	// generate  call code and  reserving R0
	function_argument_push(function, code_stream);
	function_call(address, func, code_stream);
	if (!is_NoneArgumentalFunction(function))
	{
		int sum = std::any_cast<S_FuncCall>(function).m_FuncArgs.size();
		code_stream << m_Command_generator.assign("DW", m_Alocator.find_empty_GPR_in_input_type(type), sum) << std::endl;
		code_stream << "SUB  A0 , R" + std::to_string(m_Alocator.find_empty_GPR_in_input_type(type)) + " , A0 " << std::endl;
	}
	m_Alocator.change_GPR_state(E_GPR::R0);
	//write  in  register  he  are number
	code_stream << m_Command_generator.assign(get_operation_type(type), reg, num) << std::endl;
	m_Alocator.change_GPR_state(reg);
	// genreation  arithmetic  operation code
	arithmetic_operation_code_generator(_operator, type, E_GPR::R0, reg, code_stream);
	code_stream << m_Command_generator.move(temp_gpr.top(), E_GPR::R0) << std::endl;
	// swap  registers  and values
	m_Expectant_GPR.pop();
	m_Expectant_GPR.push(temp_gpr.top());
	m_Alocator.change_GPR_state(temp_gpr.top());
	temp_gpr.pop();
	m_Alocator.pop_register(temp_gpr, temp_ar, code_stream);
}


// genreate code  from function and  variable
void  C_CodeGenerator::case_function_variable(const std::any& oper, const std::any& function,
											  const std::any& variable, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//push registers
	std::stack<E_GPR> temp_gpr;
	std::stack<E_AR> temp_ar;
	///////////////////////////////////////////
	E_Type type = E_Type::DWORD;
	E_GPR reg = m_Alocator.find_empty_GPR_in_input_type(type);
	m_Alocator.push_register(temp_gpr, temp_ar, code_stream);
	//get function name and other atributs
	std::string func = std::any_cast<S_FuncCall>(function).m_FuncName;
	E_AR address = m_Alocator.find_empty_AR();
	// call and  reserving R0
	function_argument_push(function, code_stream);
	function_call(address, func, code_stream);
	m_Alocator.change_GPR_state(E_GPR::R0);
	if (!is_NoneArgumentalFunction(function))
	{
		int sum = std::any_cast<S_FuncCall>(function).m_FuncArgs.size();
		code_stream << m_Command_generator.assign("DW", m_Alocator.find_empty_GPR_in_input_type(type), sum) << std::endl;
		code_stream << "SUB  A0 , R" + std::to_string(m_Alocator.find_empty_GPR_in_input_type(type)) + " , A0 " << std::endl;
	}
	// get  variable  name 
	if (m_IsFunction && is_LocalVariable(variable))
	{
		load_local_argument(variable, code_stream);
	}
	else
	{
		std::string var = std::any_cast<S_VarToken>(variable).m_VariableName;
		//get variable value
		code_stream << m_Command_generator.assign(address, var) << std::endl;
		code_stream << m_Command_generator.load(reg, address);
	}
	/*---------------------------------------------------*/
	arithmetic_operation_code_generator(_operator, type, E_GPR::R0, reg, code_stream);
	// swap  registers  and values
	m_Expectant_GPR.pop();
	m_Expectant_GPR.push(temp_gpr.top());
	m_Alocator.change_GPR_state(temp_gpr.top());
	temp_gpr.pop();
	m_Alocator.pop_register(temp_gpr, temp_ar, code_stream);
}


// genrate code from two  function case
void  C_CodeGenerator::case_function_function(const std::any& oper, const std::any& function_1,
											  const std::any& function_2, std::stringstream& code_stream) noexcept
{
	//get  operator 
	char _operator = std::any_cast<S_OperToken>(oper).m_Operator;
	//push registers
	std::stack<E_GPR> temp_gpr;
	std::stack<E_AR> temp_ar;
	///////////////////////////////////////////
	m_Alocator.push_register(temp_gpr, temp_ar, code_stream);
	//get function name and other atributs
	std::string func_1 = std::any_cast<S_FuncCall>(function_1).m_FuncName;
	E_AR address = m_Alocator.find_empty_AR();
	// call and  reserving R0
	function_argument_push(function_1, code_stream);
	function_call(address, func_1, code_stream);
	if (!is_NoneArgumentalFunction(function_1))
	{
		int sum = std::any_cast<S_FuncCall>(function_1).m_FuncArgs.size();
		E_Type type = E_Type::DWORD;
		code_stream << m_Command_generator.assign("DW", m_Alocator.find_empty_GPR_in_input_type(type), sum) << std::endl;
		code_stream << "SUB  A0 , R" + std::to_string(m_Alocator.find_empty_GPR_in_input_type(type)) + " , A0 " << std::endl;
	}
	code_stream << m_Command_generator.push(E_GPR::R0) << std::endl;
	temp_gpr.push(E_GPR::R0);
	m_Alocator.change_GPR_state(E_GPR::R0);
	std::string func_2 = std::any_cast<S_FuncCall>(function_2).m_FuncName;
	if (is_NoneArgumentalFunction(function_2))
	{
		none_Argumental_funcion_call(_operator, address, func_2, temp_gpr, code_stream);
		m_Alocator.pop_register(temp_gpr, temp_ar, code_stream);
	}
	else
	{
		none_Argumental_funcion_call(_operator, address, func_2, temp_gpr, code_stream);
		int sum = std::any_cast<S_FuncCall>(function_1).m_FuncArgs.size();
		E_Type type = E_Type::DWORD;
		code_stream << m_Command_generator.assign("DW", m_Alocator.find_empty_GPR_in_input_type(type), sum);
		code_stream << "SUB  A0 , R" + std::to_string(m_Alocator.find_empty_GPR_in_input_type(type)) + " , A0 " << std::endl;
		m_Alocator.pop_register(temp_gpr, temp_ar, code_stream);
	}
}

/*########################### END OPERAION CASE PART ############################*/

/*############################## FINALLY CASE PART ##############################*/


void  C_CodeGenerator::case_register_register(const std::any& oper, std::stringstream& code_stream) noexcept
{
	// get our  registers
	E_GPR right = m_Expectant_GPR.top();
	m_Expectant_GPR.pop();
	E_GPR left = m_Expectant_GPR.top();
	m_Expectant_GPR.pop();
	//get  operator
	char operator_1 = std::any_cast<S_OperToken>(oper).m_Operator;
	arithmetic_operation_code_generator(operator_1, get_register_type(left > right ? left : right), left, right, code_stream);
}


void  C_CodeGenerator::case_assigment_operands(std::stringstream& code_stream, const bool& is_function)
{
	if (is_Variable(m_PostorderToken[1]))
	{
		E_AR address = m_Alocator.find_empty_AR();
		std::any  variable = m_PostorderToken[1];
		if (is_function  && is_LocalVariable(variable))
		{
			load_local_argument(variable, code_stream);
		}
		else
		{
			std::string var = std::any_cast<S_VarToken>(variable).m_VariableName;
			code_stream << m_Command_generator.assign(address, var) << std::endl;
			code_stream << m_Command_generator.load(E_GPR::R0, address) << std::endl;
		}
	}
	else if (is_Function(m_PostorderToken[1]))
	{
		E_AR address = m_Alocator.find_empty_AR();
		std::any function = m_PostorderToken[1];
		std::string func = std::any_cast<S_FuncCall>(function).m_FuncName;
		function_argument_push(function, code_stream);
		function_call(address, func, code_stream);
		if (!is_NoneArgumentalFunction(function))
		{
			int sum = std::any_cast<S_FuncCall>(function).m_FuncArgs.size();
			E_Type type = E_Type::DWORD;
			code_stream << m_Command_generator.assign("DW", m_Alocator.find_empty_GPR_in_input_type(type), sum);
			code_stream << "SUB  A0 , R" + std::to_string(m_Alocator.find_empty_GPR_in_input_type(type)) + " , A0 " << std::endl;
		}
	}
	else if (is_Number(m_PostorderToken[1]))
	{
		std::any number = m_PostorderToken[1];
		int num = std::any_cast<S_NumToken>(number).m_Value;
		E_Type  type = std::any_cast<S_NumToken>(number).m_Type;
		code_stream << m_Command_generator.assign(get_operation_type(type), E_GPR::R0, num) << std::endl;
	}
	else
		throw C_Compile_Error(" UNDEFINDE  SYMBOLE ", m_Line_number);
	if (is_function)
	{
		code_stream << m_Command_generator.pop() << std::endl;
		code_stream << m_Command_generator.ret() << std::endl;
	}
	else
	{
		std::any var = m_PostorderToken.front();
		std::string name = std::any_cast<S_VarToken>(var).m_VariableName;
		code_stream << m_Command_generator.assign(E_AR::A2, name) << std::endl;
		code_stream << m_Command_generator.store(E_AR::A2, E_GPR::R0) << std::endl << std::endl;
	}
	m_PostorderToken.clear();
}


// final case
void  C_CodeGenerator::case_asigment_register(std::stringstream& code_stream, const bool& is_function) noexcept
{
	if (is_function)
	{
		if (m_Expectant_GPR.top() == E_GPR::R0)
		{
			code_stream << m_Command_generator.pop() << std::endl;
			code_stream << m_Command_generator.ret() << std::endl << std::endl;
		}
		else
		{
			code_stream << m_Command_generator.move(E_GPR::R0, m_Expectant_GPR.top()) << std::endl;
			code_stream << m_Command_generator.pop() << std::endl;
			code_stream << m_Command_generator.ret() << std::endl << std::endl;
		}
		if (!m_Expectant_GPR.empty())
			m_Expectant_GPR.pop();
		if (!m_Operand_stack.empty())
			m_Operand_stack.pop();
		if (!m_PostorderToken.empty())
		m_PostorderToken.pop_back();
	}
	else
	{
		std::string name = std::any_cast<S_VarToken>(m_PostorderToken.front()).m_VariableName;
		code_stream << m_Command_generator.assign(E_AR::A2, name) << std::endl;
		code_stream << m_Command_generator.store(E_AR::A2, m_Expectant_GPR.top()) << std::endl << std::endl;
		m_Expectant_GPR.pop();
		m_Operand_stack.pop();
		m_PostorderToken.pop_back();
	}
}

/*############################ END FINALLY CASE PART ############################*/

