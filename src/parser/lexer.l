%%

[ \t\n]+
"("         return Parser::LPAR;
")"         return Parser::RPAR;
"!" 		return Parser::NOT;
"NOT" 		return Parser::NOT;
"~"		return Parser::NOT;
"X" 		return Parser::TOMORROW;
"G" 		return Parser::ALWAYS;
"[]" 		return Parser::ALWAYS;
"F" 		return Parser::EVENTUALLY;
"<>" 		return Parser::EVENTUALLY;
"&&"		return Parser::CONJUNCTION;
"&"         return Parser::CONJUNCTION;
"AND"		return Parser::CONJUNCTION;
"||" 		return Parser::DISJUNCTION;
"|"         return Parser::DISJUNCTION;
"OR"		return Parser::DISJUNCTION;
"->" 		return Parser::THEN;
"THEN"		return Parser::THEN;
"<->"		return Parser::IFF;
"IFF"		return Parser::IFF;
"U"         return Parser::UNTIL;
"V"         return Parser::RELEASE;
"R" 		return Parser::RELEASE;
"TRUE"		return Parser::TRUE;
"true"		return Parser::TRUE;
"FALSE"     return Parser::FALSE;
"false"		return Parser::FALSE;
[a-z0-9:]+ 	return Parser::ATOM;

%%