#ifndef NODE_H
#define NODE_H
#include <string>
#include <iostream>
typedef std::string symbol;


struct Node
{
	symbol s;
	Node * l, * r;
	Node(const symbol & sym,
		 Node * const left = NULL,
		 Node * const right = NULL)
		:s(sym), l(left), r(right) {}
};

inline std::ostream & operator<<(std::ostream & os, const Node & node)
{
    os << "(";
	os << node.s;
    if (node.l)
	{
        os << " ";
		os << *node.l;
	}
    if (node.r)
	{
        os << " ";
		os << *node.r;
		os << ")";
	}
    return os;
}

#endif
