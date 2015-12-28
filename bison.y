
%{
// preamble
%}

%token  
   VARASSIGN INPUT OUTPUT NAME  
   IF TRUE FALSE LESS_EQUAL GREATER_EQUAL 
   EQUAL_EQUAL NOT_EQUAL  FLOAT INTEGER
   MIXER

%%

Mixer :
   Stmts  MixerImpl
;

Stmts:
   Stmt
  | Stmts Stmt
;

Stmt:
   NAME '=' Expr ';'
 | OUTPUT '[' Expr ']' VARASSIGN  Expr ';'
;

Expr:
   OrExpr
;

Primary :
   Literal
|  NameOrFunction
|  IOExpr
|  IF '(' Expr ',' Expr ',' Expr ')'
|  '+' Primary
|  '-' Primary
|  '!' Primary

|  '(' Expr ')'
;

IOExpr:
   INPUT '[' Expr ']'
|  OUTPUT '[' Expr ']'
;

NameOrFunction:
   NAME
|  NAME '(' ExprList ')'
;

MulExpr:
   Primary
|  MulExpr '*' Primary
|  MulExpr '/' Primary
;

AddExpr:
   MulExpr
|  AddExpr '+' MulExpr
|  AddExpr '-' MulExpr
;

RelationalExpr:
   AddExpr
|  RelationalExpr RelationalOp AddExpr
;

AndExpr:
   RelationalExpr
|  AndExpr '&' RelationalExpr

OrExpr:
   AndExpr
|  OrExpr '|' AndExpr
;

Literal : 
    NumericLiteral
 |  BoolLiteral
;

NumericLiteral:
     FLOAT
 |  INTEGER
;

BoolLiteral:
   TRUE
|   FALSE
;

RelationalOp:
   '>'
|   '<'
|  LESS_EQUAL 
|  GREATER_EQUAL 
|  EQUAL_EQUAL 
|  NOT_EQUAL
;

ExprList :
   Expr
|  ExprList ',' Expr
;

MixerImpl :
   MIXER '{' Stmts '}'
;

%%

