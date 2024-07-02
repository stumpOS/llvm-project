//===- llvm/TailCallKind.h - LLVM Tail Call Kind ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines LLVM's set of tail call kinds.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_IR_TAILCALLKIND_H
#define LLVM_IR_TAILCALLKIND_H

namespace llvm {

/// TailCallKind Namespace - This namespace contains an enum with a value for
/// the tail call kinds.
///
namespace TailCallKind {

/// LLVM IR allows to use arbitrary numbers as calling convention identifiers.
using ID = unsigned;

/// A set of enums which specify the assigned numeric values for known llvm
/// calling conventions.
/// Note that 'musttail' implies 'tail'.
enum {
  None = 0,
  Tail = 1,
  MustTail = 2,
  NoTail = 3,
  Last = NoTail
};

} // end namespace TailCallKind

} // end namespace llvm

#endif // LLVM_IR_TAILCALLKIND_H
