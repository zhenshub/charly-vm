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

#include <algorithm>
#include <unordered_set>

#include "charly/core/compiler/passes/duplicates_check.h"

namespace charly::core::compiler::ast {

void DuplicatesCheck::inspect_leave(const ref<UnpackDeclaration>& node) {
  std::unordered_set<std::string> names;

  for (const ref<UnpackTargetElement>& element : node->target->elements) {
    if (names.count(element->name->value)) {
      m_console.error(element->name, "duplicate key '", element->name->value, "'");
    }
    names.insert(element->name->value);
  }
}

void DuplicatesCheck::inspect_leave(const ref<Dict>& node) {
  std::unordered_set<std::string> dict_keys;

  for (ref<DictEntry>& entry : node->elements) {
    switch (entry->key->type()) {
      case Node::Type::Name: {
        ref<Name> name = cast<Name>(entry->key);

        if (dict_keys.count(name->value)) {
          m_console.error(entry->key, "duplicate key '", name->value, "'");
        }

        dict_keys.insert(name->value);
        break;
      }
      default: {
        continue;
      }
    }
  }
}

void DuplicatesCheck::inspect_leave(const ref<Function>& node) {
  std::unordered_set<std::string> argument_names;
  for (const ref<FunctionArgument>& argument : node->arguments) {
    if (argument_names.count(argument->name->value)) {
      m_console.error(argument->name, "duplicate argument '", argument->name->value, "'");
    } else {
      argument_names.insert(argument->name->value);
    }
  }
}

void DuplicatesCheck::inspect_leave(const ref<Class>& node) {
  std::unordered_map<std::string, ref<ClassProperty>> class_member_properties;
  std::unordered_map<std::string, ref<Function>> class_member_functions;
  std::unordered_map<std::string, ref<ClassProperty>> class_static_properties;

  // check for duplicate properties
  for (const ref<ClassProperty>& prop : node->member_properties) {
    if (class_member_properties.count(prop->name->value)) {
      m_console.error(prop->name, "duplicate declaration of '", prop->name->value, "'");
      m_console.info(class_member_properties.at(prop->name->value)->name, "first declared here");
      continue;
    }

    class_member_properties.insert({prop->name->value, prop});
  }

  for (const ref<Function>& func : node->member_functions) {
    const std::string& name = func->name->value;

    if (class_member_properties.count(name)) {
      m_console.error(func->name, "redeclaration of property '", func->name->value, "' as function");
      m_console.info(class_member_properties.at(name)->name, "declared as property here");
      continue;
    }

    if (class_member_functions.count(name)) {
      m_console.error(func->name, "duplicate declaration of member function '", func->name->value, "'");
      m_console.info(class_member_functions.at(name)->name, "first declared here");
      continue;
    }

    class_member_functions.insert({name, func});
  }

  // check for duplicate static properties
  for (const ref<ClassProperty>& prop : node->static_properties) {
    if (class_static_properties.count(prop->name->value)) {
      m_console.error(prop->name, "duplicate declaration of static property '", prop->name->value, "'");
      m_console.info(class_static_properties.at(prop->name->value)->name, "declared here");
      continue;
    }

    class_static_properties[prop->name->value] = prop;
  }
}

}  // namespace charly::core::compiler::ast