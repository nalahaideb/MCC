****************************  LL(1) argument  *******************
[] why is it LL(1)
[] show it is LL(1) except for else
[] explain how i resolved LL(1) else conflict

The grammar has unambiguous productions, is left-factored removing
left recursion, and the token tape is read left to right

FIRST(A) intersect FOLLOW(B) is the null set for all elements of
the grammar where A and B are alternate productions of C and that
B can derive epsilon

ElsePart and else are both resolutions when 'else' is evaluated,
so a priority rule that states "when the next token is else,
choose a production to consume it by joining an if "
****************************  GRAMMAR  **************************

//artificial start state production
{"START", {"Program", "EOF"}},
	  
//Program ::= FunDecl { FunDecl }
{"Program", {"FunDecl", "ProgramChain"}},
{"ProgramChain", {"FunDecl", "ProgramChain"}},
{"ProgramChain", {}}, //epsilon

//FunDecl ::= Type ID "(" [ Params ] ")" Block
{"FunDecl", {"Type", "IDENT", "LPAREN", "FunDeclChain"}},
{"FunDeclChain", {"Params", "RPAREN", "Block"}},
{"FunDeclChain", {"RPAREN", "Block"}},

//Params ::= Param { "," Param }
{"Params", {"Param", "ParamsChain"}},
{"ParamsChain", {"COMMA", "Param", "ParamsChain"}},
{"ParamsChain", {}}, //epsilon

//Param ::= Param ::= Type ID
{"Param", {"Type", "IDENT"}},

//Type ::= "int" | "bool"
{"Type", {"TYPE"}},

//Block ::= "{" { Item } "}"
{"Block", {"LBRACE", "BlockItems", "RBRACE"}},
{"BlockItems", {"Item", "BlockItems"}},
{"BlockItems", {}}, //epsilon

//Item ::= Decl | Stmt
{"Item", {"Decl"}},
{"Item", {"Stmt"}},

//Decl ::= Type ID "=" Exp ";"
{"Decl", {"Type", "IDENT", "ASSIGN", "Exp", "SEMI"}},

//Stmt
{"Stmt", {"IF", "LPAREN", "Exp", "RPAREN", "Stmt", "ElsePart"}},
{"Stmt", {"WHILE", "LPAREN", "Exp", "RPAREN", "Stmt"}},
{"Stmt", {"PRINT", "LPAREN", "Exp", "RPAREN", "SEMI"}},
{"Stmt", {"RETURN", "ReturnChain"}},
{"Stmt", {"Block"}},
{"Stmt", {"IDENT", "StmtIDChain"}},
{"ElsePart", {"ELSE", "Stmt"}},
{"ElsePart", {}}, //epsilon

{"StmtIDChain", {"ASSIGN", "Exp", "SEMI"}},
{"StmtIDChain", {"LPAREN", "StmtCallChain"}},
{"StmtCallChain", {"Args", "RPAREN", "SEMI"}},
{"StmtCallChain", {"RPAREN", "SEMI"}},

{"ReturnChain", {"Exp", "SEMI"}},
{"ReturnChain", {"SEMI"}},

//Exp ::= AndExp { "||" AndExp }
{"Exp",     {"AndExp", "ExpChain"}},
{"ExpChain", {"OR", "AndExp", "ExpChain"}},
{"ExpChain", {}}, //epsilon

//AndExp ::= EqExp { "&&" EqExp }
{"AndExp",     {"EqExp", "AndExpChain"}},
{"AndExpChain", {"AND", "EqExp", "AndExpChain"}},
{"AndExpChain", {}}, //epsilon

//EqExp ::= RelExp [ ("==" | "!=") RelExp ]
{"EqExp",     {"RelExp", "EqExpChain"}},
{"EqExpChain", {"EQ", "RelExp"}},
{"EqExpChain", {"NE", "RelExp"}},
{"EqExpChain", {}}, //epsilon

