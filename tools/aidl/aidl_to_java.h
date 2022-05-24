/*
 * Copyright (C) 2018, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "aidl_language.h"
#include "aidl_typenames.h"

#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace android {
namespace aidl {
namespace java {

using std::map;
using std::pair;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

// This header provides functions that translate AIDL things to Java things.

std::string ConstantValueDecorator(const AidlTypeSpecifier& type, const std::string& raw_value);

// Returns the corresponding Java type name for an AIDL type spec
// This excludes generic type parameters and array modifiers.
const string& JavaNameOf(const AidlTypeSpecifier& aidl);

// Returns the Java type signature of the AIDL type spec
// This includes generic type parameters and array modifiers.
string JavaSignatureOf(const AidlTypeSpecifier& aidl);

// Returns the default Java value of the AIDL type spec
string DefaultJavaValueOf(const AidlTypeSpecifier& aidl);

// This carries information that is required to generate code for
// marshalling and unmarshalling a method argument or a parcelable field
struct CodeGeneratorContext {
  CodeWriter& writer;  // CodeWriter::Write() is mutable
  const AidlTypenames& typenames;
  const AidlTypeSpecifier& type;
  const string parcel;
  const string var;

  // Set to true when the marshalled data will be returned to the client
  // This is given as a hint to the Parcelable that is being marshalled
  // so that the Parcelable can release its resource after the marshalling
  // is done.
  const bool is_return_value;

  // Most of the variables created by AIDL compiler are typed, i.e., the code
  // knows exactly what type of object is in the parcel -- because the parcel
  // itself is created by the code generated by AIDL compiler.
  //
  // However, for some collection types (List and Map for now), we write the
  // elements in them untyped (object is flattened along with its type name)
  // as the AIDL compiler does not know the type of the contained elements.
  // So, when unmarshalling such collection, we need to provide a classloader
  // from where the parcel can reflectively find a class object for
  // the contained element.
  //
  // This field is a pointer to a boolean state variable that indicates whether
  // the code for declaring and getting the classloader has been emitted or not.
  // We emit the code at most once per an AIDL method, otherwise we are wasting
  // time doing the same thing multiple time.
  bool* const is_classloader_created;

  // for error message printing
  const string filename;
};

// Writes code fragment that writes a variable to the parcel.
bool WriteToParcelFor(const CodeGeneratorContext& c);

// Writes code fragment that reads data from the parcel into a variable. When
// the variable type is array or List, the array or List is created.
bool CreateFromParcelFor(const CodeGeneratorContext& c);

// Writes code fragment that reads data from the parcel into an existing
// array or a List.
bool ReadFromParcelFor(const CodeGeneratorContext& c);

}  // namespace java
}  // namespace aidl
}  // namespace android
