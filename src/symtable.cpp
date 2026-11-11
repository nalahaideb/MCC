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

binding Symtable::lookup(const name n)
{
    binding b;
	for (auto & scope : sym_table)
	{
	    binding f = scope.find(n);
		if (f != scope.end())
		{
			b = f;
			break;
		}
 	}
	return b;
}

//binds a type / var_data pair to a name
//should only be called when non-empty and reference start of list
void Symtable::bind(const name n, const type t, const int value)
{
	std::pair< type, std::string > entry;
	entry.first = t;
	entry.second = value;
	if (!empty())
		sym_table.front().insert({n, entry});
}

bool Symtable::is_leaf(const Node * n)
{
	if (n)
		return !(n->l || n->r);
	else
		return false;
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
	// std::ofstream out("ast.dot");
	// out << dot(root);
	// out.close();
	// Raw r = raw(root);
	//std::cout << r << std::endl;
	if (sym_table.size() > max) max = sym_table.size();
	
	if (!root)
		return;
	else
	{
		type root_type = kind_name(root->kind);
		std::cout << "root type = " << root_type << std::endl;
		//open new scope on block
	    if (root_type == "block")
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
			enter();
			if (!root->l)
			{
				Error e("[check]: ERROR: fun decl has no left child");
				throw e;
			}
			else
				check(root->l);
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
	std::cout << "max size was: " << max << std::endl;
}

