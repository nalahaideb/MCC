#include "symtable.h"

bool Symtable::empty()
{
	return sym_table.empty();
}

void Symtable::enter()
{
	scope_table s;
	sym_table.push_front(s);
}

void Symtable::exit()
{
	//std::cout << "destroyed last scope\n";
	if (!empty())
		sym_table.pop_front();
}

scope_table * Symtable::lookup(const name n)
{
    scope_table * b = NULL;
	for (auto & scope : sym_table)
	{
	    const auto f = scope.find(n);
		if (f != scope.end())
		{
			b = &scope;
			break;
		}
 	}
	return b;
}

//binds a name, type pair
//should only be called when non-empty and reference start of list
void Symtable::bind(const name n, const type t, const std::string line)
{
	if (!empty())
	{
		if (sym_table.front().find(n) != sym_table.front().end())
		{
			Error e("[expr_type]: ERROR: line " + line
					+ ": variable \"" + n + "already declared");
			throw e;
		}
		else
			sym_table.front().insert({n, t});
	}
}

bool Symtable::is_leaf(const Node * n)
{
	if (n)
		return !(n->l || n->r);
	else
		return false;
}

//check if valid nonterminal for binding value pair
bool Symtable::is_binding(const name & bound)
{
	return (bound == "decl" || bound == "assign" || bound == "param");
}

//doesnt really need to check left since it shouldnt ever be there
//change it later once its verified to work for all cases
bool Symtable::func_returns(const Node * n)
{
	if (!n)
		return false;
	switch (n->kind)
	{
	case Kind::Return:
		return true;
	case Kind::Seq:
		return func_returns(n->l) || func_returns(n->r);
		//list returns if any stmt does
	case Kind::Block:
		return func_returns(n->l);
		//no else can use this case too
	case Kind::If:
		return n->r && n->r->kind == Kind::Else            
			&& func_returns(n->r->l) && func_returns(n->r->r);
		//decl, assign, while, call
	default:
		return false;
	}
}

//fun nodes rightmost child always either return or a seq that leads to blocks that
//should return, otherwise return false
bool Symtable::ends_in_return(const Node * n)
{
	if (!n)
		return false;
	switch (n->kind)
	{
	case Kind::Return:
		return true;
		//left-nested r should be last item
	case Kind::Seq:
		return ends_in_return(n->r);   
		//nested block must end in a return
	case Kind::Block:
		return ends_in_return(n->l);   
		//only true when there is an else and both branches return
	case Kind::If:
		return func_returns(n);
	default:
		return false;                  
	}
}

//throws unless n has type expected
void Symtable::subexpr_type(const Node * n, const type & expected)
{
	const type got = expr_type(n);
	if (got != expected)
	{
		Error e("[subexpr_type]: ERROR: line " + n->line
				+ ": expected type \"" + expected
				+ "\" but found \"" + got + "\"");
		throw e;
	}
}

//check the type of expression being evaluated
type Symtable::expr_type(const Node * n)
{
	const type k = kind_name(n->kind);

	//leaves: these end the recursion
	if (k == "true" || k == "false")
		return "bool";
	if (k == "num"  || k == "read")
		return "int";
	if (k == "call")
		return call_type(n);
	if (k == "ident")
	{
		scope_table * s = lookup(n->s);
		if (!s)
		{
			Error e("[expr_type]: ERROR: line " + n->line + ": no such variable \"" + n->s + "\"");
			throw e;
		}
		return (*s)[n->s];
	}

	//both types should match 
	if (k == "==" || k == "!=")
	{
		subexpr_type(n->r, expr_type(n->l));
		return "bool";
	}
 
	//integer operators
	//children must be of equal type for expr to be valid
	type expected = "int", ret = "int";
	if (k == "<" || k == "<=" || k == ">" || k == ">=")
		ret = "bool";
	else if (k == "&&" || k == "||" || k == "!")
		expected = ret = "bool";

	subexpr_type(n->l, expected);
	
	//! and neg have no right child
	if (n->r)
		subexpr_type(n->r, expected);  
	return ret;
}

//is of the form b->l == type, b->r == name, except decl which has
//a seq node as its left child to accommodate assignment on the right
//should handle that someway concisely
void Symtable::select_binding(const Node * n)
{
	type n_type = kind_name(n->kind);
	if (n_type == "decl")
		bind_decl(n);
	else if (n_type == "assign")
		bind_assign(n);
	else if (n_type == "param")
		bind_param(n);
}

