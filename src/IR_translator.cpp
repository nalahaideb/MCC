#include "IR_translator.h"

//==================================================================
//                 PUBLIC
//==================================================================

//Program(l = Seq-list of Func)
void IR_translator::translate(const Node * root)
{
	code_.clear();
	std::vector< const Node * > funs;
	node_list(root->l, funs);
	for (const Node * f : funs)
		trans_function(f);
}

void IR_translator::to_file(const std::string & path) const
{
	std::ofstream out(path);
	for (const auto & line : code_)
		out << line << '\n';
}

const std::vector< std::string > & IR_translator::code() const
{
	return code_;
}

//==================================================================
//                 HELPERS
//==================================================================

//increment temp counter and return new temp var
std::string IR_translator::new_temp()
{
	return "t" + std::to_string(++temp_count);
}

//increment lable counter and return new lablel
std::string IR_translator::new_label()
{
	return "l" + std::to_string(++label_count);
}

//add produced IR code to return vector
void IR_translator::add_IR_code(const std::string & line)
{
	code_.push_back(line);
}

void IR_translator::enter()
{
	vtable_.push_front(var_scope());
}

void IR_translator::exit()
{
	if (!vtable_.empty())
		vtable_.pop_front();
}

//binds new temp values 
void IR_translator::bind(const std::string & var, const std::string & temp)
{
	vtable_.front()[var] = temp;
}

//most recently added binding is priority
const std::string & IR_translator::lookup(const Node * ident)
{
	for (const auto & scope : vtable_)
	{
		const auto f = scope.find(ident->s);
		if (f != scope.end())
			return f->second;
	}
	throw Error("[IR_translator]: ERROR: line " + ident->line
				+ ": no temp bound for \"" + ident->s + "\"");
}

//in-order walk for Seq nodes that hold args or subexprs
//should probably template this since its also used in symtable,
//no need to have both
void IR_translator::node_list(const Node * n, std::vector< const Node * > & out)
{
	if (!n) return;
	if (n->kind == Kind::Seq)
	{
		node_list(n->l, out);
		node_list(n->r, out);
	}
	else
		out.push_back(n);
}

//operators whose value is a bool, translated through trans_cond
bool IR_translator::is_cond(Kind k) const
{
	switch (k)
	{
	case Kind::Eq:
	case Kind::Ne:
	case Kind::Lt:
	case Kind::Le:
	case Kind::Gt:
	case Kind::Ge:
	case Kind::And:
	case Kind::Or:
	case Kind::Not:
		return true;
	default:
		return false;
	}
}

//==================================================================
//                 TRANSLATIONS
//==================================================================

//Func(l = Seq(Seq(Type, Ident), Params(l = Seq-list of Param)), r = Block)
void IR_translator::trans_function(const Node * f)
{
	temp_count = 0;
	label_count = 0;
	enter();

	const Node * name = f->l->l->r;
	//collects in-order list of func params
	std::vector< const Node * > params;
	node_list(f->l->r->l, params);

	std::string header = "FUNCTION " + name->s + "(";
	for (size_t i = 0; i < params.size(); ++i)
	{
		const std::string t = new_temp();
		bind(params[i]->r->s, t);
		header += (i ? ", " : "") + t;
	}
	add_IR_code(header + ")");

	trans_stmt(f->r);

	exit();
	add_IR_code("END");
}

void IR_translator::trans_stmt(const Node * n)
{
	if (!n) return;
	switch (n->kind)
	{
	case Kind::Seq:
		trans_stmt(n->l);
		trans_stmt(n->r);
		break;
			
	case Kind::Block:
		enter();
		trans_stmt(n->l);
		exit();
		break;

		//Decl(l = Seq(Type, Ident), r = Exp), bound after the initializer
	case Kind::Decl:
		{
			const std::string t = new_temp();
			trans_exp(n->r, t);
			bind(n->l->r->s, t);
			break;
		}

		//assigns a new temp so self-referencing exprs dont get overwritten
		//a = a + 1, b = !b etc
	case Kind::Assign:
		{
			const std::string t = new_temp();
			trans_exp(n->r, t);
			add_IR_code(lookup(n->l) + " := " + t);
			break;
		}

		//call statements, result goes to a throwaway temp
	case Kind::Print:
		{
			const std::string c = trans_call("print", n->l);
			add_IR_code(new_temp() + " := " + c);
			break;
		}
		
		//function calls + args
	case Kind::Call:
		{
			const std::string c = trans_call(n->l->s, n->r);
			add_IR_code(new_temp() + " := " + c);
			break;
		}

		//function returns + arg
	case Kind::Return:
		{
			const std::string t = new_temp();
			trans_exp(n->l, t);
			add_IR_code("RETURN " + t);
			break;
		}

		//If(l = cond, r = Stmt | Else(then, other))
	case Kind::If:
		{
			const std::string l1 = new_label(), l2 = new_label();
			if (n->r->kind == Kind::Else)
			{
				const std::string l3 = new_label();
				trans_cond(n->l, l1, l2);
				add_IR_code("LABEL " + l1);
				trans_stmt(n->r->l);
				add_IR_code("GOTO " + l3);
				add_IR_code("LABEL " + l2);
				trans_stmt(n->r->r);
				add_IR_code("LABEL " + l3);
			}
			else
			{
				trans_cond(n->l, l1, l2);
				add_IR_code("LABEL " + l1);
				trans_stmt(n->r);
				add_IR_code("LABEL " + l2);
			}
			break;
		}

	case Kind::While:
		{
			const std::string l1 = new_label(), l2 = new_label(), l3 = new_label();
			add_IR_code("LABEL " + l1);
			trans_cond(n->l, l2, l3);
			add_IR_code("LABEL " + l2);
			trans_stmt(n->r);
			add_IR_code("GOTO " + l1);
			add_IR_code("LABEL " + l3);
			break;
		}

	default:
		throw Error("[IR_translator]: ERROR: line " + n->line
					+ ": unexpected statement \"" + kind_name(n->kind) + "\"");
	}
}

