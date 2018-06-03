#include "Symbole_Table.h"



C_SymTab::C_SymTab()
{
}


C_SymTab::C_SymTab(const C_SymTab&)
{
}


C_SymTab::~C_SymTab()
{
}


//locale  varible  symbole  table
std::vector<std::string> C_SymTab::m_LocalVariable;
//variable  symbole  table
std::unordered_set<std::string> C_SymTab::m_VarSymboleTable;
//function  symbole  table
std::unordered_map<std::string, int>  C_SymTab::m_FuncSymboleTable;

//reset  locale  symbole table
void C_SymTab::reset()
{
	m_LocalVariable.clear();
}

//insert locale  argument
void C_SymTab::isnert_local(const std::string& var_name)
{
	m_LocalVariable.emplace_back(var_name);
}

//insert   varible  name 
void C_SymTab::insert_var(const std::string& var_name)
{
	m_VarSymboleTable.insert(var_name);
}


//insert   function  name 
void C_SymTab::insert_func(const std::string& func_name,int func_argCount)
{
	m_FuncSymboleTable.insert(std::make_pair(func_name,func_argCount));
}


bool C_SymTab::has_locale(const std::string& var_name) noexcept
{
	for (const auto& element : C_SymTab::m_LocalVariable)
	{
		if (element == var_name)
			return true;
	}
	return false;
}


//returned  symbole  table  has a  this  variable  or  not  
bool C_SymTab::has_variable(const std::string& var_name) noexcept
{
	std::unordered_set<std::string>::iterator iter = m_VarSymboleTable.find(var_name);
	if (iter == m_VarSymboleTable.end())
		return false;
	return  true;
}

//returned  symbole  table  has a  this  function  or  not  
bool C_SymTab::has_function(const std::string& var_name) noexcept
{
	std::unordered_map<std::string,int>::iterator iter = m_FuncSymboleTable.find(var_name);
	if (iter == m_FuncSymboleTable.end())
		return false;
	return  true;
}

//returned   const iterator in function  object
Tfunc_Iterator C_SymTab::get_function_iterator(const std::string& func_name) noexcept
{
	Tfunc_Iterator iter = m_FuncSymboleTable.find(func_name);
	return std::move(iter);
}
