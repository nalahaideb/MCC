#include "IR_interpreter.h"
#include "helper.h"

//assign the read_head to index of main function
inline void IR_interpreter::find_main()
{
	for (size_t i = 0; i < file_content.size(); ++i)
		if (file_content[i].find("main()") != std::string::npos)
		{
			read_head = i;
			return;
		}
	//this should never be thrown but its here just in case
	throw Error("[find_main]: ERROR: line " + std::to_string(file_content.size())
				+ ": no main function in file");
}

//should read IR code in line-by-line to a vector
void IR_interpreter::read(const std::string file)
{
	std::ifstream f(file);
	std::string line;
    if (!f.is_open())
	{
		throw Error({"[read_file]: ERROR: FAILED TO OPEN FILE \"" + file + "\""});
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
	find_labels();
	find_main();
	while (file_content[read_head] != "END")
	{
		decide(file_content[read_head]);
		//std::cout << "read_head = " << read_head << std::endl;
		++read_head;
	}
	// std::cout << "output results: \n";
	// for (const std::string & o : output)
	// 	std::cout << o << std::endl;
}

//preprocessing step for recording jump points called by jump()
//treats lines with "FUNCTION" the same as a label
void IR_interpreter::find_labels()
{
	size_t func = 0;
	for (size_t i = 0; i < file_content.size(); ++i)
	{
		if (file_content[i].rfind("FUNCTION", 0) == 0)
		{
			func = i;
			read_head = i;
			funcs[func_eval(file_content[i]).name] = i;
		}
		else if (file_content[i].rfind("LABEL", 0) == 0)
		{
			read_head = i;
			add_label(std::to_string(func) + ":"
					  + label_eval(file_content[i]).name);
		}
	}
	read_head = 0;
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
//"main()" -> name = "main", args = {}
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
	size_t k = l.find("FUNCTION");
	if (k == std::string::npos)
		throw Error("[func_eval]: ERROR: line " + std::to_string(read_head)
					+ ": expected FUNCTION in \"" + l + "\"");
	split_args(l.substr(k + 8), f.name, f.params);
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

	//func = func name, args = vector of registers
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
		//div by zero check
		if ((a.op == "/" || a.op == "%") && (a.rhs == "0" || vtable[a.rhs] == "0"))
			throw Error("[assign_eval]: ERROR: line " + std::to_string(read_head)
						+ ": div by zero \"" + l + "\"");
	}
	else
		throw Error("[assign_eval]: ERROR: line " + std::to_string(read_head)
					+ ": bad assignment \"" + l + "\"");
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

//labels restart at l1 in every function, so they're keyed
//"<FUNCTION line>:<label>" and looked up in the current function
 size_t IR_interpreter::get_label_pos(const std::string l)
{
	auto label = labels.find(std::to_string(func_start()) + ":" + l);
	if (label == labels.end())
		throw Error("[get_label_pos]: ERROR: line "
					+ std::to_string(read_head)
					+ ": no such label \"" + l + "\"");
	else
		return label->second;
}

//index of the FUNCTION line the read head is currently inside
size_t IR_interpreter::func_start()
{
	for (size_t i = read_head + 1; i-- > 0; )
		if (file_content[i].rfind("FUNCTION", 0) == 0)
			return i;
	throw Error("[func_start]: ERROR: line " + std::to_string(read_head)
				+ ": not inside a function");
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
	//std::cout << " tok = " << tok << std::endl;
	std::string result = "";
    if (tok == "FUNCTION") //function header
		IR_func f = func_eval(line);
	else if (tok == "LABEL") //label
		IR_label label = label_eval(line);
	else if (tok == "GOTO") //jump
	{
		IR_goto g = goto_eval(line);
		jump(get_label_pos(g.label));
	}
	else if (tok[0] == 't') //register
	{
		std::vector< std::string > t = split(line);
		//t1 := CALL f(...)
		if (t.size() > 2 && t[2] == "CALL")
		{
			IR_call call = call_eval(line);
			if (call.func == "print")
				result = print_call(call);
			else if (call.func == "read")
				read_call(call);
			else //user defined function
				func_call(call);
		}
		else //direct assignment on a register
		{
			IR_assign a = assign_eval(line);
			if (a.op == "")
				if (a.lhs[0] == 't') //register
					vtable[a.dest] = vtable[a.lhs];
				else //literal
					vtable[a.dest] = a.lhs;
			else if (a.lhs == "") //unary minus
				vtable[a.dest] = binop_result("0", "-", a.rhs);
			else
				vtable[a.dest] = binop_result(a.lhs, a.op, a.rhs);
		}
	}
	else if (tok == "IF") //conditional
	{
		IR_cond cond = cond_eval(line);
		cond_result(cond.lhs, cond.relop, cond.rhs,
					cond.then_label, cond.else_label);
	}
	else if (tok == "RETURN") //return
	{
		IR_return ret = return_eval(line);
		return_call(ret);
	}
	else
		throw Error("[decide]: ERROR: line " + std::to_string(read_head)
					+ ": no match for \"" + tok + "\"");
	if (result != "")
		output.push_back(result);
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

std::string IR_interpreter::binop_result(const std::string opr0,
								  const std::string operator_,
								  const std::string opr1)
{
	long long left, right, eval;
	if (opr0.find('t') == std::string::npos)
	    left = std::stoi(opr0);
	else
		left = std::stoi(vtable[opr0]);
	if (opr1.find('t') == std::string::npos)
	    right = std::stoi(opr1);
	else
		right = std::stoi(vtable[opr1]);

	//resize to 32bit int for storing in register
	left = static_cast< int >(left);
	right = static_cast< int >(right);
	
	//whatever man
	if (operator_ == "+") //add
		eval = static_cast< int >(left + right);
	else if (operator_ == "-") //sub
		eval = static_cast< int >(left - right);
	else if (operator_ == "*") //mult
		eval = static_cast< int >(left * right);
	else if (operator_ == "/") //div
		eval = static_cast< int >(left / right);
	else if (operator_ == "%") //mod
		eval = static_cast< int >(left % right);
	else
		throw Error("[binop_result]: ERROR: line " + std::to_string(read_head) + ": unknown operator " + operator_);
	return std::to_string(eval);
}

void IR_interpreter::cond_result(const std::string opr0,
								 const std::string relop,
								 const std::string opr1,
								 const std::string then_label,
								 const std::string else_label)
{
	int left, right;
	bool taken;
	if (opr0.find('t') == std::string::npos)
	    left = std::stoi(opr0);
	else
		left = std::stoi(vtable[opr0]);
	if (opr1.find('t') == std::string::npos)
	    right = std::stoi(opr1);
	else
		right = std::stoi(vtable[opr1]);

	if (relop == "==")
		taken = left == right;
	else if (relop == "!=")
		taken = left != right;
	else if (relop == "<")
		taken = left < right;
	else if (relop == "<=")
		taken = left <= right;
	else if (relop == ">")
		taken = left > right;
	else if (relop == ">=")
		taken = left >= right;
	else
		throw Error("[cond_result]: ERROR: line "
					+ std::to_string(read_head)
					+ ": unknown relop " + relop);

	jump(get_label_pos(taken ? then_label : else_label));
}

std::string IR_interpreter::print_call(const IR_call & c)
{
	if (c.args.size() != 1)
		throw Error("[print_call]: ERROR: line " + std::to_string(read_head)
					+ ": print takes one argument");
	const std::string a = c.args[0];
	//literal
	if (a[0] != 't')
		return a;
	auto reg = vtable.find(a);
	if (reg == vtable.end())
		throw Error("[print_call]: ERROR: line " + std::to_string(read_head)
					+ ": undefined register \"" + a + "\"");
	return reg->second;
}

void IR_interpreter::read_call(const IR_call & c)
{
	if (!c.args.empty())
		throw Error("[read_call]: ERROR: line " + std::to_string(read_head)
					+ ": read takes no arguments");
	int n;
	//EOF or non-int input
	if (!(std::cin >> n))
		throw Error("[read_call]: ERROR: line " + std::to_string(read_head)
					+ ": expected an int on input");
	add_reg(c.dest, std::to_string(n));
}

void IR_interpreter::to_file(const std::string s)
{
	std::string output_file = stem(s) + ".out";
	std::ofstream f(output_file);
	if (!f.is_open())
		throw Error("[IR_interpreter::to_file]: ERROR: FAILED TO OPEN FILE \"" + output_file + "\"");
	for (const auto & k : output)
		f << k << '\n';
}

//t1 := CALL f(t2, t3)
//saves the caller line, dest, regs then starts f with
//new regs holding its params
//evaluate() ++read_head moves past the FUNCTION line into f's body
void IR_interpreter::func_call(const IR_call & c)
{
	auto f = funcs.find(c.func);
	if (f == funcs.end())
		throw Error("[func_call]: ERROR: line " + std::to_string(read_head)
					+ ": undefined function \"" + c.func + "\"");
	//args read intp the caller regs before switching
	std::vector< std::string > vals;
	for (const std::string & a : c.args)
		vals.push_back(get_val(a));

	IR_func header = func_eval(file_content[f->second]);
	if (header.params.size() != vals.size())
		throw Error("[func_call]: ERROR: line " + std::to_string(read_head)
					+ ": \"" + c.func + "\" expects "
					+ std::to_string(header.params.size()) + " arguments, got "
					+ std::to_string(vals.size()));

	call_stack.push_back({read_head, c.dest, std::move(vtable)});
	vtable.clear();
	for (size_t i = 0; i < vals.size(); ++i)
		add_reg(header.params[i], vals[i]);
	jump(f->second);
}

//RETURN t1
//call: restore the caller regs, put t1s value in the call struct dest and resume after
//main: jump to mains END
void IR_interpreter::return_call(const IR_return & r)
{
	const std::string val = get_val(r.val);
	if (call_stack.empty())
	{
		size_t e = read_head;
		while (e < file_content.size() && file_content[e] != "END")
			++e;
		if (e == file_content.size())
			throw Error("[return_call]: ERROR: line " + std::to_string(read_head)
						+ ": no END after RETURN");
		//evaluate() increments read_head onto END
		jump(e - 1);
		return;
	}
	IR_frame f = std::move(call_stack.back());
	call_stack.pop_back();
	vtable = std::move(f.regs);
	add_reg(f.dest, val);
	jump(f.ret);
}

std::string IR_interpreter::get_val(const std::string a)
{
	if (a[0] != 't') //literal
		return a;
	auto reg = vtable.find(a);
	if (reg == vtable.end())
		throw Error("[get_val]: ERROR: line " + std::to_string(read_head)
					+ ": undefined register \"" + a + "\"");
	return reg->second;
}
