#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <unordered_set>
#include <unordered_map>
#include <string>

// #define ISNUM() ()
// #define
//types (and block comments)
enum 
	{
		INT, BOOL, IF, ELSE, WHILE, RETURN, TRUE, FALSE, PRINT, READ, BCOM, ECOM, SCOM
	};

typedef std::pair<std::string, int> token;

// const static std::unordered_set<std::string> alphabet = {};
// const static std::unordered_set<std::string> numbers = {};
const static std::unordered_map<std::string, std::string> operators =
	{
		{"<", "LT"}, {">", "GT"}, {"==","EQ"},{"<=","LE"},{">=","GE"},{"!","NOT"},{"||","OR"},{"&&","AND"},{"(","LPAREN"},{")","RPAREN"},{"+","PLUS"},{"-","MINUS"},{"/","DIV"},{"%","MOD"},{".","DECIMAL"}, {"=","ASSIGN"},{";","SEMI"}, {"{","LBRACE"}, {"}","RBRACE"}
	};

//type map
const static std::unordered_map<std::string, int> type_map =
	{
		{"int", INT}, {"bool", BOOL}, {"if", IF}, {"else", ELSE}, {"while", WHILE}, {"return", RETURN}, {"true", TRUE}, {"false", FALSE}, {"print", PRINT}, {"read", READ}, {"/*", BCOM}, {"*/", ECOM}, {"//", SCOM}
	};

#endif
