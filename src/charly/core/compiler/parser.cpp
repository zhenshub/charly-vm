/*
 * This file is part of the Charly Virtual Machine (https://github.com/KCreate/charly-vm)
 *
 * MIT License
 *
 * Copyright (c) 2017 - 2021 Leonard Schütz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "charly/core/compiler/parser.h"

namespace charly::core::compiler {

using namespace ast;

ref<Program> Parser::parse_program() {
  return make<Program>(m_filename, parse_block_body());
}

ref<Block> Parser::parse_block() {
  eat(TokenType::LeftCurly);
  ref<Block> block = parse_block_body();
  eat(TokenType::RightCurly);
  return block;
}

ref<Block> Parser::parse_block_body() {
  ref<Block> block = make<Block>();

  at(block);

  while (!(type(TokenType::RightCurly) || type(TokenType::Eof))) {
    ref<Statement> stmt = parse_statement();
    block->statements.push_back(stmt);
    block->end(stmt);
  }

  return block;
}

ref<Statement> Parser::parse_statement() {
  switch (m_token.type) {
    default: {
      ref<Expression> exp = parse_expression();
      skip(TokenType::Semicolon);
      return exp;
    }
  }
}

ref<Expression> Parser::parse_comma_expression() {
  ref<Expression> exp = parse_expression();
  if (!type(TokenType::Comma))
    return exp;

  ref<Tuple> tuple = make<Tuple>();
  tuple->begin(exp);
  tuple->add_element(exp);

  while (type(TokenType::Comma)) {
    eat(TokenType::Comma);
    tuple->add_element(parse_expression());
  }

  return tuple;
}

ref<Expression> Parser::parse_expression() {
  return parse_tuple();
}

ref<Expression> Parser::parse_tuple() {
  if (!type(TokenType::LeftParen)) {
    return parse_literal();
  }

  ref<Tuple> tuple = make<Tuple>();
  begin(tuple);

  eat(TokenType::LeftParen);

  bool force_tuple = false;

  while (!(type(TokenType::RightParen))) {
    tuple->add_element(parse_expression());

    // (<exp>,) becomes a tuple with one element
    if (skip(TokenType::Comma) && type(TokenType::RightParen)) {
      if (tuple->elements.size() == 1) {
        force_tuple = true;
        break;
      }
    }
  }

  end(tuple);
  eat(TokenType::RightParen);

  // (<exp>) becomes just <exp>
  if (!force_tuple && tuple->elements.size() == 1) {
    return tuple->elements.at(0);
  }

  return tuple;
}

ref<Expression> Parser::parse_literal() {
  switch (m_token.type) {
    case TokenType::Int: {
      return parse_int_token();
    }
    case TokenType::Float: {
      return parse_float_token();
    }
    case TokenType::True:
    case TokenType::False: {
      return parse_bool_token();
    }
    case TokenType::Identifier: {
      return parse_identifier_token();
    }
    case TokenType::String: {
      return parse_string_token();
    }
    case TokenType::FormatString: {
      return parse_format_string();
    }
    case TokenType::Null: {
      return parse_null_token();
    }
    case TokenType::Self: {
      return parse_self_token();
    }
    case TokenType::Super: {
      return parse_super_token();
    }
    default: {
      unexpected_token("literal");
    }
  }
}

ref<FormatString> Parser::parse_format_string() {
  ref<FormatString> str = make<FormatString>();
  begin(str);

  str->add_part(parse_string_token());

  for (;;) {
    str->add_part(parse_expression());
    eat(TokenType::RightCurly);

    // if the expression is followed by another FormatString token the loop
    // repeats and we parse another interpolated expression
    //
    // the format string is only terminated once a regular String token is passed
    if (type(TokenType::FormatString) || type(TokenType::String)) {
      bool last_part = type(TokenType::String);
      str->add_part(parse_string_token());
      if (last_part)
        break;
    } else {
      unexpected_token(TokenType::String);
    }
  }

  return str;
}

ref<Int> Parser::parse_int_token() {
  match(TokenType::Int);
  ref<Int> node = make<Int>(m_token.intval);
  node->at(m_token.location);
  advance();
  return node;
}

ref<Float> Parser::parse_float_token() {
  match(TokenType::Float);
  ref<Float> node = make<Float>(m_token.floatval);
  node->at(m_token.location);
  advance();
  return node;
}

ref<Boolean> Parser::parse_bool_token() {
  if (type(TokenType::True) || type(TokenType::False)) {
    ref<Boolean> node = make<Boolean>(type(TokenType::True));
    node->at(m_token.location);
    advance();
    return node;
  } else {
    unexpected_token("true or false");
  }
}

ref<Identifier> Parser::parse_identifier_token() {
  match(TokenType::Identifier);
  ref<Identifier> node = make<Identifier>(m_token.source);
  node->at(m_token.location);
  advance();
  return node;
}

ref<String> Parser::parse_string_token() {
  if (type(TokenType::String) || type(TokenType::FormatString)) {
    ref<String> node = make<String>(m_token.source);
    node->at(m_token.location);
    advance();
    return node;
  } else {
    unexpected_token(TokenType::String);
  }
}

ref<Null> Parser::parse_null_token() {
  match(TokenType::Null);
  ref<Null> node = make<Null>();
  node->at(m_token.location);
  advance();
  return node;
}

ref<Self> Parser::parse_self_token() {
  match(TokenType::Self);
  ref<Self> node = make<Self>();
  node->at(m_token.location);
  advance();
  return node;
}

ref<Super> Parser::parse_super_token() {
  match(TokenType::Super);
  ref<Super> node = make<Super>();
  node->at(m_token.location);
  advance();
  return node;
}

}  // namespace charly::core::compiler
