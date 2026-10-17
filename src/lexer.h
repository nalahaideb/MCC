#ifndef LEXER_H
#define LEXER_H
#include "constants.h"
#include <vector>
//==============================================================================
/*
  TODO:
  [] lex vars
  [] lex int/floats
  [] lex sci notat floats (1E42, 42e10, -1.24e5, etc)
  [] lex booleans
  [] add support for extended assignment ops (-=, *=, ++, etc)
  [] lex with / without spaces
  [] strip comments
  [] lex tokens into vectors based on line
  [] block comments keep track of line count
  [] organize symbol checks into sets (?)
*/
//==============================================================================

//==============================================================================
/*
  OVERVIEW:

  - each function does its own designated task, "consuming" chars that match it
  - they return the modified input to "run()"
  - "run()" handles the type deduction based off character
  - "run()" will call the correct function to consume characters from the input
  - "run()" collects output tokens as strings in a vector for file printing
  - "lex_var()" consumes valid IDENT symbols
  - "lex_num()" consumes valid NUM symbols (float, int, double, etc)
  - "lex_type()" consumes valid TYPE symbols (int, bool, while, etc)
  - "lex_op()" consumes valid OPR symbols (==, !, *, ++, etc)
  
*/
//==============================================================================
class Lexer
{

 public:
 Lexer(const std::string & input)
	 :tape(input)
	{}
	inline void run();
    //inline void block_comment_lexer(std::string & input, int & line_ct);
	inline int lex_type(std::string & input);
	inline std::string lex_var(std::string & input);
	inline std::pair< std::string, std::string > lex_opr(std::string input);
	inline std::pair< int, std::string > lex_num(std::string & input);
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
std::pair< int, std::string > Lexer::lex_num(std::string & input)
{
	int k = 0;
	std::string ret;
	while (ISNUM(input[k]))
	{
		ret += input[k];
		++k;
		//add loop here to extract ints after sci notat / decimal
	}
	//throw error on digit count > MAX_INT digit count
	if (ret.size() > 11)
	{
		Error e("NUM LARGER THAN MAX");
		throw e;
	}
	else // convert to num
	{
		int pow = 1;
		k = 0;
		
		//set to reg int for same signedness, the warning was getting annoying
		int size = ret.size() - 1;
		for (int v = 0; v < size; ++v)
			pow *= 10;
		for (const auto & d : ret)
		{
		    k += pow * (d-48);
			pow /= 10;
		}
	}
	//attempt to build num, throw error if larger than MAX_INT
	if (k > MAX_INT)
	{
		Error e("NUM LARGER THAN MAX");
		throw e;
	}
	else //consume (size of int str symbols), return pair
	{
		std::pair< int, std::string > number = {k, ret};
		input = input.substr(ret.size(), (input.size() - ret.size()));
		return number;
	}
}

std::string Lexer::lex_var(std::string & input)
{
	int k = 0;
	std::string ret;
	
	//throw on non-char / underscore initial value
	if (!ISALPHA(input[k]) && input[k] != '_')
	{
		Error e("INVALID IDENT CHAR" + input[k]);
		throw e;
	}
	//break on non underscore / int / char symbol read
	while (input[k] != '\0')
	{
		const int c = input[k];
	    if (ISALPHA(c) || ISNUM(c) || ISNUM(c) || c == '_')
		{
			ret += c;
		}
		else 
		{
			break;
		}
		++k;
	}
	return ret;
}

std::pair< std::string, std::string > Lexer::lex_opr(std::string input)
{
	std::string ret;
	int k = 0;
	while (input[k] != '\0')
	{
		ret += input[k];
		++k;
	}

	const auto opr = tok_op->find(ret);
	//throw error on malformed operator
	if (opr == tok_op->end())
	{
		Error e("INVALID OPERATOR:" + ret);
		throw e;
	}
	else
	{
		return {opr->first, opr->second};
	}
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

#endif
