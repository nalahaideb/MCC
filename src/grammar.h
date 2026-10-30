#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <map>
#include "token.h"
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

class Grammar
{
 public:

	std::map< token > rules =
		{
			
		};
	
};
#endif
