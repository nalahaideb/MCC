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

//op == "" -> copy  (dest := lhs)
//lhs == "" -> unary (dest := op rhs), e.g. "t1 := - t2"
//else -> binop (dest := lhs op rhs)
struct IR_assign { std::string dest, lhs, op, rhs; };

//caller state saved by CALL, restored by RETURN
//ret = CALL line, dest = register the return value goes into
struct IR_frame  { size_t ret; std::string dest; var_scope regs; };

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
	inline void to_file(const std::string s);
	//should read IR code in line-by-line to a vector
	inline void read(const std::string file);
	
	//should run after read, calling interpreting functions on
	//a switch-case
	inline void evaluate();
 private:
	
	//member funcs	

	inline void split_args(const std::string s, std::string & name,
						   std::vector< std::string > & args);
	inline void jump(const size_t i);
	inline void add_reg(const std::string r, const std::string val);
	inline void add_label(const std::string l);
	inline void decide(const std::string line);
	inline void find_main();
	inline void find_labels();
	inline void read_call(const IR_call & c);
	inline void func_call(const IR_call & c);
	inline void return_call(const IR_return & r);
	inline void cond_result(const std::string opr0,
							const std::string relop,
							const std::string opr1,
							const std::string then_label,
							const std::string else_label);
    inline size_t get_label_pos(const std::string l);
	inline size_t func_start();
	inline std::string print_call(const IR_call & c);
	inline std::string binop_result(const std::string opr0,
									const std::string operator_,
									const std::string opr1);
	inline std::string get_val(const std::string a);
	inline std::vector< std::string > split(const std::string l);

	//structures repesenting evaluated lines in IR code
	inline IR_func func_eval(const std::string l);
	inline IR_cond cond_eval(const std::string l);
	inline IR_call call_eval(const std::string l);
	inline IR_return return_eval(const std::string l);
	inline IR_assign assign_eval(const std::string l);
	inline IR_label label_eval(const std::string l);
	inline IR_goto goto_eval(const std::string l);
	
	//member variables
	std::vector< std::string > file_content;
	std::unordered_map< std::string, size_t > labels;
	std::unordered_map< std::string, size_t > funcs;
	std::vector< IR_frame > call_stack;
	std::vector< std::string > output;
	var_scope vtable;
	var_scope::iterator return_val;
    //IR_grammar * g; //needed?? idk yet
	size_t read_head = 0;
};
#endif
