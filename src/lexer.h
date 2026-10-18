#ifndef LEXER_H
#define LEXER_H
#include "constants.h"
#include <vector>
//==============================================================================
/*
  TODO:
  [x] lex vars
  [x] lex int/floats
  [] lex sci notat floats (1E42, 42e10, -1.24e5, etc)
  [] lex booleans
  [x] add support for extended assignment ops (-=, *=, ++, etc)
  [] lex with / without spaces
  [] strip comments
  [] lex tokens into vectors based on line
  [] block comments keep track of line count
  [] organize symbol checks into sets (?)
  [] make choosing lex function for "run()" more efficient (with bits?)
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
  - "lex_op()" consumes valid OPR symbols (==, !, *, ++, etc)
  
*/
//==============================================================================
class Lexer
{
public:
	Lexer(const std::string & input)
		:tape(input)
	{}
    inline void block_comment_lexer(std::string & input, int & line_ct);
	inline void single_comment_lexer(std::string & input, int & line_ct);
	inline int lex_type(std::string & input);
	inline std::string lex_var(std::string & input);
	inline std::string lex_opr(std::string input);
	inline std::string lex_num(std::string & input);
	inline std::vector< std::vector< std::string > > lex(std::string & input);
	
private:
    inline int TYPE(const int c);
	
	//hashtables
    constexpr static const auto * tok_key = &keyword_map;
    constexpr static const auto * tok_op = &operators;
	constexpr static const auto * opr = &operator_c;
	//return vector based on line
	std::vector< std::vector< std::string > > tokens;
	//copy of input
	std::string tape;
};

std::vector< std::vector< std::string > > Lexer::lex(std::string & input)
{
	int k = 0, line_ct = 0, type = -1;
	std::string tok;
	while (input[k] != '\0')
	{
		type = TYPE(input[k]);
		switch (type)
		{
		case 0: //int
			tok = lex_num(input);
			break;
		case 1: //character
			tok = lex_var(input);
			break;
		case 2: //operator
			{
			const char c = input[k + 1];
			//on case for comments ("//", "/*")
			if (k < input.size() - 1 && (c == '/' || c == '*'))
			{
				//single comments
				if (c == '/') single_comment_lexer(input, line_ct);
				//block comments
				else block_comment_lexer(input, line_ct); 
			}
			//regular operator
			else tok = lex_opr(input);
			break;
			}
		case 3: //line increment
			line_ct++;
			break;
		default:
			std::string f = {input[k]};
			Error e("[lex]: INVALID SYMBOL: '" + f + "'");
		}
		
		//add output to current line vector
		tokens[line_ct].push_back(tok);
		++k;
	}
}

//extend to float, check todo
//check assmt for overflow
std::string Lexer::lex_num(std::string & input)
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
		Error e("[lex_int]: NUM DIGIT COUNT > MAX");
		throw e;
	}
	//attempt to build num, throw error if larger than MAX_INT
	if (k > MAX_INT)
	{
		Error e("[lex_num]: NUM LARGER THAN MAX");
		throw e;
	}
	else //consume (size of int str symbols), return pair
	{
		return ret;
	}
}

std::string Lexer::lex_var(std::string & input)
{
	int k = 0;
	std::string ret;
	
	//throw on non-char / underscore initial value
	if (!ISALPHA(input[k]) && input[k] != '_')
	{
		Error e("[lex_var]: INVALID IDENT CHAR" + input[k]);
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

//need to fix returning "//" as an operator instead of comment
//goes for "/*" too
std::string Lexer::lex_opr(std::string input)
{
	std::string ret;
	int k = 0;
	while (input[k] != '\0')
	{
		ret += input[k];
		++k;
	}
	//check if string is in operator tokens table
	const auto opr = tok_op->find(ret);
	
	//throw error on malformed operator
	if (opr == tok_op->end())
	{
		Error e("[lex_opr]: INVALID OPERATOR: " + ret);
		throw e;
	}
	else
	{
		return opr->second;
	}
}

//preprocessing, removes all block comments, throws error on nested
void Lexer::block_comment_lexer(std::string & input, int & line_ct)
{
	if (input.empty()) return;
	bool nested = false;
	int k = 0;
	while (input[k] != '\0')
	{
		if (nested)
		{
			Error e("[block_comment_lexer]: ILLEGAL NESTED COMMENT");
			throw e;
		}
		else if (k < input.size() - 1 && input.substr(k, 2) == "/*")
			nested = true;
		else if (input[k] == '\n' || input[k] == '\r')
			++line_ct;
		else if (k < input.size() - 1 && input.substr(k, 2) == "*/")
		{
			input = input.substr(k++, input.size() - k);
			break;
		}
		++k;
	}
}

//removes input until '\n' is found
//single comment at the end of file will throw error, fix later
void Lexer::single_comment_lexer(std::string & input, int & line_ct)
{
    size_t k = input.find('\n'); 
    if (k == std::string::npos)
	{
		Error e("[single_comment_lexer]: EOF REACHED: ");
		throw e;
	}
	else
	{
		input.substr(k, input.size() - k);
		++line_ct;
	}
}

//this is pretty stupid but ill make it nicer later
int Lexer::TYPE(const int c)
{
	int type = -999;
	if (ISNUM(c)) //numeral
		type = 0;
	else if (ISALPHA(c) || c == '_') //character
		type = 1;
	else if (opr->find(c) != opr->end()) //operator
		type = 2;
	else if (c == '\n' || c == '\r') //newline
		type = 3;
	else
		type = -1;
	return type;
}
#endif
