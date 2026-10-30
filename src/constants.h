#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include "token.h"
#define MAX_INT 2147483647

//operator map
const static std::unordered_map< std::string, std::string > operators =
	{
		{"<", "LT"}, {">", "GT"}, {"==","EQ"},{"<=","LE"}, {"!=", "NE"},
		{">=","GE"}, {"!","NOT"}, {"||","OR"}, {",", "COMMA"}, {"&&","AND"},
		{"(","LPAREN"}, {")","RPAREN"}, {"{","LBRACE"}, {"}","RBRACE"},
		{"+","PLUS"}, {"-","MINUS"}, {"/","SLASH"}, {"*", "STAR"},
		{"%","PERCENT"}, {"=","ASSIGN"}, {";", "SEMI"}
	};

//keyword map, this probably doesnt need to be a hashtable, just make a simple "to_upper" func instead and make it an unordered set
const static std::unordered_map< std::string, std::string > keyword_map =
	{
		{"int", "INT"}, {"bool", "BOOL"}, {"if", "IF"}, {"else", "ELSE"},
		{"while", "WHILE"}, {"return", "RETURN"}, {"true", "TRUE"},
		{"false", "FALSE"}, {"print", "PRINT"}, {"read", "READ"}
	};

//symbol sets
//used for deducing an operator in lex()
const static std::unordered_set< char > single_op =
	{
		'!', '/', '&', '|', '+', '-', '*', '%', '=',
		'(', ')', '[', ']', '{', '}', '<', '>', ','
	};

#endif