//RelExp ::= AddExp [ ("<" | "<=" | ">" | ">=") AddExp ]
{"RelExp",     {"AddExp", "RelExpChain"}},
{"RelExpChain", {"LT", "AddExp"}},
{"RelExpChain", {"LE", "AddExp"}},
{"RelExpChain", {"GT", "AddExp"}},
{"RelExpChain", {"GE", "AddExp"}},
{"RelExpChain", {}}, //epsilon

//AddExp ::= MulExp { ("+" | "-") MulExp }
{"AddExp",     {"MulExp", "AddExpChain"}},
{"AddExpChain", {"PLUS", "MulExp", "AddExpChain"}},
{"AddExpChain", {"MINUS", "MulExp", "AddExpChain"}},
{"AddExpChain", {}}, //epsilon

//MulExp ::= Unary { ("*" | "/" | "%") Unary }
{"MulExp",     {"Unary", "MulExpChain"}},
{"MulExpChain", {"STAR", "Unary", "MulExpChain"}},
{"MulExpChain", {"SLASH", "Unary", "MulExpChain"}},
{"MulExpChain", {"PERCENT", "Unary", "MulExpChain"}},
{"MulExpChain", {}}, //epsilon

// Unary ::= ("-" | "!") Unary | Primary
{"Unary", {"MINUS", "Unary"}},
{"Unary", {"NOT", "Unary"}},
{"Unary", {"Primary"}},

//Primary
{"Primary", {"IDENT", "PrimaryChain"}},
{"Primary", {"READ", "LPAREN", "RPAREN"}},
{"Primary", {"LPAREN", "Exp", "RPAREN"}},
{"Primary", {"NUM"}},
{"Primary", {"TRUE"}},
{"Primary", {"FALSE"}},

{"PrimaryChain", {"LPAREN", "PrimaryCallChain"}},
{"PrimaryChain", {}}, //epsilon
{"PrimaryCallChain", {"Args", "RPAREN"}},
{"PrimaryCallChain", {"RPAREN"}},

//Args ::= Exp { "," Exp }
{"Args", {"Exp", "ArgsChain"}},

{"ArgsChain", {"COMMA", "Exp", "ArgsChain"}},
{"ArgsChain", {}} //epsilon


****************************  SETS  ************************

================BEGIN NULLABLE================
[ MulExpChain, AddExpChain, EqExpChain, RelExpChain, AndExpChain, ArgsChain, ExpChain, ElsePart, BlockItems, ParamsChain, PrimaryChain, ProgramChain ]
================END NULLABLE================
================BEGIN FIRST================
RelExp -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
Exp -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
ProgramChain -> TYPE, 
AddExp -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
EqExpChain -> NE, EQ, 
ArgsChain -> COMMA, 
AndExpChain -> AND, 
AddExpChain -> MINUS, PLUS, 
GT -> GT, 
LE -> LE, 
Stmt -> IDENT, LBRACE, RETURN, PRINT, WHILE, IF, 
Type -> TYPE, 
Item -> IF, WHILE, PRINT, RETURN, LBRACE, IDENT, TYPE, 
OR -> OR, 
Decl -> TYPE, 
GE -> GE, 
ELSE -> ELSE, 
EqExp -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
LBRACE -> LBRACE, 
START -> TYPE, 
MulExp -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
LPAREN -> LPAREN, 
RPAREN -> RPAREN, 
COMMA -> COMMA, 
TRUE -> TRUE, 
FunDeclChain -> TYPE, RPAREN, 
READ -> READ, 
StmtCallChain -> IDENT, READ, LPAREN, TRUE, FALSE, NOT, MINUS, NUM, RPAREN, 
FALSE -> FALSE, 
WHILE -> WHILE, 
PRINT -> PRINT, 
EOF -> EOF, 
ASSIGN -> ASSIGN, 
PLUS -> PLUS, 
RETURN -> RETURN, 
NUM -> NUM, 
IF -> IF, 
RBRACE -> RBRACE, 
NE -> NE, 
SLASH -> SLASH, 
SEMI -> SEMI, 
Primary -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, 
EQ -> EQ, 
PERCENT -> PERCENT, 
MINUS -> MINUS, 
NOT -> NOT, 
ExpChain -> OR, 
STAR -> STAR, 
MulExpChain -> PERCENT, SLASH, STAR, 
LT -> LT, 
ParamsChain -> COMMA, 
Args -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
PrimaryCallChain -> IDENT, READ, LPAREN, TRUE, FALSE, NOT, MINUS, NUM, RPAREN, 
PrimaryChain -> LPAREN, 
Program -> TYPE, 
AND -> AND, 
Params -> TYPE, 
Unary -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
StmtIDChain -> LPAREN, ASSIGN, 
TYPE -> TYPE, 
AndExp -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
BlockItems -> TYPE, IDENT, LBRACE, RETURN, PRINT, WHILE, IF, 
ReturnChain -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, SEMI, 
RelExpChain -> GE, GT, LE, LT, 
FunDecl -> TYPE, 
IDENT -> IDENT, 
ElsePart -> ELSE, 
Param -> TYPE, 
Block -> LBRACE, 

