#ifndef NODE_H
#define NODE_H
#include "token.h"

struct Node
{
	symbol s;
	Node * l, * r;
	Node(const symbol & sym,
		 Node * const left,
		 Node * const right)
		:s(sym), l(left), r(right) {}
};
#endif
