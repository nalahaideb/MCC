#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <iostream>
#include <list>
#include <unordered_map>
#include <fstream>
#include "node.h"
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
*/
//==============================================================================

typedef std::string name;
typedef std::string type;
typedef std::unordered_map< name, type > scope_table;
typedef scope_table::iterator binding;

class Symtable
{
 public:
	Symtable() {}

	inline bool empty();

	inline void check(const Node * root);
	inline void run();
	
 private:

	inline bool is_leaf(const Node * n);
	inline bool is_binding(const name & bound);
	inline bool is_valid_assignment(const binding b);
	inline bool func_returns(const Node * f);
	
	inline void enter();
	inline void exit();
	inline void bind(const name n, const type t);
	inline void select_binding(const Node * n);
	inline void bind_decl(const Node * n);
	inline void bind_assign(const Node * n);

	inline scope_table * lookup(const name n);
	inline type expr_type(const Node * n, const type expected);
	
	std::list< scope_table > sym_table;
	//wishlist, not implemented yet
	/*
	  inline void bind_param(const Node * n);
	  inline Node * find();
	*/
};

//debug for scope_table
inline std::ostream & operator<<(std::ostream & os, const scope_table & s)
{
	os << "--------------------------------------------------\n";
	for (const auto & [k, v] : s)
		os << "< name, type = (" << k << ", " << v << ") >\n";
	os << "--------------------------------------------------\n";
	return os;
}

//debug for scope_table list
inline std::ostream & operator<<(std::ostream & os, const std::list< scope_table > & s)
{
	os << "begin table printout:\n\n";
	for (const auto & t : s)
	{
		os << t << "\n\n";
	}
	os << "end table printout\n";
	return os;
}

#endif