//bind a declaration node
inline void Symtable::bind_decl(const Node * n)
{
	if (!n->l || !n->r)
	{
		Error e("[bind_decl]: ERROR: line " + n->line
				+ ": one or both children NULL");
		throw e;
	}
	else
	{
		//n->left->left holds type, ident
		//n->right holds value
		Node * ll = n->l;
		Node * rr = n->r;
		name n_ = ll->r->s;
		type t = ll->l->s;
		type expr = expr_type(rr);
		//should check this before entering it in as a binding, or maybe just check the
		//most recently added binding?
		if (t != expr)
		{
			Error e("[bind_decl]: ERROR: line " + n->line + ": expected type \""
					+ t + "\", but expression is of type \"" + expr + "\"");
			throw e;
		}
		
		bind(n_, t, n->line);
	}
}

//must find a lookup before updating table
//otherwise throw an error
void Symtable::bind_assign(const Node * n)
{
	if (is_leaf(n))
	{
		if (kind_name(n->kind) == "ident")
		{
			if (lookup(n->s) == NULL)
			{
				Error e("[bind_assign]: ERROR: line " + n->line + ": no such variable \""
						+ n->s + "\"");
				throw e;			
			}
		}
		else
			return;
	}
	else
	{
		name nl_name = n->l->s;
		type nr_type = kind_name(n->r->kind);
		scope_table * k = lookup(nl_name);
		if (k == NULL)
		{
			Error e("[bind_assign]: ERROR: line " + n->line + ": no such variable \""
					+ nl_name + "\"");
			throw e;
		}
		else
		{
			type nl_type = (*k)[nl_name];			
			//lookup of lhs, compare with type return of rhs thru
			//n->l->s == expr_type(n->r) (should be both "bool" or "int")
			type expr = expr_type(n->r);
			if (nl_type != expr)
			{
				Error e("[bind_assign]: ERROR: line " + n->line + ": expected type \""
						+ nl_type + "\", but expression is of type \"" + expr + "\"");
				throw e;
			}
		}
	}
}

//should be param node, which leftmost child of params (seq) holds
//each leftmost child of a seq node holds either a param or another seq
void Symtable::bind_param(const Node * n)
{
	if (!n)
		return;
	else
	{
		type n_type = kind_name(n->kind);
		//bind children if node is param 
		if (n_type == "param")
		{
			bind(n->r->s, n->l->s, n->line);
		}
		else// if (n_type == "seq") //recursively call on seq's leftmost child
		{                         //then call it on seq's right child (should be param)
			bind_param(n->l);
			bind_param(n->r);
		}
	}
}

//evaluates function called with expression / arguments (and their number) for correct type
type Symtable::call_type(const Node * n)
{
	const name rhs = n->l->s;
	const auto f = func_table.find(rhs);
	if (f == func_table.end())
	{
		Error e("[call_type]: ERROR: line " + n->line
				+ ": no such function \"" + rhs + "\"");
		throw e;
	}
	const signature & sig = f->second;

	std::vector< const Node * > args;
	collect_args(n->r, args);
	
	if (args.size() != sig.params.size())
	{
		Error e("[call_type]: ERROR: line " + n->line + ": function \"" + rhs + "\" takes "
				+ std::to_string(sig.params.size()) + " argument(s), but "
				+ std::to_string(args.size()) + " given");
		throw e;
	}

	for (size_t i = 0; i < args.size(); ++i)
	{
		const type expected = sig.params[i];
		const type call_expr = expr_type(args[i]);
		if (call_expr != expected)
		{
			Error e("[call_type]: ERROR: line " + n->line + ": argument "
					+ std::to_string(i + 1) + " of \"" + rhs
					+ "\" expected type \"" + expected
					+ "\", but expression is of type \"" + call_expr + "\"");
			throw e;
		}
	}
	return sig.ret;
}

//collects arguments found in calls of functions or function signatures / bodies
void Symtable::collect_args(const Node * n, std::vector< const Node * > & out)
{
	if (!n) return;
	if (n->kind == Kind::Seq)
	{
		collect_args(n->l, out);
		collect_args(n->r, out);
	}
	else
		out.push_back(n);
}

