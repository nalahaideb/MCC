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

//escape characters that would break a quoted DOT label
inline std::string dot_escape(const std::string & s)
{
	std::string out;
	for (char c : s)
	{
		if (c == '"' || c == '\\') out += '\\';
		out += c;
	}
	return out;
}

//emits n and everything below it; returns n's id so the parent can draw its edge
inline int dot_node(const Node * n, std::ostream & os, int & next_id)
{
	const int id = next_id++;

	//label: kind, then leaf text and line on their own rows ("\\n" is a DOT newline)
	std::string label = dot_escape(kind_name(n->kind));
	if (!n->s.empty())    label += "\\n" + dot_escape(n->s);
	if (!n->line.empty()) label += "\\nline " + dot_escape(n->line);

	const bool glue = (n->kind == Kind::Seq || n->kind == Kind::Else);
	const bool leaf = (!n->l && !n->r && !n->s.empty());

	os << "  n" << id << " [label=\"" << label << "\"";
	if (glue)      os << ", style=dashed, color=gray50, fontcolor=gray40";
	else if (leaf) os << ", shape=box";
	os << "];\n";

	if (n->l)
	{
		const int c = dot_node(n->l, os, next_id);
		os << "  n" << id << " -> n" << c << " [label=\"l\"];\n";
	}
	if (n->r)
	{
		const int c = dot_node(n->r, os, next_id);
		os << "  n" << id << " -> n" << c << " [label=\"r\"];\n";
	}
	return id;
}

//writes a complete Graphviz document for the tree under root
inline void print_dot(const Node * root, std::ostream & os)
{
	os << "digraph AST {\n"
	   << "  graph [ordering=out];\n"          // keep l drawn left of r
	   << "  node  [shape=ellipse, fontname=\"Helvetica\"];\n"
	   << "  edge  [fontname=\"Helvetica\", fontsize=10];\n";

	if (root)
	{
		int next_id = 0;
		dot_node(root, os, next_id);
	}
	os << "}\n";
}

//wrapper so it doesn't collide with operator<< (S-expression) or raw()
struct Dot { const Node * n; };
inline Dot dot(const Node * n) { return Dot{ n }; }

inline std::ostream & operator<<(std::ostream & os, const Dot & d)
{
	print_dot(d.n, os);
	return os;
}

#endif
