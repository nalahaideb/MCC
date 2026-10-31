#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <set>
#include "constants.h"
#include "production.h"

struct grammar
{
    std::set< symbol > terminals =
		{"Type", "Primary", "NUM", "ID"};
    std::set< symbol > nonterminals
		{
			"Program", "FunDecl", "Params", "Param", "Block",
			"Item", "Decl", "Args", "Stmt", "Exp", "AndExp",
			"EqExp", "RelExp", "AddExp", "MulExp", "Unary"
		};
	
    symbol start;
	
    std::vector< production > productions
		{
			{"Program", {"FunDecl"}},
			{"Params", {}},
			{"Param", {}},
			{"Type", {}},
			{"Block", {}},
			{"Item", {}},
			{"Decl", {}},
			{"Stmt", {}},
			{"Exp", {}},
			{"AndExp", {}},
			{"EqExp", {}},
			{"RelExp", {}},
			{"AddExp", {}},
			{"MulExp", {}},
			{"Unary", {}},
			{"Primary", {}},
			{"Args", {}}
		};
	
    bool is_terminal(const symbol & s) const
	{ return terminals.find(s) != terminals.end(); }
};


/*
Program ::= FunDecl { FunDecl }	

FunDecl ::= Type ID "(" [ Params ] ")" Block

Params ::= Param { "," Param }

Param ::= Type ID

Type ::= "int" | "bool"

Block ::= "{" { Item } "}"

Item ::= Decl | Stmt -- declarations only in blocks

Decl ::= Type ID "=" Exp ";" -- declaration

Stmt ::= ID "=" Exp ";" -- assignment
       | ID "(" [ Args ] ")" ";" -- call statement
	   | "if" "(" Exp ")" Stmt [ "else" Stmt ]
	   | "while" "(" Exp ")" Stmt
	   | "return" Exp ";"
	   | "print" "(" Exp ")" ";"
	   | Block

Exp ::= AndExp { "||" AndExp }

AndExp ::= EqExp { "&&" EqExp }

EqExp ::= RelExp [ ("==" | "!=") RelExp ]

RelExp ::= AddExp [ ("<" | "<=" | ">" | ">=") AddExp ]

AddExp ::= MulExp { ("+" | "-") MulExp }

MulExp ::= Unary { ("*" | "/" | "%") Unary }

Unary ::= ("-" | "!") Unary | Primary

Primary ::= NUM | "true" | "false"
          | ID | ID "(" [ Args ] ")"
		  | "read" "(" ")"
		  | "(" Exp ")"
		  
Args ::= Exp { "," Exp }
*/

#endif
