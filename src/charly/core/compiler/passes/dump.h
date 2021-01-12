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

#include "charly/core/compiler/astpass.h"
#include "charly/utils/colorwriter.h"

#pragma once

namespace charly::core::compiler::ast {

class DumpPass : public ASTPass {
  virtual void before_enter_any(const ref<Node>& node) override {
    for (uint16_t i = 0; i < m_depth; i++) {
      m_writer.write("  ");
    }

    m_writer.write("- ");
    m_writer.blue(node->name());
  }

  virtual void after_enter_any(const ref<Node>& node) override {
    m_writer.write(" ");
    m_writer.grey(node->location());
    m_writer.write('\n');
    m_depth++;
  }

  virtual void before_leave_any(const ref<Node>&) override {
    m_depth--;
  }

  virtual bool enter(const ref<Program>& node) override {
    m_writer.write(' ');
    m_writer.yellow("filename");
    m_writer.white(" = ");
    m_writer.yellow(node->filename);
    return true;
  }

  virtual bool enter(const ref<Id>& node) override {
    m_writer.write(' ');
    m_writer.yellow(node->value);
    return true;
  }

  virtual bool enter(const ref<Int>& node) override {
    m_writer.write(' ');
    m_writer.red(node->value);
    return true;
  }

  virtual bool enter(const ref<Float>& node) override {
    m_writer.write(' ');
    m_writer.red(node->value);
    return true;
  }

  virtual bool enter(const ref<Bool>& node) override {
    m_writer.write(' ');
    m_writer.red(node->value ? "true" : "false");
    return true;
  }

  virtual bool enter(const ref<String>& node) override {
    m_writer.write(' ');
    m_writer.yellow('\"');
    m_writer.yellow(node->value);
    m_writer.yellow('\"');
    return true;
  }

public:
  DumpPass(std::ostream& stream = std::cout) : m_writer(stream), m_depth(0) {}

  utils::ColorWriter m_writer;
  uint16_t m_depth;
};

}  // namespace charly::core::compiler::ast