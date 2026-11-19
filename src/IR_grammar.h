#ifndef IR_GRAMMAR_H
#define IR_GRAMMAR_H
#include "production.h"
//==============================================================================
/*
  OVERVIEW:
  - grammar for the three-address IR produced by IR_translator from the
    type-checked AST, read by IR_interpreter
  - read() and print() are builtin functions reached through CALL
  - IR-specific terminals: COLONEQ (":="), FUNCTION, END, LABEL, GOTO, THEN, CALL
*/
//==============================================================================

//==============================================================================
/*
  TODO:
  [X] draft grammar
  [X] match grammar to IR_translator output
  [] 
  [] IR_interpreter
*/
//==============================================================================

/*
Program      -> Function { Function }
Function     -> FUNCTION IDENT "(" [ Params ] ")" Instructions END
Params       -> IDENT { "," IDENT }
Instructions -> Instruction { Instruction }

Instruction  -> LABEL IDENT
              | IDENT ":=" Rhs
              | GOTO IDENT
              | IF Atom relop Atom THEN IDENT ELSE IDENT
              | RETURN Atom

Rhs          -> Atom [ binop Atom ]
              | unop Atom
              | CALL IDENT "(" [ Args ] ")"

Args         -> Atom { "," Atom }
Atom         -> IDENT | NUM

unop         -> "-"
binop        -> "+" | "-" | "*" | "/" | "%"
relop        -> "==" | "!=" | "<" | "<=" | ">" | ">="

  AST -> IR TRANSLATIONS:

  Decl               t := <exp>

  Assign             t := <exp>, then x := t (so "b = !b" reads the old b)

  Num / True / False place := n / 1 / 0

  Ident              place := <temp bound to name>

  Add..Mod, Neg      operands into new temps, place := t1 op t2 / - t

  Eq..Ge, And, Or,   place := 0, jump through trans_cond, place := 1
  Not
  
  conditions         relops -> IF
                     &&, || -> short-circuit jumps,
                     ! -> swaps labels
					 anything else -> IF t != 0
  If / Else / While  LABEL, IF, GOTO
  Print / Read       t := CALL print(a) / place := CALL read()
  Call               t := CALL f(args) args into new temps left to right, 
  Return             t := <exp>, RETURN t
*/

struct IR_grammar
{
	symbol start_ = "Program";
	symbol EOF_ = "EOF";

	std::vector< production > productions
		{
			//Program ::= Function { Function }
			{"Program", {"Function", "ProgramChain"}},
			{"ProgramChain", {"Function", "ProgramChain"}},
			{"ProgramChain", {}}, //epsilon

			//Function ::= FUNCTION IDENT "(" [ Params ] ")" Instructions END
			{"Function", {"FUNCTION", "IDENT", "LPAREN", "Params", "RPAREN",
						  "Instructions", "END"}},

			//Params ::= [ IDENT { "," IDENT } ]
			{"Params", {"IDENT", "ParamsChain"}},
			{"Params", {}}, //epsilon
			{"ParamsChain", {"COMMA", "IDENT", "ParamsChain"}},
			{"ParamsChain", {}}, //epsilon

			//Instructions ::= Instruction { Instruction }
			{"Instructions", {"Instruction", "InstrChain"}},
			{"InstrChain", {"Instruction", "InstrChain"}},
			{"InstrChain", {}}, //epsilon

			//Instruction
			{"Instruction", {"LABEL", "IDENT"}},
			{"Instruction", {"IDENT", "COLONEQ", "Rhs"}},
			{"Instruction", {"GOTO", "IDENT"}},
			{"Instruction", {"IF", "Atom", "relop", "Atom",
							 "THEN", "IDENT", "ELSE", "IDENT"}},
			{"Instruction", {"RETURN", "Atom"}},

			//Rhs
			{"Rhs", {"Atom", "RhsChain"}},
			{"Rhs", {"unop", "Atom"}},
			{"Rhs", {"CALL", "IDENT", "LPAREN", "Args", "RPAREN"}},
			{"RhsChain", {"binop", "Atom"}},
			{"RhsChain", {}}, //epsilon

			//Args ::= [ Atom { "," Atom } ]
			{"Args", {"Atom", "ArgsChain"}},
			{"Args", {}}, //epsilon
			{"ArgsChain", {"COMMA", "Atom", "ArgsChain"}},
			{"ArgsChain", {}}, //epsilon

			//Atom ::= IDENT | NUM
			{"Atom", {"IDENT"}},
			{"Atom", {"NUM"}},

			//unop ::= "-"
			{"unop", {"MINUS"}},

			//binop ::= "+" | "-" | "*" | "/" | "%"
			{"binop", {"PLUS"}},
			{"binop", {"MINUS"}},
			{"binop", {"STAR"}},
			{"binop", {"SLASH"}},
			{"binop", {"PERCENT"}},

			//relop ::= "==" | "!=" | "<" | "<=" | ">" | ">="
			{"relop", {"EQ"}},
			{"relop", {"NE"}},
			{"relop", {"LT"}},
			{"relop", {"LE"}},
			{"relop", {"GT"}},
			{"relop", {"GE"}}
		};
};
#endif