================END FIRST================
================BEGIN FOLLOW================
RelExp -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
Exp -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
ProgramChain -> TYPE, 
AddExp -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
EqExpChain -> NE, EQ, 
ArgsChain -> COMMA, 
AndExpChain -> AND, 
AddExpChain -> MINUS, PLUS, 
GT -> GT, 
LE -> LE, 
Stmt -> IDENT, LBRACE, RETURN, PRINT, WHILE, IF, 
Type -> TYPE, 
Item -> IF, WHILE, PRINT, RETURN, LBRACE, IDENT, TYPE, 
OR -> OR, 
Decl -> TYPE, 
GE -> GE, 
ELSE -> ELSE, 
EqExp -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
LBRACE -> LBRACE, 
START -> TYPE, 
MulExp -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
LPAREN -> LPAREN, 
RPAREN -> RPAREN, 
COMMA -> COMMA, 
TRUE -> TRUE, 
FunDeclChain -> TYPE, RPAREN, 
READ -> READ, 
StmtCallChain -> IDENT, READ, LPAREN, TRUE, FALSE, NOT, MINUS, NUM, RPAREN, 
FALSE -> FALSE, 
WHILE -> WHILE, 
PRINT -> PRINT, 
EOF -> EOF, 
ASSIGN -> ASSIGN, 
PLUS -> PLUS, 
RETURN -> RETURN, 
NUM -> NUM, 
IF -> IF, 
RBRACE -> RBRACE, 
NE -> NE, 
SLASH -> SLASH, 
SEMI -> SEMI, 
Primary -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, 
EQ -> EQ, 
PERCENT -> PERCENT, 
MINUS -> MINUS, 
NOT -> NOT, 
ExpChain -> OR, 
STAR -> STAR, 
MulExpChain -> PERCENT, SLASH, STAR, 
LT -> LT, 
ParamsChain -> COMMA, 
Args -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
PrimaryCallChain -> IDENT, READ, LPAREN, TRUE, FALSE, NOT, MINUS, NUM, RPAREN, 
PrimaryChain -> LPAREN, 
Program -> TYPE, 
AND -> AND, 
Params -> TYPE, 
Unary -> IDENT, READ, LPAREN, TRUE, NUM, FALSE, NOT, MINUS, 
StmtIDChain -> LPAREN, ASSIGN, 
TYPE -> TYPE, 
AndExp -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, 
BlockItems -> TYPE, IDENT, LBRACE, RETURN, PRINT, WHILE, IF, 
ReturnChain -> FALSE, NUM, TRUE, LPAREN, READ, IDENT, MINUS, NOT, SEMI, 
RelExpChain -> GE, GT, LE, LT, 
FunDecl -> TYPE, 
IDENT -> IDENT, 
ElsePart -> ELSE, 
Param -> TYPE, 
Block -> LBRACE, 