//computes e into place
void IR_translator::trans_exp(const Node * e, const std::string & place)
{
	if (is_cond(e->kind))
	{
		const std::string l1 = new_label(), l2 = new_label();
		add_IR_code(place + " := 0");
		trans_cond(e, l1, l2);
		add_IR_code("LABEL " + l1);
		add_IR_code(place + " := 1");
		add_IR_code("LABEL " + l2);
		return;
	}

	switch (e->kind)
	{
		//atoms
	case Kind::Num:
		add_IR_code(place + " := " + e->s);
		break;
	case Kind::True:
		add_IR_code(place + " := 1");
		break;
	case Kind::False:
		add_IR_code(place + " := 0");
		break;
	case Kind::Ident:
		add_IR_code(place + " := " + lookup(e));
		break;
	case Kind::Read:
		add_IR_code(place + " := CALL read()");
		break;
	case Kind::Call:
		{
			const std::string c = trans_call(e->l->s, e->r);
			add_IR_code(place + " := " + c);
			break;
		}

		//binop ints
	case Kind::Add:
	case Kind::Sub:
	case Kind::Mul:
	case Kind::Div:
	case Kind::Mod:
		{
			const std::string t1 = new_temp();
			trans_exp(e->l, t1);
			const std::string t2 = new_temp();
			trans_exp(e->r, t2);
			add_IR_code(place + " := " + t1 + " " + kind_name(e->kind) + " " + t2);
			break;
		}

	case Kind::Neg:
		{
			const std::string t = new_temp();
			trans_exp(e->l, t);
			add_IR_code(place + " := - " + t);
			break;
		}

	default:
		throw Error("[IR_translator]: ERROR: line " + e->line
					+ ": unexpected expression \"" + kind_name(e->kind) + "\"");
	}
}

//jumps to lt if e is true, lf otherwise
void IR_translator::trans_cond(const Node * e, const std::string & lt, const std::string & lf)
{
	switch (e->kind)
	{
	case Kind::Eq: case Kind::Ne: case Kind::Lt:
	case Kind::Le: case Kind::Gt: case Kind::Ge:
		{
			const std::string t1 = new_temp();
			trans_exp(e->l, t1);
			const std::string t2 = new_temp();
			trans_exp(e->r, t2);
			add_IR_code("IF " + t1 + " " + kind_name(e->kind) + " " + t2
						+ " THEN " + lt + " ELSE " + lf);
			break;
		}

	case Kind::True:
		add_IR_code("GOTO " + lt);
		break;
	case Kind::False:
		add_IR_code("GOTO " + lf);
		break;

	case Kind::Not:
		trans_cond(e->l, lf, lt);
		break;

	case Kind::And:
		{
			const std::string l = new_label();
			trans_cond(e->l, l, lf);
			add_IR_code("LABEL " + l);
			trans_cond(e->r, lt, lf);
			break;
		}
	case Kind::Or:
		{
			const std::string l = new_label();
			trans_cond(e->l, lt, l);
			add_IR_code("LABEL " + l);
			trans_cond(e->r, lt, lf);
			break;
		}

		//bool variable, call, etc
	default:
		{
			const std::string t = new_temp();
			trans_exp(e, t);
			add_IR_code("IF " + t + " != 0 THEN " + lt + " ELSE " + lf);
			break;
		}
	}
}

//generate code for each arg left to right
//returns "CALL f(t1, t2)"
std::string IR_translator::trans_call(const std::string & f, const Node * args)
{
	std::vector< const Node * > list;
	node_list(args, list);

	std::string c = "CALL " + f + "(";
	for (size_t i = 0; i < list.size(); ++i)
	{
		const std::string t = new_temp();
		trans_exp(list[i], t);
		c += (i ? ", " : "") + t;
	}
	return c + ")";
}
