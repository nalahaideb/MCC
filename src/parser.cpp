#include "parser.h"
#include "helper.h"

//==================================================================
//                     TAPE / SYMBOL READ FUNCS
//==================================================================

//get next token, returns EOF if pos past tape size
token Parser::look(const size_t pos) const
{
	token eof_ = {g->EOF_, ""};
	return (pos < tape->size() ? (*tape)[pos] : eof_);
};

//used to get actual terminal value instead of file.out-style piped value
symbol Parser::terminal_of(const token & t) const
{
	//keywords arrive as type TYPE with the keyword in value; INT/BOOL arent
	//grammar terminals stay TYPE
	if (t.type == "TYPE" && terminals.count(t.value))
		return t.value;

	//EOF has no type field, checks for going past tape
	if (t.type.empty())
		return t.value.empty() ? g->EOF_ : t.value;

	return t.type;
}

//==================================================================
//                 PARSE TABLE FUNC
//==================================================================


//combine NULLABLE, FIRST, FOLLOW sets into parse_table
void Parser::populate_parse_table()
{
	for (const auto & p : g->productions)
	{
		const symbol & A = p.prod_symbol;
		const std::vector< symbol > & alpha = p.produced;

		//cells this production belongs in
		std::unordered_set< symbol > select;

		//FIRST(alpha), walk until a non-nullable symbol
		bool alpha_nullable = true;
		for (const auto & s : alpha)
		{
			const auto f = first.find(s);
			if (f != first.end())
				select.insert(f->second.begin(), f->second.end());

			if (!nullable.count(s))
			{
				alpha_nullable = false;
				break;
			}
		}

		//A may be skipped when the next token can follow A
		if (alpha_nullable)
		{
			const auto f = follow.find(A);
			if (f != follow.end())
				select.insert(f->second.begin(), f->second.end());
		}

		auto & row = parse_table[A];
		for (const auto & t : select)
		{
			const auto ins = row.emplace(t, alpha);
			//cell already holds a different production -> not LL(1)
			if (!ins.second && ins.first->second != alpha)
			{
				//prevent throw on ElsePart
				if (A == "ElsePart" && t == "ELSE")
					continue;
				Error e("[populate_parse_table] ERROR: GRAMMAR NOT LL(1): conflict at M[" + A + ", " + t + "]");
				throw e;
			}
		}
	}
}

//==================================================================
//                 TOKEN AND LOOKUP FUNCS
//==================================================================

//string mapper for AST s-exp printout using tokens
std::string Parser::found(const token & t) const
{
	const symbol term = terminal_of(t);
	//'x', '42'
	if (term == "IDENT" || term == "NUM")
		return "'" + t.value + "'";
	//INT to 'int'
	if (term == "TYPE")
		return readout(t.value);
	//ELSE to 'else', EOF to end of file
	return readout(term);
}

//fail msg on bad parse
void Parser::fail(const std::string & expected) const
{
	const token & c = look(pos);
	Error e("error: line " +
			c.line + ": expected " +
			expected + " but found " + found(c));
	throw e;
}

//expected tokens for a given nonterminal A
//called when an unexpected token or lack of token is found
std::string Parser::expected_in(const symbol & A) const
{
	std::set< std::string > names;
	for (const auto & cell : parse_table.at(A))
		names.insert(readout(cell.first));

	std::string out;
	for (const auto & n : names)
		out += (out.empty() ? "" : " or ") + n;
	return out;
}

//create a return token based off symbol
token Parser::consume(const symbol & terminal)
{
	//unable to deduce desired symbol
	if (peek() != terminal)
		fail(readout(terminal));
	//passes
	const token t = look(pos);
	++pos;
	return t;
}

//terminal node
Node * Parser::leaf(Kind k, const symbol & terminal)
{
	const token t = consume(terminal);
	//terminals must retain literal text for printout
	const bool verbatim = (k == Kind::Ident || k == Kind::Num);
	return new Node(k, verbatim ? t.value : lowercase(t.value), NULL, NULL, t.line);
}

//production here MAY OR MAY NOT have a production, epsilon allowed
//production for A via table lookup, NULL if no such production
const std::vector< symbol > * Parser::predict(const symbol & A) const
{
	const auto row = parse_table.find(A);
	if (row == parse_table.end())
		return NULL;
	const auto cell = row->second.find(peek());
	return (cell == row->second.end() ? NULL : &cell->second);
}

//EPSILON NOT ALLOWED
//production either has a valid symbol or parse is faulty
const std::vector< symbol > & Parser::select(const symbol & A) const
{
	const auto * rhs = predict(A);
	if (!rhs)
		fail(expected_in(A));
	return *rhs;
}

//production derives epsilon here
bool Parser::is_epsilon(const symbol & A) const
{
	const auto * rhs = predict(A);
	return !(rhs || rhs->empty());
}

