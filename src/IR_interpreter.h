#ifndef IR_INTERPRETER_H
#define IR_INTERPRETER_H
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "error.h"
#include "IR_grammar.h"
#include "var_scope.h"

//split-up lines from file_content one struct per instruction type
//atoms  kept as strings

//FUNCTION f(t1, t2)
struct IR_func   { std::string name; std::vector< std::string > params; };
//LABEL l1
struct IR_label  { std::string name; };
//GOTO l1
struct IR_goto   { std::string label; };
//IF t1 relop t2 THEN l1 ELSE l2
struct IR_cond   { std::string lhs, relop, rhs, then_label, else_label; };
//RETURN t1
struct IR_return { std::string val; };
//t1 := CALL f(t2, t3)
struct IR_call   { std::string dest, func; std::vector< std::string > args; };
//op == ""  -> copy  (dest := lhs)
//lhs == "" -> unary (dest := op rhs), e.g. "t1 := - t2"
//else      -> binop (dest := lhs op rhs)
struct IR_assign { std::string dest, lhs, op, rhs; };

/*
  - read all file input into vector
  - iterate over vector, based off unique initial first token
    call the respective function to operate on it
  - for each function split args into atoms
  - evaluate and update registers or labels where applicable
*/
class IR_interpreter
{
 public:
	IR_interpreter() {};
	inline void to_file();
	//should read IR code in line-by-line to a vector
	inline void read(const std::string file);
	
	//should run after read, calling interpreting functions on
	//a switch-case
	inline void evaluate();
 private:
	
	//member funcs	

	//assign function name to an index in file_content
	inline IR_func func_eval(const std::string l);
	//if/then/else forks into two possible labels, moving read space
	inline IR_cond cond_eval(const std::string l);
	//jumps to a label and back again
	inline IR_call call_eval(const std::string l);
	inline IR_return return_eval(const std::string l);
	inline IR_assign assign_eval(const std::string l);
	inline IR_label label_eval(const std::string l);
	inline IR_goto goto_eval(const std::string l);
	//whitespace split of a line into tokens
	inline std::vector< std::string > split(const std::string l);
	//"f(t1, t2)" -> name = "f", args = {"t1", "t2"}
	inline void split_args(const std::string s, std::string & name,
						   std::vector< std::string > & args);
	//set read head to position from label
	inline void jump(const size_t i);
	inline size_t get_label_pos(const std::string l);
	inline void add_reg(const std::string r, const std::string val);
	inline void add_label(const std::string l);
	inline void decide(const std::string line);
	inline bool is_register(const std::string line);
	//member variables
	std::vector< std::string > file_content;
	std::unordered_map< std::string, size_t > labels;
	std::vector< std::string > output;
	var_scope vtable;
    IR_grammar * g; //needed?? idk yet
	size_t read_head = 0;
};
#endif
