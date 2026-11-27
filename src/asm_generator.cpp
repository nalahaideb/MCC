#include "asm_generator.h"
#include <sstream>
#include "error.h"

//picks the handler for one IR line based off its first token, same layout
//as IR_interpreter::decide then lowers IR code into RISC-V 
std::vector< std::string > ASM_generator::lower(const std::string s)
{
	std::vector< std::string > out;

	//split on whitespace
	//t[0] is the first token
	std::vector< std::string > t;
	std::istringstream in(s);
	for (std::string w; in >> w;)
		t.push_back(w);
	if (t.empty())
		return out;
	const std::string & tok = t[0];

	if (tok == "FUNCTION") //function header
		out.push_back(func(s));
	else if (tok == "END") //function end
		out.push_back(end(s));
	else if (tok == "LABEL") //label
		out.push_back(label(s));
	else if (tok == "GOTO") //jump
		out.push_back(jump(s));
	else if (tok[0] == 't') //register
	{
		if (t.size() > 3 && t[2] == "CALL") //functions
		{
			const std::string f = t[3].substr(0, t[3].find('('));
			//builtins
			if (f == "print")
				print(s);
			else if (f == "read")
				read(s);
			else //user defined function
				out.push_back(call(s));
		}
		else if (t.size() == 3) //direct assignment
			out.push_back(assign(s));
		else if (t.size() == 4) //negation
			out.push_back(neg(s));
		else if (t.size() == 5) //binary operator
			out.push_back(binop(s));
		else
			throw Error("[lower]: ERROR: malformed assignment \"" + s + "\"");
	}
	else if (tok == "IF") //conditional
		out.push_back(boolop(s));
	else if (tok == "RETURN") //return
		out.push_back(ret(s));
	else
		throw Error("[lower]: ERROR: no match for \"" + tok + "\"");

	return out;
}

//t1 := CALL read()
void ASM_generator::read(const std::string s)
{
	const std::string dest = s.substr(0, s.find(' '));
	output.push_back("\tli a7, 5");
	output.push_back("\tecall");
	output.push_back("\tsw a0, " + offset(dest));
}

//t2 := CALL print(t1)
void ASM_generator::print(const std::string s)
{
	const size_t open = s.find('('), close = s.rfind(')');
	if (open == std::string::npos || close == std::string::npos || close <= open + 1)
		throw Error("[print]: ERROR: print takes one argument \"" + s + "\"");
	const std::string a = s.substr(open + 1, close - open - 1);

	if (a[0] == 't') //load from offset if a[0] is a register
		output.push_back("\tlw a0, " + offset(a));
	else //literal
		output.push_back("\tli a0, " + a);
	output.push_back("\tli a7, 1");
	output.push_back("\tecall");
	//prints a newline
	output.push_back("\tli a0, 10");
	output.push_back("\tli a7, 11");
	output.push_back("\tecall");
}
