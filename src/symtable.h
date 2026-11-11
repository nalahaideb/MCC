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
  block's scope and bindings
  - enter() and exit() open and close scopes
  - algorithm recursively looks for scope markers (fun, block) and for
    parameters within them (leaf nodes bound by param, assign, decl)
  - 
*/
//==============================================================================

//==============================================================================
/*
  TODO:
  [] how to determine when a scope is "closed" in the AST? or even CST?
     (process its children until leaf nodes are hit, different cases
	 for different constructs, check notes)
  [] define how to bind variables, which nodes require special rules
     in "check()" (assign, decl, fun, block, etc)
  [] anywhere a variable is used or can be declared should be within
     a scope that can be checked
  []
*/
//==============================================================================

typedef std::string name;
typedef std::string type;
typedef std::unordered_map< name, std::pair< type, std::string > > scope_table;
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
	inline binding lookup(const name n);
	inline void bind(const name n, const type t, const int value);
	inline bool is_valid_assignment(const binding b);
	inline void enter();
	inline void exit();
	unsigned int max = 0;
	std::list< scope_table > sym_table;
	//wishlist, not implemented yet
	/*
	  bool func_returns(Node * f);
	  bool is_binding(Node * b);
	*/
};

//debug for scope_table
inline std::ostream & operator<<(std::ostream & os, const scope_table & s)
{
	os << "==================================================\n";
	for (const auto & [k, v] : s)
		os << "<name = " << k << ": type, value = " << v.first << ", "
		   << v.second << '\n';
	os << "==================================================\n";
	return os;
}

#endif
