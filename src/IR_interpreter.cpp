#include "IR_interpreter.h"
#include "helper.h"
//should read IR code in line-by-line to a vector
void IR_interpreter::read(const std::string file)
{
	std::ifstream f(file);
	std::string line;
    if (!f.is_open())
	{
		Error e({"[read_file]: FAILED TO OPEN FILE \"" + file + "\""});
		throw e;
    }
	else
	{
		while(std::getline(f, line))
			file_content.push_back(line);
	}
	f.close();
}

//should run after read, calling interpreting functions on
//a switch-case
void IR_interpreter::evaluate()
{
	while (file_content[read_head] != "END")
	{
		decide(file_content[read_head]);
		std::cout << "read_head = " << read_head << std::endl; 
		++read_head;
	}
}


//whitespace split of a line into tokens
std::vector< std::string > IR_interpreter::split(const std::string l)
{
	std::vector< std::string > toks;
	std::istringstream ss(l);
	std::string tok;
	while (ss >> tok)
		toks.push_back(tok);
	return toks;
}

//"f(t1, t2)" -> name = "f", args = {"t1", "t2"}
//"main()"    -> name = "main", args = {}
void IR_interpreter::split_args(const std::string s, std::string & name,
								std::vector< std::string > & args)
{
	size_t open = s.find('(');
	size_t close = s.rfind(')');
	if (open == std::string::npos || close == std::string::npos || close < open)
		throw Error("[split_args]: ERROR: line " + std::to_string(read_head)
					+ ": bad argument list \"" + s + "\"");

	std::vector< std::string > n = split(s.substr(0, open));
	if (n.size() != 1)
		throw Error("[split_args]: ERROR: line " + std::to_string(read_head)
					+ ": bad name \"" + s.substr(0, open) + "\"");
	name = n[0];

	args.clear();
	std::string inside = s.substr(open + 1, close - open - 1);
	//no args
	if (split(inside).empty()) 
		return;
	std::istringstream ss(inside);
	std::string arg;
	while (std::getline(ss, arg, ','))
	{
		//trims spaces
		std::vector< std::string > a = split(arg);
		if (a.size() != 1)
			throw Error("[split_args]: ERROR: line " + std::to_string(read_head)
						+ ": bad argument \"" + arg + "\"");
		args.push_back(a[0]);
	}
}

//assign function name to an index in file_content
//FUNCTION f(t1, t2)
IR_func IR_interpreter::func_eval(const std::string l)
{
	IR_func f;
	size_t kw = l.find("FUNCTION");
	if (kw == std::string::npos)
		throw Error("[func_eval]: ERROR: line " + std::to_string(read_head)
					+ ": expected FUNCTION in \"" + l + "\"");
	split_args(l.substr(kw + 8), f.name, f.params);
	return f;
}

//if/then/else forks into two possible labels, moving read space
//IF t1 relop t2 THEN l1 ELSE l2
IR_cond IR_interpreter::cond_eval(const std::string l)
{
	std::vector< std::string > t = split(l);
	if (t.size() != 8 || t[0] != "IF" || t[4] != "THEN" || t[6] != "ELSE")
		throw Error("[cond_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad conditional \"" + l + "\"");
	IR_cond c;
	c.lhs = t[1];
	c.relop = t[2];
	c.rhs = t[3];
	c.then_label = t[5];
	c.else_label = t[7];
	return c;
}

//jumps to a label and back again
//t1 := CALL f(t2, t3)
IR_call IR_interpreter::call_eval(const std::string l)
{
	std::vector< std::string > t = split(l);
	size_t kw = l.find("CALL");
	if (t.size() < 4 || t[1] != ":=" || t[2] != "CALL" || kw == std::string::npos)
		throw Error("[call_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad call \"" + l + "\"");
	IR_call c;
	c.dest = t[0];
	split_args(l.substr(kw + 4), c.func, c.args);
	return c;
}

//RETURN t1
IR_return IR_interpreter::return_eval(const std::string l)
{
	std::vector< std::string > t = split(l);
	if (t.size() != 2 || t[0] != "RETURN")
		throw Error("[return_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad return \"" + l + "\"");
	IR_return r;
	r.val = t[1];
	return r;
}

//t1 := t2 | t1 := - t2 | t1 := t2 op t3
IR_assign IR_interpreter::assign_eval(const std::string l)
{
	std::vector< std::string > t = split(l);
	if (t.size() < 3 || t[1] != ":=")
		throw Error("[assign_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad assignment \"" + l + "\"");
	IR_assign a;
	a.dest = t[0];
	if (t.size() == 3) //copy
		a.lhs = t[2];
	else if (t.size() == 4 && t[2] == "-") //unary
	{
		a.op = t[2];
		a.rhs = t[3];
	}
	else if (t.size() == 5) //binop
	{
		a.lhs = t[2];
		a.op = t[3];
		a.rhs = t[4];
	}
	else
		throw Error("[assign_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad assignment \"" + l + "\"");
	//div by zero check
	return a;
}

//LABEL l1
IR_label IR_interpreter::label_eval(const std::string l)
{
	std::vector< std::string > t = split(l);
	if (t.size() != 2 || t[0] != "LABEL")
		throw Error("[label_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad label \"" + l + "\"");
	IR_label lb;
	lb.name = t[1];
	return lb;
}

//GOTO l1
IR_goto IR_interpreter::goto_eval(const std::string l)
{
	std::vector< std::string > t = split(l);
	if (t.size() != 2 || t[0] != "GOTO")
		throw Error("[goto_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad goto \"" + l + "\"");
	IR_goto g;
	g.label = t[1];
	return g;
}

//set read head to position from label
void IR_interpreter::jump(const size_t i)
{
	read_head = i;
}

 size_t IR_interpreter::get_label_pos(const std::string l)
{
	auto label = labels.find(l);
	if (label == labels.end())
		throw Error("[get_label_pos]: ERROR: line " + std::to_string(read_head));
	else
		return labels[l];
}

void IR_interpreter::add_reg(const std::string r, const std::string val)
{
	auto reg = vtable.find(r);
	if (reg == vtable.end())
		vtable.insert({r, val});
	else
		vtable[r] = val;
}

void IR_interpreter::add_label(const std::string l)
{
	labels.insert({l, read_head});
}

//MAKE THIS A SWITCH STATEMENT LATER
void IR_interpreter::decide(const std::string line)
{
	std::string tok = line.substr(0, line.find(' '));
	std::cout << " tok = " << tok << std::endl;
    if (tok == "FUNCTION") //function header
		func_eval(line);
	else if (tok == "LABEL") //label
		label_eval(line);
	else if (tok == "GOTO") //jump
		goto_eval(line);
	else if (tok[0] == 't') //register
	{
		std::vector< std::string > t = split(line);
		if (t.size() > 2 && t[2] == "CALL") //t1 := CALL f(...)
			call_eval(line);
		else
			assign_eval(line);
	}
	else if (tok == "IF") //conditional
		cond_eval(line);
	else if (tok == "RETURN") //return
		return_eval(line);
	else
		throw Error("[decide]: ERROR: line " + std::to_string(read_head) + ": no match for \"" + tok + "\"");
	// switch (tok)
	// {
	// case: //function header
	// case: //label
	// case: //register
	// case: //conditional
	// case: //jump
	// case: //return
	// }
}

bool IR_interpreter::is_register(const std::string line)
{
	return (vtable.find(line) != vtable.end());
}
