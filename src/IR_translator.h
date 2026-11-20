#ifndef IR_TRANSLATOR_H
#define IR_TRANSLATOR_H
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "error.h"
#include "node.h"
#include "var_scope.h"
//==============================================================================
/*
  OVERVIEW:
  - takes the output of a type-checker (decorated AST), traverses over the tree
    and matches arguments against an IR_grammar to translate into IR code
  - IR code is stored in a vector until the program is fully parsed, then output
    to a file and read by IR_interpreter

  - translations (cond, exp, stmt) map associated node Kind values to IR code
    to be placed in the returned IR code vector, multi-arg/line Kind values
	process available nodes until finished and batch-insert. control sequences
	insert labels, functions insert jumps 
	
  - temp and label counters reset per function, variables are mapped to temps
    through a stack of scopes like in symtable.h
	
  - &&, ||, ! and relops in a value resolve to 0 / 1 through trans_cond, so
    short-circuiting should be the same 
	
  - read() and print() are builtin CALLs
*/
//==============================================================================

//==============================================================================
/*
  TODO:
  [X] translations
     [X] define functions, arguments and returns
     [X] define declarations, statements and expressions
     [X] define control statements
     [X] define print() and read()
  [] IR_interpreter
*/
//==============================================================================

class IR_translator
{
public:
	IR_translator() {}

	inline void translate(const Node * root);
	inline void to_file(const std::string & path) const;
	inline const std::vector< std::string > & code() const;

private:
	//member funcs
	inline std::string new_temp();
	inline std::string new_label();
	inline void add_IR_code(const std::string & line);

	inline void enter();
	inline void exit();
	inline void bind(const std::string & var, const std::string & temp);
	inline const std::string & lookup(const Node * ident);

	inline void node_list(const Node * n, std::vector< const Node * > & out);
	inline bool is_cond(Kind k) const;

	inline void trans_function(const Node * f);
	inline void trans_stmt(const Node * n);
	inline void trans_exp(const Node * e, const std::string & place);
	inline void trans_cond(const Node * e, const std::string & lt, const std::string & lf);
	inline std::string trans_call(const std::string & f, const Node * args);

	//member variables
	std::vector< std::string > code_;
	std::list< var_scope > vtable_;
	int temp_count = 0;
	int label_count = 0;
};

#endif
