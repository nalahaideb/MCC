#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <set>
#include "constants.h"
#include "production.h"
//==============================================================================
/*
  TODO:
  [] verify left-factorization
  [] verify dangling-else conflict resolution in parser

  OPTIMIZATIONS:
  [] custom hash for converting productions vector to productions set
*/
//==============================================================================
struct grammar
{
    // std::set< symbol > terminals =
	// 	{"Type", "Primary", "NUM", "ID"};
    // std::set< symbol > nonterminals
	// 	{
	// 		"Program", "FunDecl", "Params", "Param", "Block",
	// 		"Item", "Decl", "Args", "Stmt", "Exp", "AndExp",
	// 		"EqExp", "RelExp", "AddExp", "MulExp", "Unary"
	// 	};
	
    symbol start;

	//needs to be referecing the productions set?
	// bool is_terminal(const symbol & s) const
	// { return terminals.find(s) != terminals.end(); }

	//splits up possible matches ( {} or [] elements ) into cases that can
	//satisfy a production
	std::vector< production > productions
		{
			//artificial start state production
			{"START", {"Program", "$"}},
			
			//Program ::= FunDecl { FunDecl }
			{"Program", {"FunDecl", "ProgramChain"}},
			{"ProgramChain", {"FunDecl", "ProgramChain"}},
			{"ProgramChain", {}}, //epsilon

			//FunDecl ::= Type ID "(" [ Params ] ")" Block
			{"FunDecl", {"Type", "ID", "(", "FunDeclChain"}},
			{"FunDeclChain", {"Params", ")", "Block"}},
			{"FunDeclChain", {")", "Block"}},

			//Params ::= Param { "," Param }
			{"Params", {"Param", "ParamsChain"}},
			{"ParamsChain", {",", "Param", "ParamsChain"}},
			{"ParamsChain", {}}, //epsilon
			
			//Param ::= Param ::= Type ID
			{"Param", {"Type", "ID"}},

			//Type ::= "int" | "bool"
			{"Type", {"int"}},
			{"Type", {"bool"}},

			//Block ::= "{" { Item } "}"
			{"Block", {"{", "BlockItems", "}"}},
			{"BlockItems", {"Item", "BlockItems"}},
			{"BlockItems", {}}, //epsilon

			//Item ::= Decl | Stmt
			{"Item", {"Decl"}},
			{"Item", {"Stmt"}},

			//Decl ::= Type ID "=" Exp ";"
			{"Decl", {"Type", "ID", "=", "Exp", ";"}},

			//Stmt
			{"Stmt", {"if", "(", "Exp", ")", "Stmt", "ElsePart"}},
			{"Stmt", {"while", "(", "Exp", ")", "Stmt"}},
			{"Stmt", {"print", "(", "Exp", ")", ";"}},
			{"Stmt", {"return", "ReturnChain"}},
			{"Stmt", {"Block"}},
			{"Stmt", {"ID", "StmtIDChain"}},
			{"ElsePart", {"else", "Stmt"}},
			{"ElsePart", {}}, //epsilon

			{"StmtIDChain", {"=", "Exp", ";"}},
			{"StmtIDChain", {"(", "StmtCallChain"}},
			{"StmtCallChain", {"Args", ")", ";"}},
			{"StmtCallChain", {")", ";"}},
			
			{"ReturnChain", {"Exp", ";"}},
			{"ReturnChain", {";"}},

			//Exp ::= AndExp { "||" AndExp }
			{"Exp",     {"AndExp", "ExpChain"}},
			{"ExpChain", {"||", "AndExp", "ExpChain"}},
			{"ExpChain", {}}, //epsilon

			//AndExp ::= EqExp { "&&" EqExp }
			{"AndExp",     {"EqExp", "AndExpChain"}},
			{"AndExpChain", {"&&", "EqExp", "AndExpChain"}},
			{"AndExpChain", {}}, //epsilon

			//EqExp ::= RelExp [ ("==" | "!=") RelExp ]
			{"EqExp",     {"RelExp", "EqExpChain"}},
			{"EqExpChain", {"==", "RelExp"}},
			{"EqExpChain", {"!=", "RelExp"}},
			{"EqExpChain", {}}, //epsilon

			//RelExp ::= AddExp [ ("<" | "<=" | ">" | ">=") AddExp ]
			{"RelExp",     {"AddExp", "RelExpChain"}},
			{"RelExpChain", {"<",  "AddExp"}},
			{"RelExpChain", {"<=", "AddExp"}},
			{"RelExpChain", {">",  "AddExp"}},
			{"RelExpChain", {">=", "AddExp"}},
			{"RelExpChain", {}}, //epsilon

			//AddExp ::= MulExp { ("+" | "-") MulExp }
			{"AddExp",     {"MulExp", "AddExpChain"}},
			{"AddExpChain", {"+", "MulExp", "AddExpChain"}},
			{"AddExpChain", {"-", "MulExp", "AddExpChain"}},
			{"AddExpChain", {}}, //epsilon

			//MulExp ::= Unary { ("*" | "/" | "%") Unary }
			{"MulExp",     {"Unary", "MulExpChain"}},
			{"MulExpChain", {"*", "Unary", "MulExpChain"}},
			{"MulExpChain", {"/", "Unary", "MulExpChain"}},
			{"MulExpChain", {"%", "Unary", "MulExpChain"}},
			{"MulExpChain", {}}, //epsilon

			// Unary ::= ("-" | "!") Unary | Primary
			{"Unary", {"-", "Unary"}},
			{"Unary", {"!", "Unary"}},
			{"Unary", {"Primary"}},
			
			//Primary
			{"Primary", {"ID", "PrimaryChain"}},
			{"Primary", {"read", "(", ")"}},
			{"Primary", {"(", "Exp", ")"}},
			{"Primary", {"NUM"}},
			{"Primary", {"true"}},
			{"Primary", {"false"}},
			
			{"PrimaryChain", {"(", "PrimaryCallChain"}},
			{"PrimaryChain", {}}, //epsilon
			{"PrimaryCallChain", {"Args", ")"}},
			{"PrimaryCallChain", {")"}},
			
			//Args ::= Exp { "," Exp }
			{"Args", {"Exp", "ArgsChain"}},

			{"ArgsChain", {",", "Exp", "ArgsChain"}},
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
