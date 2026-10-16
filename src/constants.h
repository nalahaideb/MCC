#include <unordered_set>
#include <unordered_map>
#include <string>


//types (and comments)
enum 
	{
		INT, BOOL, IF, ELSE, WHILE, RETURN, TRUE, FALSE, PRINT, READ, BCOM, ECOM, SCOM
	};

typedef std::pair<std::string, int> token;

// const static std::unordered_set<char> alphabet = {};

// const static std::unordered_set<char> operators = {};

//type map
const static std::unordered_map<std::string, int> type_map =
	{
		{"int", INT}, {"bool", BOOL}, {"if", IF}, {"else", ELSE}, {"while", WHILE}, {"return", RETURN}, {"true", TRUE}, {"false", FALSE}, {"print", PRINT}, {"read", READ}, {"/*", BCOM}, {"*/", ECOM}, {"//", SCOM}
	};
