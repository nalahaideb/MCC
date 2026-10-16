#include "constants.h"
#include <vector>
/*
  TODO:
  [] lex vars
  [] lex int/floats
  [] lex sci not floats (1E42, 42e10, -1.24e5, etc)
  [] lex booleans
  [] add support for extended assignment ops (-=, *=, ++, etc)
  [] lex with / without spaces
  [] strip comments
  [] lex tokens into vectors based on line
  [] block comments keep track of line count
*/

/*
  should return a vector of vectors with tokens, each subvector denoting a line of formatted input 
*/
class Lexer
{

 public:
 Lexer(const std::string & input)
	 :tape(input)
	{}
	inline void run();
    //inline void block_comment_lexer(std::string & input, int & line_ct);
	inline int type(const std::string & input);
	inline std::string lex_var(const std::string & input);
	inline std::string lex_num(const std::string & input);
 private:
	//ptr to type_map in constants.h
    constexpr static const auto * tok_type = &type_map;
    constexpr static const auto * tok_op = &operators;
    /* constexpr static const auto * tok_num = &type_map;	 */
    /* constexpr static const auto * tok_char = &type_map;	 */
	std::string tape;
	std::vector< std::string > tokens;
};

//extend to float, check todo
//check assmt for overflow
std::pair< int, std::string > Lexer::lex_num(const std::string & input)
{
	int k = 0;
	while (ISNUM(input[k]))
	{
		++k;
	}
		
}

std::string Lexer::lex_var(const std::string & input)
{
	int k = 0;
	std::string ret;
	
	//break on non-char / underscore initial value
	if (!ISALPHA(input[k]) && input[k] != '_')
	{
		Error e("INVALID IDENT CHAR" + input[k]);
		throw e;
	}
	//break on non underscore / int / char symbol read
	while (input[k] != '\0')
	{
		const int c = input[k];
		if (c == ' ' || c == ';') break;
		else if (ISALPHA(c) || ISNUM(c) || ISNUM(c) || c == '_')
			ret += c;
		else 
		{
			Error e("INVALID IDENT CHAR" + char(input[k]));
			throw e;
		}
		++k;
	}
	return ret;
}

//preprocessing, removes all block comments, throws error otherwise
/* void Lexer::block_comment_lexer(std::string & input, int & line_ct) */
/* { */
/* 	if (input.empty()) return; */
	
/* 	std::vector< std::pair< int, int > > comment_blocks; */
/* 	size_t k = 0; */
/* 	//find beginning / end delimiters for block comments */
/* 	while (input[k] != '\0') */
/* 	{ */
/* 		if (k < input.size() - 1 && input[k] == '/' && input[k+1] == '*') */
/* 			comment_blocks.push_back({BCOM, k}); */
/* 		else if (k < input.size() - 1 && input[k] == '*' && input[k+1] == '/') */
/* 			comment_blocks.push_back({ECOM, k});//have to return substring of ECOM + 1 */
/* 		else if (input[k] == '\n') */
/* 			++line_ct; */
/* 	} */
/* 	//should be 2n comment delimiters per n comment section blocks */
/* 	if (comment_blocks.size() % 2) throw -1; */
/* 	std::string replacement; */
/* 	int k = 0; */
/* 	for (int c = 1; c < comment_blocks.size(); c += 2) */
/* 	{ */
/* 		const auto & begin = comment_blocks[c - 1]; */
/* 		const auto & end = comment_blocks[c]; */
		
/* 		//check for sequential comment block pairs */
/* 		if (begin.first != BCOM || end.first != ECOM) */
/* 		{ */
/* 			throw -1; */
/* 		} */
/* 		else //ignore chars found between the positions of the above comment blocks */
/* 		{ */
/* 			replacement += input.substr(input, begin.second) + ' '; */
/* 		} */
/* 	} */
/* 	//string without (correctly formatted) block comments  */
/* 	input = replacement; */
/* } */
