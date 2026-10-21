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
  - they return the modified input to "lex()"
  - "lex()" handles the type deduction based off character
  - "lex()" will call the correct function to consume characters from the input
  - "lex()" collects output tokens as strings in a vector for file printing
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
  [/] lex int/floats
  [] lex sci notat floats (1E42, 42e10, -1.24e5, etc)
  [x] add support for extended assignment ops (-=, *=, ++, etc)
  [x] lex with / without spaces
  [x] strip comments
  [x] lex tokens into vectors based on line
  [x] block comments keep track of line count
  [x] tag tokens with type instead of recomputing when placed in "tokens"
  [x] read input from file
  [x] output tokens vector to file
  [x] fix single-comment lexer throwing an error when 'EOF' reached before '\n'
  [x] fix unary operator matching first, switch to longest match
  [] arg passing for file via flags (i.e. "mcc --tokens.mc")
  [] integer overflow / underflow checks
  [] switch line read to sstream buffer file content read
  [] run tests
  
  CLEANUP / OPTIMIZATION:
  [] probably dont need to actually "consume" the string, doing substr is expensive
  [] im leaving ';' in the TYPE function as a standalone, idk where to put it atm
  [] organize symbol checks into sets (?)
  [] make choosing lex function for "lex()" more efficient (with bits?)
  [] template / generalize functions, lots of duplicate code atm (block comm / braces)
  [] make variables more explicit (length or # of lines can never be negative, make them size_t, etc)
*/
//==============================================================================

class Lexer
{
public:
	Lexer() {}
	
	//comments
	inline std::vector< std::string > lex();
	inline void read_file(const std::string file);
private:
	inline void assign_tape(const std::string s)
	{ tape = s; }
	inline void to_file();
	//member functions
	inline void block_comment_lexer(std::string & input, int & line_ct);
	inline void single_comment_lexer(std::string & input);
    inline int TYPE(const int c);
	inline int lex_opr_helper(const char c, bool flag);
	inline std::string lex_var(std::string & input);
	inline std::string lex_opr(std::string & input);
	inline std::string lex_num(std::string & input);
	inline std::string select_token(const char c_, int & line_ct);

	//hashtables / sets
    constexpr static const auto * tok_key = &keyword_map;
    constexpr static const auto * tok_op = &operators;
	constexpr static const auto * single_op_ = &single_op;

	//return vector based on line
	std::vector< std::string > tokens;

	//copy of input
	std::string tape;

	//file name copy
	std::string file_name;
};

std::vector< std::string > Lexer::lex()
{
	int line_ct = 1;
	std::string tok;

	//return on empty file
	if (tape.empty())
	{
		tokens = {std::to_string(line_ct) + " EOF"};
	}

	while (!tape.empty())
	{
		const char c_ = tape[0];
		//ignore whitespace
		if (c_ != ' ' && c_ != '\t')
		{
			try
			{
				tok = select_token(c_, line_ct);
			}
			catch (Error & e) //break on any error
			{
				std::ofstream f(file_name + ".err");
				f << "error: line " + std::to_string(line_ct) + ":\n";
				return tokens;
			}
			//add output to current line vector (if it wasnt a comment)
			if (!tok.empty())
			{
			    tok = std::to_string(line_ct) + " " + tok;
		    	//cast to int to avoid warnings about signedness
				tokens.push_back(tok);
			}
		}
		else
			if (!tape.empty())
				tape.replace(0, 1, "");
			else
				break;
	}
	//break loop, add EOF to token list
	tokens.push_back(std::to_string(line_ct) + " EOF");

	//write output to file
	to_file();

	//maybe doesnt need to return the tokens since its writing to file
	//could fix this later on
	return tokens;
}

//extend to float, check todo
//check assmt for overflow
std::string Lexer::lex_num(std::string & input)
{
	long long int k = 0;
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
	k = std::stoll(ret);
	if (k > MAX_INT)
	{
		Error e("[lex_num]: NUM LARGER THAN MAX");
		throw e;
	}
	else //consume (size of int str symbols)
	{
		input.replace(0, ret.size(), "");
		k = 0;
		//clear leading zeros
		while (ret[k] == '0' && k < static_cast<long int>(ret.size() - 1))
			++k;
		ret.replace(0, k, "");
		return "NUM " + ret;
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
		c = input[k];
	    if (ISALPHA(c) || ISNUM(c) || c == '_')
			ret += c;
		else 
			break;
		++k;
	}
	//erase token from str
	if (!ret.empty()) input.replace(0, ret.size(), "");

	//check if keyword
	if (tok_key->find(ret) != tok_key->end())
		return tok_key->find(ret)->second;
	
	return "IDENT " + ret;
}

//extract operator tokens
std::string Lexer::lex_opr(std::string & input)
{
	std::string ret = {input[0]};
	if (input.size() > 1 &&
		tok_op->find({input[0],input[1]}) != tok_op->end())
		ret += input[1];
	
	//check if string is in operator tokens table
	const auto opr = tok_op->find(ret);
	
	//throw error on malformed operator
	if (opr == tok_op->end())
	{
		Error e("[lex_opr]: INVALID OPERATOR: [" + ret + "]");
		throw e;
	}

	if (!ret.empty())
		input.replace(0, ret.size(), "");
	return opr->second;
}


//removes all block comments, throws error on nested
void Lexer::block_comment_lexer(std::string & input, int & line_ct)
{
	size_t k = 0, end = input.find("*/"), size = input.size();
	if (end == std::string::npos || end <= 0)
	{
		Error e("[block_comment_lexer]: COMMENT UNTERMINATED");
		throw e;
	}
	//check if a block comment substr exists k < "/*" < end
	else if (input.find("/*", k + 1) <= end) 
	{
		Error e("[block_comment_lexer]: ILLEGAL NESTED COMMENT");
		throw e;		
	}
	while (k < end + 1)
	{
		if (input[k] == '\n')
			++line_ct;
		++k;
	}
	input = input.substr(k + 1, size - k);
}

//removes input until '\n' is found
//single comment at the end of file will throw error, fix later
void Lexer::single_comment_lexer(std::string & input)
{
    size_t k = input.find('\n');
	
	//if file ends with comment, set input to empty
    if (k == std::string::npos)
		input = "";
	else
		input.replace(0, k, "");
}

//this is pretty stupid but ill make it nicer later
int Lexer::TYPE(const int c)
{
	int type = -1;
	if (ISNUM(c)) //int
		type = 0;
	else if (ISALPHA(c) || c == '_') //character
		type = 1;
	else if (single_op_->find(c) != single_op_->end()) //operator
		type = 2;
	else if (c == '\n' || c == '\r') //newline
		type = 3;
	else if (c == ';') //semi, leaving here for now, should move later
		type = 4;
	return type;
}

//need to prepend with type when returning
std::string Lexer::select_token(const char c_, int & line_ct)
{
	int type = TYPE(c_);
	std::string tok = "";
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
			//on case for comments ("//", "/*")
			char c = ' ';
			if (tape.size() > 1 && tape[0] == '/')
			{
			    c = tape[1];
			}
			//single comments
			if (c == '/') single_comment_lexer(tape);
			//block comments
			else if (c == '*') block_comment_lexer(tape, line_ct);
			//regular operator
			else tok = lex_opr(tape);
			break;
		}
	case 3: //line increment / newline
		line_ct++;
		tape.replace(0, 1, "");
		break;
	case 4:
		tok = "SEMI";
		tape.replace(0, 1, "");
		break;
	default:
		std::string f = {c_};
		Error e("[lex]: INVALID SYMBOL: '" + f + "'");
		throw e;
	}
	return tok;
}

void Lexer::read_file(const std::string file)
{
	std::ifstream f(file);
    if (!f.is_open())
	{
		Error e({"[read_file]: FAILED TO OPEN FILE \"" + file + "\""});
		std::cout << e.message << std::endl;
		throw e;
    }

    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();
	file_name = file.substr(0, file.find('.'));
    tape = buffer.str();
}

void Lexer::to_file()
{
	std::string output_file = file_name + ".out";
	std::ofstream f(output_file);
	if (f.is_open())
		for (const auto & k : tokens)
			f << k << '\n';
}

#endif

