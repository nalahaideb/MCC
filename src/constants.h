#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <fstream>
#include "helper.h"

#define MAX_INT 2147483647

class Error
{
	public:
	Error(const std::string m)
	:message(m) {}
	
	const std::string message;
};

//types (and block comments)
enum 
	{
		INT, BOOL, IF, ELSE, WHILE,
		RETURN, TRUE, FALSE, PRINT,
		READ, BCOM, ECOM, SCOM
	};

// typedef std::pair< std::string, int > token;

//operator map
const static std::unordered_map< std::string, std::string > operators =
	{
		{"<", "LT"}, {">", "GT"}, {"==","EQ"},{"<=","LE"}, {"!=", "NEQ"},
		{">=","GE"}, {"!","NOT"}, {"||","OR"}, {"&&","AND"}, {"(","LPAREN"},
		{")","RPAREN"}, {"{","LBRACE"}, {"}","RBRACE"}, {"[", "LBRACK"},
		{"]", "RBRACK"}, {"+","PLUS"}, {"-","MINUS"}, {"/","DIV"}, {"*", "MULT"},
		{"%","MOD"}, {".","DECIMAL"}, {"=","ASSIGN"}, //move maybe? {";","SEMI"},
		{"++", "SINC"}, {"--", "SDEC"}, {"+=", "CADD"}, {"-=", "CMIN"},
		{"/=", "CDIV"}, {"%=", "CMOD"}, {"*=", "CMULT"}
	};

//keyword map
const static std::unordered_map< std::string, std::string > keyword_map =
	{
		{"int", "INT"}, {"bool", "BOOL"}, {"if", "IF"}, {"else", "ELSE"},
		{"while", "WHILE"}, {"return", "RETURN"}, {"true", "TRUE"}, {"false", "FALSE"},
		{"print", "PRINT"}, {"read", "READ"}
	};

//symbol sets
//used for deducing an operator in run()
const static std::unordered_set< char > operator_c = {'!', '/', '&', '|', '+', '-', '*', '%', '=', '(', ')', '[', ']', '{', '}', '<', '>'};
#endif
