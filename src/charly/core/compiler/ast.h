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

#include <functional>
#include <iostream>
#include <sstream>
#include <memory>

#include "charly/utils/map.h"
#include "charly/utils/string.h"
#include "charly/utils/vector.h"

#include "charly/core/compiler/location.h"
#include "charly/core/compiler/token.h"

#pragma once

namespace charly::core::compiler::ast {

template <typename T>
using ref = std::shared_ptr<T>;

template <typename T, typename... Args>
inline ref<T> make(Args... params) {
  return std::make_shared<T>(std::forward<Args>(params)...);
}

template <typename T, typename O>
inline ref<T> cast(ref<O> node) {
  return std::dynamic_pointer_cast<T>(node);
}

class ASTPass; // forward declaration

// base class of all ast nodes
class Node : std::enable_shared_from_this<Node> {
  friend class ASTPass;

public:
  enum class Type : uint8_t {
    Unknown = 0,
    Program = 1,
    Block,
    Id,
    Int,
    Float,
    Bool,
    String,
    FormatString,
    Null,
    Self,
    Super,
    Tuple,
    TypeCount
  };

  static constexpr const char* TypeNames[] = {
    "Unknown",
    "Program",
    "Block",
    "Id",
    "Int",
    "Float",
    "Bool",
    "String",
    "FormatString",
    "Null",
    "Self",
    "Super",
    "Tuple",
    "__SENTINEL",
  };

  const LocationRange& location() const {
    return m_location;
  }

  const Location& begin() const {
    return m_location.begin;
  }

  const Location& end() const {
    return m_location.end;
  }

  void set_location(const Location& loc) {
    set_begin(loc);
    set_end(loc);
  }

  void set_location(const LocationRange& range) {
    m_location = range;
  }

  void set_location(const ref<Node>& node) {
    m_location = node->location();
  }

  void set_begin(const Location& loc) {
    m_location.begin = loc;
  }

  void set_begin(const LocationRange& range) {
    m_location.begin = range.begin;
  }

  void set_begin(const ref<Node>& node) {
    set_begin(node->begin());
  }

  void set_end(const Location& loc) {
    m_location.end = loc;
  }

  void set_end(const LocationRange& range) {
    m_location.end = range.end;
  }

  void set_end(const ref<Node>& node) {
    set_end(node->end());
  }

  // get the name of this node based on its type
  const char* name() const {
    return TypeNames[static_cast<int>(this->type())];
  }

  virtual Type type() const = 0;

protected:
  virtual ~Node() {};
  template <typename T>
  ref<T> visit(ASTPass* pass, const ref<T>& node);
  virtual void visit_children(ASTPass*) {}

  LocationRange m_location;
};

template <typename T>
bool isa(const ref<Node>& node) {
  return dynamic_cast<T*>(node.get()) != nullptr;
}

#define AST_NODE(T)                          \
  virtual Node::Type type() const override { \
    return Node::Type::T;                    \
  }

class Statement : public Node {};

class Block final : public Statement {
  AST_NODE(Block)
public:
  utils::vector<ref<Statement>> statements;

  virtual void visit_children(ASTPass*) override;
};

class Program final : public Node {
  AST_NODE(Program)
public:
  Program(const utils::string& filename, ref<Block> block = nullptr) : filename(filename), block(block) {}

  utils::string filename;
  ref<Block> block;

  virtual void visit_children(ASTPass*) override;
};

class Expression : public Statement {};

class ConstantExpression : public Expression {};

class Null final : public ConstantExpression {
  AST_NODE(Null)
};

class Self final : public ConstantExpression {
  AST_NODE(Self)
};

class Super final : public ConstantExpression {
  AST_NODE(Super)
};

template <typename T>
class Atom : public ConstantExpression {
public:
  Atom(T value) : value(value) {}
  T value;
};

class Id final : public Atom<utils::string> {
  AST_NODE(Id)
public:
  using Atom<utils::string>::Atom;
};

class Int final : public Atom<int64_t> {
  AST_NODE(Int)
public:
  using Atom<int64_t>::Atom;
};

class Float final : public Atom<double> {
  AST_NODE(Float)
public:
  using Atom<double>::Atom;
};

class Bool final : public Atom<bool> {
  AST_NODE(Bool)
public:
  using Atom<bool>::Atom;
};

class String final : public Atom<utils::string> {
  AST_NODE(String)
public:
  using Atom<utils::string>::Atom;
};

class FormatString final : public Expression {
  AST_NODE(FormatString)
public:
  utils::vector<ref<Expression>> elements;

  virtual void visit_children(ASTPass*) override;
};

class Tuple final : public Expression {
  AST_NODE(Tuple)
public:
  utils::vector<ref<Expression>> elements;

  virtual void visit_children(ASTPass*) override;
};

#undef AST_NODE

}  // namespace charly::core::compiler::ast