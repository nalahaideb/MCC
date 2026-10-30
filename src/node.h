#ifndef NODE_H
#define NODE_H
#include "token.h"

class Node
{
 public:
 Node(const token & x,
	  const Node * l,
	  const Node * r)
	 :t(x), left(l), right(r) {}
	token t;
	Node * l, * r;
};
#endif
