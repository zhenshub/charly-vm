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

#include <catch2/catch_all.hpp>

#include "astmacros.h"

TEST_CASE("validates assignments") {
  COMPILE_ERROR("2 = 25", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("false = 25", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("self = 25", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("(a, b) += 25",
                "this type of expression cannot be used as the left-hand side of an operator assignment");
  COMPILE_ERROR("({a, b} += 25)",
                "this type of expression cannot be used as the left-hand side of an operator assignment");
  COMPILE_ERROR("() = 25", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("(1) = 25", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("(...a, ...b) = 25", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("({} = 25)", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("({a: 1} = 25)", "left-hand side of assignment is not assignable");
  COMPILE_ERROR("({...a, ...b} = 25)", "left-hand side of assignment is not assignable");

  COMPILE_ERROR("let () = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let (1) = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let (a.a) = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let (2 + 2) = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let (...a, ...d) = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let ([1, 2]) = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let (\"a\") = 1", "left-hand side of declaration is not assignable");

  COMPILE_ERROR("let {} = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let {1} = 1", "expected identifier, member access or spread expression");
  COMPILE_ERROR("let {a.a} = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let {2 + 2} = 1", "expected identifier, member access or spread expression");
  COMPILE_ERROR("let {...a, ...d} = 1", "left-hand side of declaration is not assignable");
  COMPILE_ERROR("let {[1, 2]} = 1", "expected identifier, member access or spread expression");
  COMPILE_ERROR("let {\"a\"} = 1", "expected identifier, member access or spread expression");
}

TEST_CASE("validates dict literals") {
  COMPILE_ERROR("({25})", "expected identifier, member access or spread expression");
  COMPILE_ERROR("({false})", "expected identifier, member access or spread expression");
  COMPILE_ERROR("({,})", "unexpected ',' token, expected an expression");
  COMPILE_ERROR("({:})", "unexpected ':' token, expected an expression");
  COMPILE_ERROR("({\"foo\"})", "expected identifier, member access or spread expression");
  COMPILE_ERROR("({[x]})", "expected identifier, member access or spread expression");
  COMPILE_ERROR("({-5})", "expected identifier, member access or spread expression");
  COMPILE_ERROR("({[1, 2]: 1})", "expected identifier or string literal");
  COMPILE_ERROR("({25: 1})", "expected identifier or string literal");
  COMPILE_ERROR("({true: 1})", "expected identifier or string literal");
  COMPILE_ERROR("({...x: 1})", "expected identifier or string literal");
}

TEST_CASE("validates spawn statements") {
  COMPILE_ERROR("spawn foo", "expected a call expression");
  COMPILE_ERROR("spawn foo.bar", "expected a call expression");
  COMPILE_ERROR("spawn foo.bar[1]", "expected a call expression");
  COMPILE_ERROR("spawn foo().bar", "expected a call expression");
}

TEST_CASE("checks for reserved identifiers") {
  COMPILE_ERROR("const $ = 1", "'$' is a reserved variable name");
  COMPILE_ERROR("const $$ = 1", "'$$' is a reserved variable name");
  COMPILE_ERROR("const $0 = 1", "'$0' is a reserved variable name");
  COMPILE_ERROR("let $1 = 1", "'$1' is a reserved variable name");
  COMPILE_ERROR("let $5 = 1", "'$5' is a reserved variable name");
  COMPILE_ERROR("let $500 = 1", "'$500' is a reserved variable name");

  COMPILE_ERROR("let ($) = 1", "'$' is a reserved variable name");
  COMPILE_ERROR("let (a, $$) = 1", "'$$' is a reserved variable name");
  COMPILE_ERROR("let (...$$) = 1", "'$$' is a reserved variable name");
  COMPILE_ERROR("let { $ } = 1", "'$' is a reserved variable name");
  COMPILE_ERROR("let { a, $$ } = 1", "'$$' is a reserved variable name");
  COMPILE_ERROR("let { ...$$ } = 1", "'$$' is a reserved variable name");

  COMPILE_ERROR("func foo($10) {}", "'$10' is a reserved variable name");
  COMPILE_ERROR("func foo($10 = 1) {}", "'$10' is a reserved variable name");
  COMPILE_ERROR("func foo(...$10) {}", "'$10' is a reserved variable name");

  COMPILE_ERROR("class x { property $10 }", "'$10' cannot be the name of a property");
  COMPILE_ERROR("class x { $10 {} }", "'$10' cannot be the name of a member function");
  COMPILE_ERROR("class x { static property $10 }", "'$10' cannot be the name of a static property");
  COMPILE_ERROR("class x { static $10 {} }", "'$10' cannot be the name of a static property");

  COMPILE_ERROR("class x { property klass }", "'klass' cannot be the name of a property");
  COMPILE_ERROR("class x { property object_id }", "'object_id' cannot be the name of a property");

  COMPILE_ERROR("class x { static property constructor }",
                "'constructor' cannot be the name of a static property");
  COMPILE_ERROR("class x { static property name }", "'name' cannot be the name of a static property");
  COMPILE_ERROR("class x { static property parent }", "'parent' cannot be the name of a static property");
}

TEST_CASE("checks for duplicate identifiers") {
  COMPILE_ERROR("let (a, a) = x", "duplicate key 'a'");
  COMPILE_ERROR("let (a, ...a) = x", "duplicate key 'a'");
  COMPILE_ERROR("let {a, a} = x", "duplicate key 'a'");
  COMPILE_ERROR("let {a, ...a} = x", "duplicate key 'a'");

  COMPILE_ERROR("({a: 1, a: 2})", "duplicate key 'a'");

  COMPILE_ERROR("func foo(a, a) {}", "duplicate argument 'a'");
  COMPILE_ERROR("func foo(a, a = 1) {}", "duplicate argument 'a'");
  COMPILE_ERROR("func foo(a, ...a) {}", "duplicate argument 'a'");

  COMPILE_ERROR("class A { property foo property foo }", "duplicate declaration of 'foo'");
  COMPILE_ERROR("class A { property foo foo {} }", "redeclaration of property 'foo' as function");
  COMPILE_ERROR("class A { foo {} property foo }", "redeclaration of property 'foo' as function");
  COMPILE_ERROR("class A { constructor {} constructor {} }", "duplicate declaration of class constructor");
  COMPILE_ERROR("class A { foo {} foo {} }", "duplicate declaration of member function 'foo'");
  COMPILE_ERROR("class A { static property foo static property foo }",
                "duplicate declaration of static property 'foo'");
  COMPILE_ERROR("class A { static property foo static foo {} }", "duplicate declaration of static property 'foo'");
  COMPILE_ERROR("class A { static foo {} static property foo }", "duplicate declaration of static property 'foo'");

  COMPILE_OK("class A { property foo static property foo }");
  COMPILE_OK("class A { foo {} static foo {} }");
  COMPILE_OK("class A { constructor {} }");
}

TEST_CASE("checks for missing function default arguments") {
  COMPILE_ERROR("func foo(a = 1, b) {}", "argument 'b' is missing a default value");
  COMPILE_ERROR("->(a = 1, b) {}", "argument 'b' is missing a default value");
}

TEST_CASE("spread arguments cannot have default arguments") {
  COMPILE_ERROR("func foo(...x = 1) {}", "spread argument cannot have a default value");
  COMPILE_ERROR("->(...x = 1) {}", "spread argument cannot have a default value");
}

TEST_CASE("checks for excess arguments in functions") {
  COMPILE_ERROR("func foo(...foo, ...rest) {}", "excess parameter(s)");
  COMPILE_ERROR("func foo(...foo, a, b, c) {}", "excess parameter(s)");
  COMPILE_ERROR("->(...foo, ...rest) {}", "excess parameter(s)");
  COMPILE_ERROR("->(...foo, a, b, c) {}", "excess parameter(s)");
}

TEST_CASE("checks for missing calls to parent constructor in subclasses") {
  COMPILE_ERROR("class A extends B { constructor {} }",
                "missing call to super inside constructor of class 'A'");
  COMPILE_ERROR("class A extends B { constructor { super.foo() } }",
                "missing call to super inside constructor of class 'A'");
  COMPILE_OK("class A { constructor {} }");
}

TEST_CASE("checks for missing constructors in subclasses with properties") {
  COMPILE_ERROR("class A extends B { property x }", "class 'A' is missing a constructor");
  COMPILE_OK("let B = null class A extends B {}");
}

TEST_CASE("checks for yield statements outside regular functions") {
  COMPILE_ERROR("yield 1", "yield expression not allowed at this point");
  COMPILE_ERROR("->{ yield 1 }", "yield expression not allowed at this point");
  COMPILE_ERROR("class A { constructor { yield 1 } }", "yield expression not allowed at this point");
  COMPILE_ERROR("class A { constructor { ->{ yield 1 } } }", "yield expression not allowed at this point");
  COMPILE_OK("class A { foo { yield 1 } }");
  COMPILE_OK("class A { static foo { yield 1 } }");
  COMPILE_OK("func foo { yield 1 }");
  COMPILE_OK("spawn { yield 1 }");
}

TEST_CASE("only allows @a parameter syntax inside class member functions") {
  COMPILE_ERROR("func foo(@a) {}",
                "unexpected '@' token, self initializer arguments are only allowed inside class member functions");
  COMPILE_ERROR("->(@a) {}",
                "unexpected '@' token, self initializer arguments are only allowed inside class member functions");
  COMPILE_ERROR("class A { static foo(@a) }",
                "unexpected '@' token, self initializer arguments are only allowed inside class member functions");
  COMPILE_OK("class A { foo(@a) }");
}