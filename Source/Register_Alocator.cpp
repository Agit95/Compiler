#include "Register_Alocator.h"


//ctor  by  default
C_Register_Alocator::C_Register_Alocator()
{
}


//copy ctor 
C_Register_Alocator::C_Register_Alocator(const C_Register_Alocator&)
{
}


// virtual ~ctor 
C_Register_Alocator::~C_Register_Alocator()
{
}


//alocate  empty  GPR  
E_GPR C_Register_Alocator::find_empty_GPR_in_input_type(E_Type& type) const noexcept
{
	int variable_type;
	// get the number of bytes  the variable type
	switch (type)
	{
	case E_Type::BYTE:  variable_type = 1; break;
	case E_Type::WORD:  variable_type = 2; break;
	case E_Type::DWORD: variable_type = 4; break;
	case E_Type::QWORD: variable_type = 8; break;
	default:            variable_type = 4; break;
	}
	//itrators  begin and  end
	Const_GPR_Iterator iter_begin = m_GPR_name_and_state.begin();
	Const_GPR_Iterator iter_end   = m_GPR_name_and_state.end();

	int n_r = variable_type;
	//get  first   empty  GPR in  type  
	for (; iter_begin != iter_end;)
	{
		if ((static_cast<int>(iter_begin->first) % variable_type) == 0 && iter_begin->second == false)
		{
			return std::move(iter_begin->first);
		}
		//forward   iterator  to  n gives  
		while (n_r != 0)
		{
			++iter_begin;
			--n_r;
		}
		n_r = variable_type;
	}
	// if  dont have  this  type  GPR  return undefinde
	return std::move(E_GPR::undefinde);
}


// returned  first  reserved  register
void C_Register_Alocator::push_register(std::stack<E_GPR>& GPR, std::stack<E_AR>& AR, std::stringstream& code_stream) noexcept
{
	GPR_Iterator iter_begin = m_GPR_name_and_state.begin();
	GPR_Iterator iter_end = m_GPR_name_and_state.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if (iter_begin->second == true)
		{
			GPR.push(iter_begin->first);
			code_stream << m_Generator.push(iter_begin->first) << std::endl;
			change_GPR_state(iter_begin->first);
		}
	}

	/*------------- push  Address  register --------------*/
	AR_Iterator iter_1 = m_Address_register_and_state.begin();
	AR_Iterator iter_2 = m_Address_register_and_state.end();

	++iter_1;
	++iter_1;

	for (; iter_1 != iter_2; ++iter_1)
	{
		if (iter_1->second == true)
		{
			AR.push(iter_1->first);
			code_stream << m_Generator.push(iter_1->first) << std::endl;
			change_AR_state(iter_1->first);
		}
	}
}

//alocate  empty  Address  register
E_AR  C_Register_Alocator::find_empty_AR() const noexcept
{
	Const_AR_Iterator iter_begin = m_Address_register_and_state.begin();
	Const_AR_Iterator iter_end   = m_Address_register_and_state.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		if (iter_begin->second == false)
			return std::move(iter_begin->first);
	}
}


void  C_Register_Alocator::pop_register(std::stack<E_GPR>& GPR, std::stack<E_AR>& AR, std::stringstream& code_stream) noexcept
{
	/*+++++++++++++ GPR +++++++++++++*/
	while (!GPR.empty())
	{
		code_stream << m_Generator.pop(GPR.top()) << std::endl;
		change_GPR_state(GPR.top());
		GPR.pop();
	}
	/*++++++++++ AR +++++++++*/
	while (!AR.empty())
	{
		code_stream << m_Generator.pop(AR.top()) << std::endl;
		change_AR_state(AR.top());
		AR.pop();
	}
}


//changet   the  GPR  state 
void  C_Register_Alocator::change_GPR_state(E_GPR GPR_name) noexcept
{
	GPR_Iterator  iter = m_GPR_name_and_state.find(GPR_name);
	iter->second = !iter->second;
}

//change  the  Address   register  state
void  C_Register_Alocator::change_AR_state(E_AR reg) noexcept
{
	AR_Iterator iter = m_Address_register_and_state.find(reg);
	iter->second = !iter->second;
}

// update  GPR  and  Address  register   state
void  C_Register_Alocator::update_GPA_and_AR() noexcept
{
	//update  AR state  in false
	for (auto& element : m_Address_register_and_state)
	{
		element.second = false;
	}
	//rezevring  BASE ptr  and  STACK ptr  in  ADDRESS  registers
	m_Address_register_and_state.find(E_AR::SF)->second = true;
	m_Address_register_and_state.find(E_AR::SP)->second = true;

	//update GPR state  in false 
	for (auto& element : m_GPR_name_and_state)
	{
		element.second = false;
	}
}