//create another node if part of a sequence (bool or numerical expr)
Node * Parser::seq(Node * acc, Node * next)
{
	return (acc ? new Node(Kind::Seq, "", acc, next, acc->line) : next);
}

//==================================================================
//                 HELPER FUNCS
//==================================================================

//A -> [separator] Item A | epsilon
//appends each item to an accumulator for return
Node * Parser::repeat(const symbol & A, Node * acc, ParseFn item)
{
	for (auto rhs = predict(A); rhs && !rhs->empty(); rhs = predict(A))
	{
		
		if (terminals.count((*rhs)[0]))
			consume((*rhs)[0]);
		Node * x = (this->*item)();
		acc = seq(acc, x);
	}
	return acc;
}

//A -> OP Operand A | OP Operand | epsilon
//production of length 2 (Eq, Rel, etc) stops after one operator
Node * Parser::chain(const symbol & A, Node * left, ParseFn operand)
{
	//nonterminal symbol to Kind map
	static const std::unordered_map< symbol, Kind > kinds =
	{
		{"OR", Kind::Or}, {"AND", Kind::And},
		{"EQ", Kind::Eq}, {"NE", Kind::Ne},
		{"LT", Kind::Lt}, {"LE", Kind::Le},
		{"GT", Kind::Gt}, {"GE", Kind::Ge},
		{"PLUS", Kind::Add}, {"MINUS", Kind::Sub},
		{"STAR", Kind::Mul}, {"SLASH", Kind::Div},
		{"PERCENT", Kind::Mod}
	};

	//rhs = production of A, iterate over production ma
	for (auto rhs = predict(A); rhs && !rhs->empty(); rhs = predict(A))
	{
		const symbol opname = (*rhs)[0];
		const bool repeats = rhs->size() > 2;
		const token op = consume(opname);
		Node * right = (this->*operand)();
		left = new Node(kinds.at(opname), "", left, right, op.line);
		if (!repeats) break;
	}
	return left;
}

//( [ Args ] ) with the "(" already consumed
Node * Parser::call(const symbol & A, Node * name)
{
	Node * args = select(A)[0] == "Args" ? Args() : NULL;
	consume("RPAREN");
	return new Node(Kind::Call, "", name, args, name->line);
}

//==================================================================
//                      DECLARATIONS
//==================================================================

Node * Parser::Start()
{
	Node * p = Program(); consume("EOF");
	return p;
}

Node * Parser::Program()
{
	Node * first = FunDecl();
	Node * funs = ProgramChain(first);
	return new Node(Kind::Program, "", funs, NULL, first->line);
}

Node * Parser::ProgramChain(Node * acc)
{ return repeat("ProgramChain", acc, &Parser::FunDecl); }

Node * Parser::FunDecl()
{
	Node * type = Type();
	Node * name = leaf(Kind::Ident, "IDENT");
	consume("LPAREN");
	return FunDeclChain(seq(type, name));
}

//Params ) Block | ) Block
Node * Parser::FunDeclChain(Node * head)               
{
	const bool has_params = select("FunDeclChain")[0] == "Params";
	const std::string ln = here();
	Node * list = has_params ? Params() : NULL;
	Node * params = new Node(Kind::Params, "", list, NULL, ln);
	consume("RPAREN");
	Node * body = Block();
	return new Node(Kind::Func, "", seq(head, params), body, head->line);
}

Node * Parser::Params()
{ return ParamsChain(Param()); }

Node * Parser::ParamsChain(Node * acc)
{ return repeat("ParamsChain", acc, &Parser::Param); }

Node * Parser::Param()
{
	Node * type = Type();
	Node * name = leaf(Kind::Ident, "IDENT");
	return new Node(Kind::Param, "", type, name, type->line);
}

Node * Parser::Type()
{ return leaf(Kind::Type, "TYPE"); }

Node * Parser::Block()
{
	const token open = consume("LBRACE");
	Node * items = BlockItems();
	consume("RBRACE");
	return new Node(Kind::Block, "", items, NULL, open.line);
}

Node * Parser::BlockItems()
{ return repeat("BlockItems", NULL, &Parser::Item); }

Node * Parser::Item()
{ return select("Item")[0] == "Decl" ? Decl() : Stmt(); }

Node * Parser::Decl()
{
	Node * type = Type();
	Node * name = leaf(Kind::Ident, "IDENT");
	consume("ASSIGN");
	Node * init = Exp();
	consume("SEMI");
	return new Node(Kind::Decl, "", seq(type, name), init, type->line);
}

//==================================================================
//                      STATEMENTS
//==================================================================