//first pass of program to identify func signatures for calling later
void Symtable::collect_signatures(const Node * root)
{
	std::vector< const Node * > funs;
	collect_args(root->l, funs);

	for (const Node * f : funs)
	{
		//seq(type, ident)
		const Node * head = f->l->l;
		//params -> list of param (may be NULL)
		const Node * plist = f->l->r->l;

		signature sig;
		sig.ret = head->l->s;
		sig.line = f->line;

		std::vector< const Node * > ps;
		collect_args(plist, ps);
		for (const Node * p : ps)
			//param l = type
			sig.params.push_back(p->l->s);

		const name fname = head->r->s;
		if (!func_table.insert({ fname, sig }).second)
		{
			Error e("[collect_signatures]: ERROR: line " + f->line
					+ ": function \"" + fname + "\" is already defined");
			throw e;
		}
	}
}

//traverses the tree to open / close scopes and identify types / errors
/*
  [x] block -> open scope and process children down to terminals,
     close after reaching processing all leaf nodes
  [x] params -> open scope, include arguments in scope, close after
      processing all leaf nodes
*/
void Symtable::check(const Node * root)
{
	//base
	if (!root)
		return;
	//recursive
	else 
	{
		type root_type = kind_name(root->kind);
		if (is_binding(root_type))
		{
			select_binding(root);
		}
		//open new scope on block
	    else if (root_type == "block")
		{
			//create new scope
			enter();
			if (root->l)
				check(root->l);
			if (root->r)
				check(root->r);
			//destroy latest scope
			exit();
		}
		//process function declarations, parameters of function
		//are in leftmost child, process rightmost child before
		//calling exit() to keep parameters in the same scope
		else if (root_type == "fun")
		{

			//look up by func name
			current_ret = func_table.at(root->l->l->r->s).ret;
			enter();
			//checks params
			check(root->l);
			//checks body, have to skip the inherent "block" item in the func
			//so it goes to r->l
			check(root->r->l);
			if (!func_returns(root->r))
			{
				Error e("[check]: ERROR: line " + root->line
						+ ": function \"" + root->l->l->r->s
						+ "\" has no return statement or not all paths return");
				throw e;
			}
			if (!ends_in_return(root->r)) //rightmost child should be return/seq
			{
				Error e("[check]: ERROR: line " + root->line
						+ ": function \"" + root->l->l->r->s
						+ "\" must end with a return statement");
				throw e;
			}
			
			exit();
		}
		else if (root_type == "return")
		{
			if (!root->l)
			{
				Error e("[check]: ERROR: line " + root->line
						+ ": expected a value of type \""
						+ current_ret + "\" but found a bare return");
				throw e;
			}
			const type ret_type = expr_type(root->l);
			if (ret_type != current_ret)
			{
				Error e("[check]: ERROR: line " + root->line
						+ ": expected return type \"" + current_ret
						+ "\", but expression is of type \"" + ret_type + "\"");
				throw e;
			}
		}
		else if (root_type == "print")
		{
			//throw on undeclared variable or a type mismatch
			type should_be_int = expr_type(root->l);
			if (should_be_int != "int")
			{
				Error e("[check]: ERROR: line " + root->line
						+ ": cannot print booleans\n");
				throw e;				
			}
		}
		else if (root_type == "if" || root_type == "while")
		{
			const type expected = expr_type(root->l);
			if (expected != "bool")
			{
				Error e("[check]: ERROR: line " + root->line
						+ ": condition must be \"bool\", but expression is of type \""
						+ expected + "\"");
				throw e;
			}
			//
			check(root->r);                          
		}
		else
		{
			check(root->l);
			check(root->r);
		}
	}
}

void Symtable::check_program(const Node * root)
{
	collect_signatures(root);
	check(root);

	//check main func correctness
	const auto main = func_table.find("main");
	if (main == func_table.end())
	{
		Error e("[check_program]: ERROR: line 1: no main function");
		throw e;
	}
	else if (main != func_table.end() && !((*main).second.params.empty()))
	{
		Error e("[check_program]: ERROR: line 1: main has parameters");
		throw e;
	}
	else if ((*main).second.ret != "int")
	{
		Error e("[check_program]: ERROR: line 1: main type not int");
		throw e;
	}
}

