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

// bool Symtable::is_valid_assignment(const binding b)
// {
// 	return false;
// }

//binds a type / var_data pair to a name
//should only be called when non-empty and reference start of list
//type bool can only have 1 or 0, int can be anything <= MAX_INT
void Symtable::bind(const name n, const type t)
{
	if (!empty())
		sym_table.front().insert({n, t});
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
	return (bound == "decl"
			|| bound == "assign"
			|| bound == "param");
}

//doesnt really need to check left since it shouldnt ever be there
//change it later once its verified to work for all cases
bool Symtable::func_returns(const Node * f)
{
	//base
	type node = kind_name(f->kind);
	if (node == "return") return true;
	//recursive
	bool left, right;
	if (f->l)
	    left = func_returns(f->l);
	if (f->r)
	    right = func_returns(f->r);
	return (left || right);
}

type Symtable::expr_type(const Node * n, const type expected)
{
	//dont check null
	type ret = "int";
	if (!n)
	{
		Error e("[expr_type]: ERROR: null value for 'n'");
		throw e;			
	}
	else //if any boolean operator or value is detected, it is a boolean expression
	{
		type n_type = kind_name(n->kind);
		if (n_type == "true" || n_type == "false"
			|| n_type == "||" || n_type == "&&"
			|| n_type == "==" || n_type == "!="
			|| n_type == "<" || n_type == "<="
			|| n_type == ">" || n_type == ">="
			)
		    ret = "bool";
		else if (n_type == "ident")
		{
			//check if variable exists on rhs of expr
			scope_table * k = lookup(n->s);
			if (!k)
			{
				Error e("[expr_type]: ERROR: no such variable \""
						+ n->s + "\"");
				throw e;			
			}
			else if (expected != (*k)[n->s])
			{
				Error e("[expr_type]: ERROR: variable \""
						+ n->s + "\" has type \"" + (*k)[n->s] + "\", expected type \""
						+ expected);
				throw e;							
			}			
		}
		else //recursively check sub expressions when no terminal is found
		{
			if (n->l)
				expr_type(n->l, expected);
			if (n->r)
				expr_type(n->r, expected);
		}
	}
	return ret;
}

//is of the form b->l == type, b->r == name, except decl which has
//a seq node as its left child to accommodate assignment on the right
//should handle that someway concisely
void Symtable::select_binding(const Node * n)
{
	type n_type = kind_name(n->kind);
	//std::cout << "select binding n_type = " << n_type << std::endl;
	if (n_type == "decl")
		bind_decl(n);
	else if (n_type == "assign")
		bind_assign(n);
	else if (n_type == "param")
	{
		
	}
}

//bind a declaration node
inline void Symtable::bind_decl(const Node * n)
{
	if (!n->l || !n->r)
	{
		Error e("[bind_decl]: ERROR: one or both children NULL");
		throw e;
	}
	else
	{
		//n->left->left holds type, ident
		//n->right holds value
		Node * ll = n->l;
		Node * rr = n->r;
		name n = ll->r->s;
		type t = ll->l->s;
		type expr = expr_type(rr, t);
		// std::cout << t << ", " << expr << std::endl;
		//should check this before entering it in as a binding, or maybe just check the
		//most recently added binding?
		if (t != expr)
		{
			Error e("[bind_decl]: ERROR: expected type \""
					+ t + "\", but expression is of type \"" + expr + "\"");
			throw e;
		}
		
		bind(n, t);
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
				Error e("[bind_assign]: ERROR: no such variable \""
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
			Error e("[bind_assign]: ERROR: no such variable \""
					+ nl_name + "\"");
			throw e;
		}
		else
		{
			// std::cout << "name = " << nl_name << std::endl;
			// std::cout << "rhs kind = " << n->r->kind << std::endl;
			// std::cout << "rhs value = " << n->r->s << std::endl;
			
			type nl_type = (*k)[nl_name];
			//lookup of lhs, compare with type return of rhs thru
			//n->l->s == expr_type(n->r) (should be both "bool" or "int")
			type expr = expr_type(n->r, nl_type);
			// std::cout << "lh name, lh type, rh name, rh type = ("
			// 		  << nl_name << ", " << nl_type <<"), (" << n->r->s
			// 		  << ", " << nr_type << ")\n";
			if (nl_type != expr)
			{
				Error e("[bind_assign]: ERROR: expected type \""
						+ nl_type + "\", but expression is of type \"" + expr + "\"");
				throw e;
			}
		}
	}
}

//traverses the tree to open / close scopes and identify types / errors
/*
  [] block -> open scope and process children down to terminals,
     close after reaching processing all leaf nodes
  [] params -> open scope, include arguments in scope, close after
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
		//std::cout << sym_table << std::endl;
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
			{
				if (is_leaf(root->l))
					return;
				else
					check(root->l);
			}
			if (root->r)
			{
				if (is_leaf(root->r))
					return;
				else
					check(root->r);
			}
			//destroy latest scope
			exit();
		}
		//process function declarations, parameters of function
		//are in leftmost child, process rightmost child before
		//calling exit() to keep parameters in the same scope
		else if (root_type == "fun")
		{
			//std::cout << "fun type \n";
			enter();
			if (!root->l || !root->r)
			{
				Error e("[check]: ERROR: fun decl missing a child");
				throw e;
			}
			else
			{
				check(root->l);
				check(root->r);
			}
			exit();
		}
		else
		{
			check(root->l);
			check(root->r);
		}
	}
}

void Symtable::run()
{
	//std::cout << "max size was: " << max << std::endl;
}

