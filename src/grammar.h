#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <set>
#include "production.h"
//==============================================================================
/*
  TODO:
  [X] verify left-factorization
  [X] verify dangling-else conflict resolution in parser
  [X] change terminals in grammar to reflect lexer token outputs
  [] streamline token struct, two parts is kind of annoying
  OPTIMIZATIONS:
  [] custom hash for converting productions vector to productions set
*/
//==============================================================================
struct grammar
{	
    symbol start_ = "START";
	symbol EOF_ = "EOF";

	//splits up possible matches ( {} or [] elements ) into cases that can
	//satisfy a production
	std::vector< production > productions
		{
			//artificial start state production
			{"START", {"Program", "EOF"}},
			
			//Program ::= FunDecl { FunDecl }
			{"Program", {"FunDecl", "ProgramChain"}},
			{"ProgramChain", {"FunDecl", "ProgramChain"}},
			{"ProgramChain", {}}, //epsilon

			//FunDecl ::= Type ID "(" [ Params ] ")" Block
			{"FunDecl", {"Type", "IDENT", "LPAREN", "FunDeclChain"}},
			{"FunDeclChain", {"Params", "RPAREN", "Block"}},
			{"FunDeclChain", {"RPAREN", "Block"}},

			//Params ::= Param { "," Param }
			{"Params", {"Param", "ParamsChain"}},
			{"ParamsChain", {"COMMA", "Param", "ParamsChain"}},
			{"ParamsChain", {}}, //epsilon
			
			//Param ::= Param ::= Type ID
			{"Param", {"Type", "IDENT"}},

			//Type ::= "int" | "bool"
			{"Type", {"TYPE"}},

			//Block ::= "{" { Item } "}"
			{"Block", {"LBRACE", "BlockItems", "RBRACE"}},
			{"BlockItems", {"Item", "BlockItems"}},
			{"BlockItems", {}}, //epsilon

			//Item ::= Decl | Stmt
			{"Item", {"Decl"}},
			{"Item", {"Stmt"}},

			//Decl ::= Type ID "=" Exp ";"
			{"Decl", {"Type", "IDENT", "ASSIGN", "Exp", "SEMI"}},

			//Stmt
			{"Stmt", {"IF", "LPAREN", "Exp", "RPAREN", "Stmt", "ElsePart"}},
			{"Stmt", {"WHILE", "LPAREN", "Exp", "RPAREN", "Stmt"}},
			{"Stmt", {"PRINT", "LPAREN", "Exp", "RPAREN", "SEMI"}},
			{"Stmt", {"RETURN", "ReturnChain"}},
			{"Stmt", {"Block"}},
			{"Stmt", {"IDENT", "StmtIDChain"}},
			{"ElsePart", {"ELSE", "Stmt"}},
			{"ElsePart", {}}, //epsilon

			{"StmtIDChain", {"ASSIGN", "Exp", "SEMI"}},
			{"StmtIDChain", {"LPAREN", "StmtCallChain"}},
			{"StmtCallChain", {"Args", "RPAREN", "SEMI"}},
			{"StmtCallChain", {"RPAREN", "SEMI"}},
			
			{"ReturnChain", {"Exp", "SEMI"}},
			{"ReturnChain", {"SEMI"}},

			//Exp ::= AndExp { "||" AndExp }
			{"Exp",     {"AndExp", "ExpChain"}},
			{"ExpChain", {"OR", "AndExp", "ExpChain"}},
			{"ExpChain", {}}, //epsilon

			//AndExp ::= EqExp { "&&" EqExp }
			{"AndExp",     {"EqExp", "AndExpChain"}},
			{"AndExpChain", {"AND", "EqExp", "AndExpChain"}},
			{"AndExpChain", {}}, //epsilon

			//EqExp ::= RelExp [ ("==" | "!=") RelExp ]
			{"EqExp",     {"RelExp", "EqExpChain"}},
			{"EqExpChain", {"EQ", "RelExp"}},
			{"EqExpChain", {"NE", "RelExp"}},
			{"EqExpChain", {}}, //epsilon

			//RelExp ::= AddExp [ ("<" | "<=" | ">" | ">=") AddExp ]
			{"RelExp",     {"AddExp", "RelExpChain"}},
			{"RelExpChain", {"LT", "AddExp"}},
			{"RelExpChain", {"LE", "AddExp"}},
			{"RelExpChain", {"GT", "AddExp"}},
			{"RelExpChain", {"GE", "AddExp"}},
			{"RelExpChain", {}}, //epsilon

			//AddExp ::= MulExp { ("+" | "-") MulExp }
			{"AddExp",     {"MulExp", "AddExpChain"}},
			{"AddExpChain", {"PLUS", "MulExp", "AddExpChain"}},
			{"AddExpChain", {"MINUS", "MulExp", "AddExpChain"}},
			{"AddExpChain", {}}, //epsilon

			//MulExp ::= Unary { ("*" | "/" | "%") Unary }
			{"MulExp",     {"Unary", "MulExpChain"}},
			{"MulExpChain", {"STAR", "Unary", "MulExpChain"}},
			{"MulExpChain", {"SLASH", "Unary", "MulExpChain"}},
			{"MulExpChain", {"PERCENT", "Unary", "MulExpChain"}},
			{"MulExpChain", {}}, //epsilon

			// Unary ::= ("-" | "!") Unary | Primary
			{"Unary", {"MINUS", "Unary"}},
			{"Unary", {"NOT", "Unary"}},
			{"Unary", {"Primary"}},
			
			//Primary
			{"Primary", {"IDENT", "PrimaryChain"}},
			{"Primary", {"READ", "LPAREN", "RPAREN"}},
			{"Primary", {"LPAREN", "Exp", "RPAREN"}},
			{"Primary", {"NUM"}},
			{"Primary", {"TRUE"}},
			{"Primary", {"FALSE"}},
			
			{"PrimaryChain", {"LPAREN", "PrimaryCallChain"}},
			{"PrimaryChain", {}}, //epsilon
			{"PrimaryCallChain", {"Args", "RPAREN"}},
			{"PrimaryCallChain", {"RPAREN"}},
			
			//Args ::= Exp { "," Exp }
			{"Args", {"Exp", "ArgsChain"}},

			{"ArgsChain", {"COMMA", "Exp", "ArgsChain"}},
			{"ArgsChain", {}} //epsilon
		};	
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
