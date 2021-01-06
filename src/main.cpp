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

#include <iostream>

#include "charly/core/compiler.h"
#include "charly/utils/buffer.h"

using namespace charly;
using namespace charly::core::compiler;

int main() {
  utils::string line;
  for (;;) {
    std::cout << "> ";

    if (!std::getline(std::cin, line))
      break;

    if (line.compare(".exit") == 0)
      break;

    {
      utils::Buffer buf;
      buf.append_string(line);

      Parser parser("stdin", buf.buffer_string());

      try {
        ref<Program> program = parser.parse_program();

        // check if program has nodes
        if (program->block->statements.size() == 0)
          continue;

        // for (ref<Statement>& node : program->block->statements) {
        //   if (ref<Int> int_node = node->as<Int>()) {
        //     int_node->value += 100;
        //   }
        // }

        program->dump(std::cout);
      } catch (CompilerError& exc) {
        exc.dump(std::cout);
        std::cout << '\n';
      }
    }
  }

  return 0;
}
