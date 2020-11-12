// Copyright 2015-2020 Autoware Foundation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__FILE_HEADER_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__FILE_HEADER_HPP_

#include <openscenario_interpreter/reader/attribute.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ==== FileHeader ===========================================================
 *
 * <xsd:complexType name="FileHeader">
 *   <xsd:attribute name="revMajor" type="UnsignedShort" use="required"/>
 *   <xsd:attribute name="revMinor" type="UnsignedShort" use="required"/>
 *   <xsd:attribute name="date" type="DateTime" use="required"/>
 *   <xsd:attribute name="description" type="String" use="required"/>
 *   <xsd:attribute name="author" type="String" use="required"/>
 * </xsd:complexType>
 *
 * ======================================================================== */
struct FileHeader
{
  const UnsignedShort revMajor;
  const UnsignedShort revMinor;

  const String date;

  const String description;

  const String author;

  template<typename Node, typename Scope>
  explicit FileHeader(const Node & node, Scope & outer_scope)
  : revMajor{readAttribute<UnsignedShort>("revMajor", node, outer_scope)},
    revMinor{readAttribute<UnsignedShort>("revMinor", node, outer_scope)},
    date{readAttribute<String>("date", node, outer_scope)},
    description{readAttribute<String>("description", node, outer_scope)},
    author{readAttribute<String>("author", node, outer_scope)}
  {}
};
}
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__FILE_HEADER_HPP_