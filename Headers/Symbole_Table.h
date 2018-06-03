#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>

typedef std::unordered_map<std::string, int>::const_iterator Tfunc_Iterator;

class C_SymTab
{
public:
	C_SymTab();
	C_SymTab(const C_SymTab&);
	~C_SymTab();

	static  std::vector<std::string>			 m_LocalVariable;

	static  void reset();
	static  void isnert_local(const std::string&);
	static  void insert_var(const std::string&);
	static  void insert_func(const std::string&,int);
	static  bool has_locale(const std::string&) noexcept;
	static  bool has_variable(const std::string&) noexcept;
	static  bool has_function(const std::string&) noexcept;
	static  Tfunc_Iterator get_function_iterator(const std::string&) noexcept;

protected:
	static  std::unordered_set<std::string>		 m_VarSymboleTable;
	static  std::unordered_map<std::string,int>  m_FuncSymboleTable;
};

