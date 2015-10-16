%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%baseclass-preinclude leviathan.hpp
%start root

%polymorphic ATOM: std::string; FORMULA: LTL::FormulaPtr;

%token ATOM
%token TRUE
%token FALSE
%token LPAR
%token RPAR
%left CONJUNCTION
%left DISJUNCTION
%left THEN
%left IFF
%left UNTIL
%left RELEASE
%right NOT
%right TOMORROW
%right ALWAYS
%right EVENTUALLY

%type <FORMULA> ltl
%type <ATOM> atom
%type <FORMULA> root

%%
root: ltl {
		$$ = $1;
		_result = $1;
	}
;

ltl:	TRUE			{ $$ = LTL::make_true(); } |
	FALSE 			{ $$ = LTL::make_false(); } |
	LPAR ltl RPAR 	{ $$ = $2; } |
	NOT ltl 		{ $$ = LTL::make_negation($2); } |
	TOMORROW ltl 	{ $$ = LTL::make_tomorrow($2); } |
	ALWAYS ltl 		{ $$ = LTL::make_always($2); } |
	EVENTUALLY ltl 	{ $$ = LTL::make_eventually($2); } |
	ltl CONJUNCTION ltl 	{ $$ = LTL::make_conjunction($1, $3); } |
	ltl DISJUNCTION ltl 	{ $$ = LTL::make_disjunction($1, $3); } |
	ltl THEN ltl 		{ $$ = LTL::make_then($1, $3); } |
	ltl IFF ltl 		{ $$ = LTL::make_iff($1, $3); } |
	ltl UNTIL ltl 		{ $$ = LTL::make_until($1, $3); } |
	ltl RELEASE ltl 	{ $$ = LTL::make_negation(LTL::make_until(LTL::make_negation($1), LTL::make_negation($3))); } |
	atom 			{ $$ = LTL::make_atom($1); }
;

atom:	ATOM
	{
		$$ = std::string(d_scanner.matched());
	};
