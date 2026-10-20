#ifndef LEXER_H
#define LEXER_H
#include "constants.h"
#include "error.h"
#include "helper.h"
#include <vector>
//==============================================================================
/*
  OVERVIEW:

  - each function does its own designated task, "consuming" chars that match it
  - they return the modified input to "run()"
  - "run()" handles the type deduction based off character
  - "run()" will call the correct function to consume characters from the input
  - "run()" collects output tokens as strings in a vector for file printing
  - "lex_keyword()" consumes valid TYPE symbols, takes precedence over lex_var()
  - "lex_var()" consumes valid IDENT symbols
  - "lex_num()" consumes valid NUM symbols (float, int, double, etc)
  - "lex_op()" consumes valid OPR symbols (==, !, *, ++, etc)
*/
//==============================================================================

//==============================================================================
/*
  TODO:
  [x] lex vars
  [x] lex int/floats
  [] lex sci notat floats (1E42, 42e10, -1.24e5, etc)
  [x] add support for extended assignment ops (-=, *=, ++, etc)
  [x] lex with / without spaces
  [x] strip comments
  [x] lex tokens into vectors based on line
  [x] block comments keep track of line count
  [] tag tokens with type instead of recomputing when placed in "tokens"
  [] read input from file
  [] output tokens vector to file
  [] fix single-comment lexer throwing an error when 'EOF' reached before '\n'

  CLEANUP / OPTIMIZATION:
  [] probably dont need to actually "consume" the string, doing substr is expensive
  [] im leaving ';' in the TYPE function as a standalone, idk where to put it atm
  [] organize symbol checks into sets (?)
  [] make choosing lex function for "run()" more efficient (with bits?)
  [] template / generalize functions, lots of duplicate code atm (block comm / braces)
  [] make variables more explicit (length or # of lines can never be negative, make them size_t, etc)
*/
//==============================================================================

class Lexer
{
public:
	Lexer(const std::string & input)
		:tape(input)
	{}

	//comments
    inline void block_comment_lexer(std::string & input, int & line_ct);
	inline void single_comment_lexer(std::string & input, int & line_ct);
	inline std::string lex_keyword(std::string & input);
	inline std::string lex_var(std::string & input);
	inline std::string lex_opr(std::string & input);
	inline std::string lex_num(std::string & input);
	inline std::vector< std::vector< std::string > > lex();
	inline void to_file(const std::string file);
private:
    inline int TYPE(const int c);
	inline std::string select_token(const char c_, int & line_ct);
	//hashtables
    constexpr static const auto * tok_key = &keyword_map;
    constexpr static const auto * tok_op = &operators;
	constexpr static const auto * single_op = &operator_c;
	//return vector based on line
	std::vector< std::vector< std::string > > tokens;
	//copy of input
	std::string tape;
};

std::vector< std::vector< std::string > > Lexer::lex()
{
	int line_ct = 0;
	std::string tok;
	while (!tape.empty())
	{
		const char c_ = tape[0];
		//ignore whitespace
		if (c_ != ' ')
		{
			try
			{
				std::cout << "selecting, tape = [" << tape  <<"]\n";
				tok = select_token(c_, line_ct);
			}
			catch (Error & e) //break on any error
			{
				std::cout << "ERROR: " << e.message << std::endl;
				return tokens;
			}
			//add output to current line vector (if it wasnt a comment)
			if (!tok.empty())
			{
				std::cout << "[lex] TOK NOT EMPTY\n";
				tok = INT_STR(line_ct) + " " + tok;
				std::cout << "[lex] linect, tokenv size = " << line_ct << ", " << tokens.size() << std::endl;
				//cast to int to avoid warnings about signedness
				if (line_ct > static_cast<int>(tokens.size()))
				{
					while (static_cast<int>(tokens.size()) <= line_ct)
					{
						std::vector< std::string > temp;
						tokens.push_back(temp);
					}
				}
				tokens[line_ct].push_back(tok);
			}
		}
		else
		{
			std::cout << "[lex] input size = " << tape.size() << std::endl;
			std::cout << "[lex] SKIP SPACE\n";
			if (!tape.empty())
				tape.replace(0, 1, "");
				//tape = tape.substr(1, tape.size() - 1);
			else
				break;
		}
	}
	std::string end_line = {INT_STR(line_ct)};
	std::vector< std::string > end_of_file_v = {end_line + " EOF"};
	tokens.push_back(end_of_file_v);
	return tokens;
}

