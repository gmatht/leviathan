/*
 Copyright (c) 2014, Nicola Gigante
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * The names of its contributors may not be used to endorse or promote
 products derived from this software without specific prior written
 permission.
 */

#include "parser.h"


namespace LTL {
namespace detail {

optional<Token> Parser::peek(Token::Type t, std::string const&err) {
  auto tok = _lex.peek();
  if(!tok || tok->type != t) {
    _error("Expected " + err);
    return nullopt;
  }

  return tok;
}

optional<Token> Parser::consume(Token::Type t, std::string const&err) {
  auto tok = peek(t, err);
  if(tok)
    _lex.get();
  return tok;
}

FormulaPtr Parser::error(std::string const&s) {
  _error(s);
  return nullptr;
}


FormulaPtr Parser::parseAtom() {
  auto tok = consume(Token::Atom, "atom");

  if(!tok)
    return nullptr;

  return make_atom(*tok->atom);
}

FormulaPtr Parser::parseParens() {
  if(!consume(Token::LParen, "'('"))
    return nullptr;

  FormulaPtr formula = parseFormula();
  if(!formula)
    return nullptr;

  if(!consume(Token::LParen, "')'"))
    return nullptr;

  return formula;
}

FormulaPtr Parser::parsePrimary() {
  if(!_lex.peek())
    return nullptr;

  switch(_lex.peek()->type) {
    case Token::Atom:
      return parseAtom();
    case Token::LParen:
      return parseParens();
    default:
      return error("Expected formula");
  }
}

}
}
