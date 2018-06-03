#pragma once
#include <string>
#include "Address_Registers.h"
#include "General_Porpouse_Registers.h"


class C_CommandGeneratorHelper
{
public:
	const  std::string  byte(const std::string& var) const noexcept
	{
		return std::move("BYTE  " + var);
	}
	const  std::string  word(const std::string& var) const noexcept
	{
		return std::move("WORD  " + var);
	}
	const  std::string  dword(const std::string& var) const noexcept
	{
		return std::move("DWORD  " + var);
	}
	const  std::string  qword(const std::string& var) const noexcept
	{
		return std::move("QWORD  " + var);
	}
	const  std::string  data() const noexcept
	{
		return std::move(".data ");
	}
	const  std::string  code() const noexcept
	{
		return std::move(".code");
	}
	const  std::string  start() const noexcept
	{
		return std::move("JUMP  START");
	}
	const  std::string  call(const E_AR& reg)  const  noexcept 
	{
		return std::move("CALL  A" + std::to_string(reg));
	}
	const  std::string  assign(const E_AR& reg, const std::string& name) const  noexcept
	{
		return std::move("ASSIGN  A" + std::to_string(reg) + " , " + name);
	}
	const  std::string  assign(const std::string& type, const E_GPR& reg, const int& digit) const noexcept
	{
		return std::move("ASSIGN  " + type + " R" + std::to_string(reg) + " , " + std::to_string(digit));
	}
	const  std::string  store(const E_AR& areg, const E_GPR& vreg) const noexcept
	{
		return std::move("STORE  R" + std::to_string(vreg) + " , A" + std::to_string(areg));
	}
	const  std::string  load(const E_GPR& reg, const E_AR& areg) const noexcept
	{
		return std::move("LOAD  R" + std::to_string(reg) + " , A" + std::to_string(areg));
	}
	const  std::string  move() const noexcept
	{
		return std::move("MOVE  A0 , A1");
	}
	const  std::string  move(const E_AR& ar, const E_GPR& gpr) const noexcept
	{
		return std::move("MOVE  A" + std::to_string(ar) + " , R" + std::to_string(gpr));
	}
	const  std::string  move(const E_GPR& left, const E_GPR& right) const noexcept
	{
		return std::move("MOVE  R" + std::to_string(left) + " , R" + std::to_string(right));
	}
	const  std::string  add(const std::string& type, const E_GPR& left, const E_GPR& right) const noexcept
	{
		E_GPR reg = left < right ? left : right;
		return std::move("ADD " + type + " R" + std::to_string(left) + " , R" + std::to_string(right) + " , R" + std::to_string(reg));
	}
	const  std::string  sub(const std::string& type, const E_GPR& left, const E_GPR& right) const noexcept
	{
		E_GPR reg = left < right ? left : right;
		return std::move("SUB " + type + " R" + std::to_string(left) + " , R" + std::to_string(right) + " , R" + std::to_string(reg));
	}	
	const  std::string  mul(const std::string& type, const E_GPR& left, const E_GPR& right) const noexcept
	{
		E_GPR reg = left < right ? left : right;
		return std::move("MUL " + type + " R" + std::to_string(left) + " , R" + std::to_string(right) + " , R" + std::to_string(reg));
	}
	const  std::string  div(const std::string& type, const E_GPR& left, const E_GPR& right) const noexcept
	{
		E_GPR reg = left < right ? left : right;
		return std::move("DIV " + type + " R" + std::to_string(left) + " , R" + std::to_string(right) + " , R" + std::to_string(reg));
	}
	const  std::string  push(const E_GPR& reg) const noexcept
	{
		return std::move("PUSH  R" + std::to_string(reg));
	}
	const  std::string  push(const E_AR& reg = E_AR::SF) const noexcept
	{
		return std::move("PUSH  A" + std::to_string(reg));
	}
	const  std::string  pop(const E_GPR& reg) const noexcept
	{
		return std::move("POP  R" + std::to_string(reg));
	}
	const  std::string  pop(const E_AR& reg = E_AR::SF) const noexcept
	{
		return std::move("POP  A" + std::to_string(reg));
	}
	const  std::string  label(const std::string& name) const noexcept
	{
		return std::move(name + ":");
	}
	const  std::string  in(const E_GPR& reg) const noexcept
	{
		return std::move("IN  R" + std::to_string(reg) + " , 0");
	}
	const  std::string  out(const E_GPR& reg) const noexcept
	{
		return std::move("OUT  R" + std::to_string(reg) + " , 1");
	}
	const  std::string  ret() const  noexcept
	{
		return std::move("RET");
	}
	const  std::string  exit() const noexcept
	{
		return std::move("EXIT");
	}
};