//extend to float, check todo
//check assmt for overflow
std::string Lexer::lex_num(std::string & input)
{
	int k = 0;
	std::string ret;
	while (ISNUM(input[k]))
	{
		ret += input[k++];
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
	else //consume (size of int str symbols)
	{
		input = input.substr(ret.size(), input.size() - ret.size());
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
	char c = input[k];
	while (c != '\0')
	{
		//const char c = input[k];
		c = input[k];
	    if (ISALPHA(c) || ISNUM(c) || c == '_')
			ret += c;
		else 
			break;
		++k;
	}
	if (!ret.empty()) input.replace(0, ret.size(), "");
	std::cout << "[lexvar] exiting with ret = [" << ret
			  << "], input = [" << input << "]\n";
	return ret;
}

//need to fix returning "//" as an operator instead of comment
//goes for "/*" too
std::string Lexer::lex_opr(std::string & input)
{
	size_t k = 0;
	std::string ret;
	std::cout << "[lexopr] in opr\n";
	while (k < input.size())
	{
		if (single_op->find(input[k]) == single_op->end() || input[k] == ' ')
			break;
		ret += input[k];
		++k;
	}
	std::cout << "[lexopr] ret = " << ret << std::endl;

	//check if string is in operator tokens table
	const auto opr = tok_op->find(ret);
	//throw error on malformed operator
	if (opr == tok_op->end())
	{
		Error e("[lex_opr]: INVALID OPERATOR: [" + ret + "]");
		throw e;
	}
	else
	{
		if (!ret.empty())
			input.replace(0, ret.size(), "");
		return opr->second;
	}
}

//removes all block comments, throws error on nested
void Lexer::block_comment_lexer(std::string & input, int & line_ct)
{
	if (input.empty()) return;
	size_t k = 0, end = input.find("*/"), size = input.size();
	if (end == std::string::npos || end <= 0)
	{
		Error e("[block_comment_lexer]: COMMENT UNTERMINATED");
		throw e;
	}
	while (k < end + 1)
	{
		if (input[k] == '\n') ++line_ct;
		++k;
	}

	input = input.substr(k + 1, size - k);
	std::cout << "input after substr = " << input << std::endl;
	// input.replace(0, 1, "");
}

//removes input until '\n' is found
//single comment at the end of file will throw error, fix later
void Lexer::single_comment_lexer(std::string & input, int & line_ct)
{
    size_t k = input.find('\n');
	//file ends with comment, set input to empty
    if (k == std::string::npos)
	{
		input = "";
	}
	else
	{
		if (input.size() - k > 0)
			input = input.substr(k, input.size() - k);
		else input = {};
		++line_ct;
	}
}

//this is pretty stupid but ill make it nicer later
int Lexer::TYPE(const int c)
{
	int type = -1;
	if (ISNUM(c)) //int
		type = 0;
	else if (ISALPHA(c) || c == '_') //character
		type = 1;
	else if (single_op->find(c) != single_op->end()) //operator
		type = 2;
	else if (c == '\n' || c == '\r') //newline
		type = 3;
	else if (c == ';') //semi, leaving here for now, should move later
		type = 4;
	std::cout << "[type] returning type:" << type << std::endl;
	return type;
}

std::string Lexer::select_token(const char c_, int & line_ct)
{
	int type = TYPE(c_);
	std::string tok = "";
	std::cout << "[seltok], type = " << type << ", char: " << c_ << std::endl;
	switch (type)
	{
	case 0: //int
		tok = lex_num(tape);
		break;
	case 1: //character
		tok = lex_var(tape);
		break;
	case 2: //operator
		{
			std::cout << "case 2, tape = [" << tape << "]\n"; 
			//on case for comments ("//", "/*")
			char c = ' ';
			if (tape.size() > 1)
			{
			    c = tape[1];
			}
			//single comments
			if (c == '/') single_comment_lexer(tape, line_ct);
			//block comments
			else if (c == '*') block_comment_lexer(tape, line_ct);
			//regular operator
			else tok = lex_opr(tape);
			break;
		}
	case 3: //line increment / newline
		line_ct++;
		tape = tape.substr(1, tape.size() - 1);
		break;
	case 4:
		tok = "SEMI";
		tape = tape.substr(1, tape.size() - 1);
		break;
	default:
		std::string f = {c_};
		Error e("[lex]: INVALID SYMBOL: '" + f + "'");
		throw e;
	}
	std::cout << "[seltok] returning with [" << tok <<"]\n";
	return tok;
}

inline void Lexer::to_file(const std::string file)
{
	try
	{
		std::fstream f(file);
		if (f.is_open())
		{
			for (const auto & k : tokens)
				for (const auto & j : k)
					f << j << '\n';
		}
	}
	catch (Error & e)
	{
		std::ofstream x("../part1/part1/tokens.fail");
		x << "error: failed to write to " + file + ", no such file exists";
	}
}

#endif

