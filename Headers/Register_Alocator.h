#pragma once
#include <map>
#include <stack>
#include <sstream>
#include "GeneratorHelper.h"
#include "Types.h"
#include "Compile_Error.h"
#include "Address_Registers.h"
#include "General_Porpouse_Registers.h"

typedef std::map<E_GPR, bool>::const_iterator  Const_GPR_Iterator;
typedef std::map<E_GPR, bool>::iterator		   GPR_Iterator;
typedef std::map<E_AR,  bool>::const_iterator  Const_AR_Iterator;
typedef std::map<E_AR,  bool>::iterator		   AR_Iterator;

class C_Register_Alocator
{
public:
	C_Register_Alocator();
	C_Register_Alocator(const C_Register_Alocator&);
	virtual ~C_Register_Alocator();
	/*-----------------------------------------------------------*/
	E_GPR find_empty_GPR_in_input_type(E_Type&) const noexcept;
	E_AR  find_empty_AR() const noexcept;
	void  change_GPR_state(E_GPR) noexcept;
	void  change_AR_state(E_AR) noexcept;
	void  update_GPA_and_AR() noexcept;
	/*------------------------------------------------------------------------------------*/
	void  push_register(std::stack<E_GPR>&, std::stack<E_AR>&, std::stringstream&) noexcept;
	void  pop_register(std::stack<E_GPR>&, std::stack<E_AR>&, std::stringstream&) noexcept;

private:
	C_CommandGeneratorHelper m_Generator;

	std::map<E_GPR, bool>  m_GPR_name_and_state
	{
		std::make_pair(E_GPR::R0,  false),
		std::make_pair(E_GPR::R1,  false),
		std::make_pair(E_GPR::R2,  false),
		std::make_pair(E_GPR::R3,  false),
		std::make_pair(E_GPR::R4,  false),
		std::make_pair(E_GPR::R5,  false),
		std::make_pair(E_GPR::R6,  false),
		std::make_pair(E_GPR::R7,  false),
		std::make_pair(E_GPR::R8,  false),
		std::make_pair(E_GPR::R9,  false),
		std::make_pair(E_GPR::R10, false),
		std::make_pair(E_GPR::R11, false),
		std::make_pair(E_GPR::R12, false),
		std::make_pair(E_GPR::R13, false),
		std::make_pair(E_GPR::R14, false),
		std::make_pair(E_GPR::R15, false),
		std::make_pair(E_GPR::R16, false),
		std::make_pair(E_GPR::R17, false),
		std::make_pair(E_GPR::R18, false),
		std::make_pair(E_GPR::R19, false),
		std::make_pair(E_GPR::R20, false),
		std::make_pair(E_GPR::R21, false),
		std::make_pair(E_GPR::R22, false),
		std::make_pair(E_GPR::R23, false),
		std::make_pair(E_GPR::R24, false),
		std::make_pair(E_GPR::R25, false),
		std::make_pair(E_GPR::R26, false),
		std::make_pair(E_GPR::R27, false),
		std::make_pair(E_GPR::R28, false),
		std::make_pair(E_GPR::R29, false),
		std::make_pair(E_GPR::R30, false),
		std::make_pair(E_GPR::R31, false),
		std::make_pair(E_GPR::R32, false),
		std::make_pair(E_GPR::R33, false),
		std::make_pair(E_GPR::R34, false),
		std::make_pair(E_GPR::R35, false),
		std::make_pair(E_GPR::R36, false),
		std::make_pair(E_GPR::R37, false),
		std::make_pair(E_GPR::R38, false),
		std::make_pair(E_GPR::R39, false),
		std::make_pair(E_GPR::R40, false),
		std::make_pair(E_GPR::R41, false),
		std::make_pair(E_GPR::R42, false),
		std::make_pair(E_GPR::R43, false),
		std::make_pair(E_GPR::R44, false),
		std::make_pair(E_GPR::R45, false),
		std::make_pair(E_GPR::R46, false),
		std::make_pair(E_GPR::R47, false),
		std::make_pair(E_GPR::R48, false),
		std::make_pair(E_GPR::R49, false),
		std::make_pair(E_GPR::R50, false),
		std::make_pair(E_GPR::R51, false),
		std::make_pair(E_GPR::R52, false),
		std::make_pair(E_GPR::R53, false),
		std::make_pair(E_GPR::R54, false),
		std::make_pair(E_GPR::R55, false),
		std::make_pair(E_GPR::R56, false),
		std::make_pair(E_GPR::R57, false),
		std::make_pair(E_GPR::R58, false),
		std::make_pair(E_GPR::R59, false),
		std::make_pair(E_GPR::R60, false),
		std::make_pair(E_GPR::R61, false),
		std::make_pair(E_GPR::R62, false),
		std::make_pair(E_GPR::R63, false),
		std::make_pair(E_GPR::undefinde,false),
	};
	std::map<E_AR, bool> m_Address_register_and_state
	{
		std::make_pair(E_AR::A2,false),
		std::make_pair(E_AR::A3,false),
		std::make_pair(E_AR::A4,false),
		std::make_pair(E_AR::A5,false),
		std::make_pair(E_AR::A6,false),
		std::make_pair(E_AR::A7,false),
		std::make_pair(E_AR::SP,true),
		std::make_pair(E_AR::SF,true)
	};
};

