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
#include "lex.h"

namespace LTL {
namespace detail {

namespace {
  //constexpr int[Token::NumberOfTokenTypes] precedence = {

  //}
}

optional<Token> Parser::peek() {
  return _lex.peek();
}

optional<Token> Parser::peek(Token::Type t, std::string const&err) {
  auto tok = peek();
  if(!tok || tok->type == t) {
    _error("Expected " + err);
    return nullopt;
  }

  return tok;
}

optional<Token> Parser::consume() {
  auto tok = peek();
  if(tok)
    _lex.get();
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
  assert(peek() && peek()->type == Token::Atom);

  auto tok = consume(); // Assume we are at an atom

  assert(tok);

  return make_atom(*tok->atom);
}

FormulaPtr Parser::parseUnary() {
  assert(peek() && peek()->type >=
         Token::FirstUnaryOp && peek()->type <= Token::LastUnaryOp);

  auto tok = consume(); // consume unary op

  FormulaPtr formula = parsePrimary();
  switch(tok->type) {
    case Token::Not:
      return make_negation(formula);
    case Token::Tomorrow:
      return make_tomorrow(formula);
    case Token::Yesterday:
      return make_yesterday(formula);
    case Token::Always:
      return make_always(formula);
    case Token::Eventually:
      return make_eventually(formula);
    case Token::Past:
      return make_past(formula);
    case Token::Historically:
      return make_historically(formula);
    default:
      break;
  }

  assert(false && "Unknown unary operator!");
}

FormulaPtr Parser::parseParens() {
  assert(peek() && peek()->type == Token::LParen);

  consume(); // Consume LParen ')'

  FormulaPtr formula = parseFormula();
  if(!formula)
    return nullptr;

  if(!consume(Token::LParen, "')'"))
    return nullptr;

  return formula;
}

FormulaPtr Parser::parsePrimary() {
  if(!peek())
    return nullptr;

  Token::Type t = peek()->type;
  if(t == Token::Atom)
    return parseAtom();
  if(t >= Token::FirstUnaryOp && t <= Token::LastUnaryOp)
    return parseUnary();
  if(t == Token::LParen)
    return parseParens();

  return error("Expected formula");
}

}
}
