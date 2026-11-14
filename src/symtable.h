#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <iostream>
#include <list>
#include <unordered_map>
#include <fstream>
#include "node.h"
#include "signature.h"
#include "error.h"

//==============================================================================
/*
  OVERVIEW:
  Symtable is a doubly-linked list of hashtables that store the local
  blocks scope and bindings

  - enter() and exit() open and close scopes

  - algorithm recursively looks for scope markers (fun, block) and for
    parameters within them (leaf nodes bound by param, assign, decl)

  - once a binding node is found (param, assign, decl), select_binding
    is called to pick a type to bind and enter it into the current
	scope
  - 
*/
//==============================================================================

//==============================================================================
/*
  TODO:
  [X] how to determine when a scope is "closed" in the AST? or even CST?
     (process its children until leaf nodes are hit, different cases
	 for different constructs, check notes)
  [X] define how to bind variables, which nodes require special rules
     in "check()" (assign, decl, fun, block, etc)
  [] bind_assign has to recursively check the rightmost subtree for
     all idents and ensure they are called within their scopes
	 but how does (or should) it evaluate it when it has an expression?
  [] table for functions

  OPTIMIZATIONS:

  [] definitely need to clear out some redundant NULL checks in the
     beginning of functions
  [] enum or hashing of string types would not only make this parts code more readable
     but the entire programs
  [] cleanup of functions, huge functions should be partitioned out, namely expr_type
  [] multiple functions check l/r children for binding, can be templated or generalized
*/
//==============================================================================

typedef std::unordered_map< name, type > scope_table;
typedef scope_table::iterator binding;

class Symtable
{
public:
	Symtable() {}
	
	inline void check_program(const Node * root);
	
private:
	//member funcs
	inline bool empty();
	inline bool is_leaf(const Node * n);
	inline bool is_binding(const name & bound);
	inline bool func_returns(const Node * f);
	inline bool ends_in_return(const Node * n);

	inline void enter();
	inline void exit();
	inline void bind(const name n, const type t, const std::string bind);
	inline void select_binding(const Node * n);
	inline void bind_decl(const Node * n);
	inline void bind_assign(const Node * n);
	inline void bind_param(const Node * n);
	inline void collect_args(const Node * n, std::vector< const Node * > & out);
	inline void collect_signatures(const Node * root);
	inline void check(const Node * root);

	inline scope_table * lookup(const name n);
	inline type expr_type(const Node * n);
	inline void subexpr_type(const Node * n, const type & expected);
	inline type call_type(const Node * n);

	//member variables
	std::list< scope_table > sym_table;
	std::unordered_map< name, signature > func_table;
	//for checking fun returns
	type current_ret;

	//wishlist, not implemented yet
	/*
	  inline Node * find();
	*/
};

//debug for scope_table
inline std::ostream & operator<<(std::ostream & os, const scope_table & s)
{
	if (s.empty())
		os << "TABLE EMPTY\n";
	else
	{
		os << "[scope size] = " << s.size() << std::endl;
		os << "--------------------------------------------------\n";
		for (const auto & [k, v] : s)
			os << "< name, type = (" << k << ", " << v << ") >\n";
		os << "--------------------------------------------------\n";
	}
	return os;
}

//debug for scope_table list
inline std::ostream & operator<<(std::ostream & os, const std::list< scope_table > & s)
{
	os << ">>>begin table printout<<<\n\n";
	for (const auto & t : s)
	{
		os << t << "\n\n";
	}
	os << "***end table printout***\n";
	return os;
}

#endif
