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


#ifndef PARSER_H_
#define PARSER_H_

#include "formula.hpp"
#include "lex.h"

#include <istream>
#include <functional>

namespace LTL {
namespace detail {

class Parser
{

  Parser(std::istream &stream, std::function<void(std::string)> error)
    : _lex(stream), _error(error)
  {
    _lex.get();
  }

  FormulaPtr parseFormula();

private:
  optional<Token> peek();
  optional<Token> consume();
  optional<Token> peek(Token::Type, std::string const&err);
  optional<Token> consume(Token::Type, std::string const&err);
  FormulaPtr error(std::string const&s);


  FormulaPtr parseAtom();
  FormulaPtr parseUnary();
  FormulaPtr parseParens();
  FormulaPtr parsePrimary();


private:
  Lexer _lex;
  std::function<void(std::string)> _error;
};


} // namespace detail
} // namespace LTL

#endif // PARSER_H_