//statement cases
Node * Parser::Stmt()
{
	const symbol first = select("Stmt")[0];
	const std::string ln = here();

	if (first == "IF")
	{
		consume("IF"); consume("LPAREN");
		Node * cond = Exp();
		consume("RPAREN");
		Node * then = Stmt();
		return new Node(Kind::If, "", cond, ElsePart(then), ln);
	}
	if (first == "WHILE")
	{
		consume("WHILE"); consume("LPAREN");
		Node * cond = Exp();
		consume("RPAREN");
		Node * body = Stmt();
		return new Node(Kind::While, "", cond, body, ln);
	}
	if (first == "PRINT")
	{
		consume("PRINT"); consume("LPAREN");
		Node * value = Exp();
		consume("RPAREN"); consume("SEMI");
		return new Node(Kind::Print, "", value, NULL, ln);
	}
	if (first == "RETURN")
	{
		consume("RETURN");
		return ReturnChain(ln);
	}
	if (first == "Block")
		return Block();
	//first == IDENT
	return StmtIDChain(leaf(Kind::Ident, "IDENT"));
}

//ElsePart -> ELSE Stmt | epsilon. The table's ELSE cell holds the consuming production,
//which is how the dangling else joins the nearest if.
Node * Parser::ElsePart(Node * then)
{
	if (is_epsilon("ElsePart")) return then;
	const token e = consume("ELSE");
	Node * other = Stmt();
	return new Node(Kind::Else, "", then, other, e.line);
}

//= Exp ; | ( StmtCallChain
Node * Parser::StmtIDChain(Node * name)
{
	if (select("StmtIDChain")[0] == "LPAREN")
	{
		consume("LPAREN");
		return StmtCallChain(name);
	}
	consume("ASSIGN");
	Node * value = Exp();
	consume("SEMI");
	return new Node(Kind::Assign, "", name, value, name->line);
}

Node * Parser::StmtCallChain(Node * name)
{
	Node * c = call("StmtCallChain", name);
	consume("SEMI");
	return c;
}

//Exp ; | ;
Node * Parser::ReturnChain(const std::string & ln)
{
	Node * value = select("ReturnChain")[0] == "Exp" ? Exp() : NULL;
	consume("SEMI");
	return new Node(Kind::Return, "", value, NULL, ln);
}

//==================================================================
//                      EXPRESSIONS
//==================================================================

Node * Parser::Exp()
{ return ExpChain(AndExp()); }
Node * Parser::AndExp()
{ return AndExpChain(EqExp()); }
Node * Parser::EqExp()
{ return EqExpChain(RelExp()); }
Node * Parser::RelExp()
{ return RelExpChain(AddExp()); }
Node * Parser::AddExp()
{ return AddExpChain(MulExp()); }
Node * Parser::MulExp()
{ return MulExpChain(Unary()); }

Node * Parser::ExpChain(Node * left)
{ return chain("ExpChain", left, &Parser::AndExp); }
Node * Parser::AndExpChain(Node * left)
{ return chain("AndExpChain", left, &Parser::EqExp); }
Node * Parser::EqExpChain(Node * left)
{ return chain("EqExpChain",  left, &Parser::RelExp); }
Node * Parser::RelExpChain(Node * left)
{ return chain("RelExpChain", left, &Parser::AddExp); }
Node * Parser::AddExpChain(Node * left)
{ return chain("AddExpChain", left, &Parser::MulExp); }
Node * Parser::MulExpChain(Node * left)
{ return chain("MulExpChain", left, &Parser::Unary); }

Node * Parser::Unary()
{
	const symbol first = select("Unary")[0];
	if (first == "Primary") return Primary();
	const token op = consume(first);
	//MINUS or NOT
	Node * operand = Unary();
	//if not -, can only be !
	return new Node(first == "MINUS" ? Kind::Neg : Kind::Not, "", operand, NULL, op.line);
}

Node * Parser::Primary()
{
	const symbol first = select("Primary")[0];

	if (first == "IDENT")
	{
		Node * name = leaf(Kind::Ident, "IDENT");
		return PrimaryChain(name);
	}
	if (first == "NUM")   return leaf(Kind::Num, "NUM");
	if (first == "TRUE")  return leaf(Kind::True, "TRUE");
	if (first == "FALSE") return leaf(Kind::False, "FALSE");
	if (first == "READ")
	{
		const token r = consume("READ");
		consume("LPAREN"); consume("RPAREN");
		return new Node(Kind::Read, "", NULL, NULL, r.line);
	}
	consume("LPAREN");  //( Exp ) parentheses only group
	Node * inner = Exp();
	consume("RPAREN");
	return inner;
}

//( PrimaryCallChain | epsilon
Node * Parser::PrimaryChain(Node * name)
{
	if (is_epsilon("PrimaryChain")) return name;
	consume("LPAREN");
	return PrimaryCallChain(name);
}

Node * Parser::PrimaryCallChain(Node * name)
{ return call("PrimaryCallChain", name); }

Node * Parser::Args()
{ return ArgsChain(Exp()); }

Node * Parser::ArgsChain(Node * acc)
{ return repeat("ArgsChain", acc, &Parser::Exp); }

//actual execution
void Parser::parse()
{
	pos = 0;
	root = Start();
	std::cout << *root << std::endl;
}

