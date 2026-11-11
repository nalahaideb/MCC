#ifndef NODE_H
#define NODE_H
#include <ostream>
#include <string>
//==============================================================================
/*
  OVERVIEW:
  Node struct has fields Kind, symbol, children (l, r) and detail
  - Kind: computable type for node, (TYPE, LT, RETURN, etc)
  - symbol: string representation in grammar
  - l,r children: NULL if leaf, at least one if nonterminal
  - line: for line tracking from token given by lexer

  Kind enum: used with kind_name(), returns a string for the enum in CST and in debug printing
*/
//==============================================================================

//==============================================================================
/*
  TODO:
  [] find method to minimize actual size of enum or convert to C macro(?), very annoying to tab up and down as of now
  [X] print CST
    [X] needs some form of strict type (enum ?) for determining what is what, more descriptive than Kind
	
  [] collapse CST into AST, print as S-expression
  [] need to add line field for tracing first error, e.g. (error: line 2: expected 'INT') or something
  
*/
//==============================================================================


typedef std::string symbol;

enum class Kind
{
	// leaves
	Ident, Num, Type, True, False, Read,
	//pairwise (or more in Seq's case)
	//Seq joins nodes into a list,
	//Else pairs else with of an if
	Seq, Else,
	//declarations
	Program, Func, Params, Param, Block, Decl,
	//statements
	Assign, If, While, Print, Return, Call,
	//operators
	Or, And, Eq, Ne, Lt, Le, Gt, Ge, Add, Sub, Mul, Div, Mod, Neg, Not
};

//map for kind of node 
inline const std::string kind_name(Kind k)
{
	switch (k)
	{
		case Kind::Ident:
			return "ident";
		case Kind::Num:
			return "num";
		case Kind::Type:
			return "type";
		case Kind::True:
			return "true";
		case Kind::False:
			return "false";
		case Kind::Read:
			return "read";
		case Kind::Seq:
			return "seq";
		case Kind::Else:
			return "else";
		case Kind::Program:
			return "program";
		case Kind::Func:
			return "fun";
		case Kind::Params:
			return "params";
		case Kind::Param:
			return "param";
		case Kind::Block:
			return "block";
		case Kind::Decl:
			return "decl";
		case Kind::Assign:
			return "assign";
		case Kind::If:
			return "if";
		case Kind::While:
			return "while";
		case Kind::Print:
			return "print";
		case Kind::Return:
			return "return";
		case Kind::Call:
			return "call";
		case Kind::Or:
			return "||";
		case Kind::And:
			return "&&";
		case Kind::Eq:
			return "==";
		case Kind::Ne:
			return "!=";
		case Kind::Lt:
			return "<";
		case Kind::Le:
			return "<=";
		case Kind::Gt:
			return ">";
		case Kind::Ge:
			return ">=";
		case Kind::Add:
			return "+";
		case Kind::Sub:
			return "-";
		case Kind::Mul:
			return "*";
		case Kind::Div:
			return "/";
		case Kind::Mod:
			return "%";
		case Kind::Neg:
			return "neg";
		case Kind::Not:
			return "!";
	}
	//default, please never reach this god
	return "?";
}

struct Node
{
	//type of node (nontermianl or terminal)
	Kind kind;
	//text for leaf terminals, empty for interior nonterminal nodes
	symbol s;
	Node * l, * r;
	//copy of token line
	std::string line;
	Node(Kind k, const symbol & text = "",
		 Node * const left = NULL, Node * const right = NULL,
		 const std::string ln = "")
		:kind(k), s(text), l(left), r(right), line(ln) {}
};

inline std::ostream & operator<<(std::ostream & os, const Node & n);

//prints conjoined statements for AST if node kind Seq or Else
inline void print_list(const Node * n, std::ostream & os)
{
	if (n->kind == Kind::Seq || n->kind == Kind::Else)
	{
		print_list(n->l, os);
		os << ' ';
		print_list(n->r, os);
	}
	else
		os << *n;
}

//s-exp printout
inline std::ostream & operator<<(std::ostream & os, const Node & n)
{
	//leaf only print s text
	if (!n.l && !n.r && !n.s.empty())
		return os << n.s;

	//parameter prints as (T x) no head word
	if (n.kind == Kind::Param)
		return os << '(' << *n.l << ' ' << *n.r << ')';

	os << '(' << kind_name(n.kind);
	if (n.l)
	{
		os << ' ';
		print_list(n.l, os);
	}
	if (n.r)
	{
		os << ' ';
		print_list(n.r, os);
	}
	return os << ')';
}
#endif
