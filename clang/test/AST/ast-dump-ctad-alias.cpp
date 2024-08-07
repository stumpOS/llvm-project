// RUN: %clang_cc1 -triple x86_64-unknown-unknown -std=c++2a -ast-dump %s | FileCheck -strict-whitespace %s

template <typename, typename>
constexpr bool Concept = true;
template<typename T> // depth 0
struct Out {
  template<typename U> // depth 1
  struct Inner {
    U t;
  };

  template<typename V> // depth1
  requires Concept<T, V>
  Inner(V) -> Inner<V>;
};

template <typename X>
struct Out2 {
  template<typename Y> // depth1
  using AInner = Out<int>::Inner<Y>;
};
Out2<double>::AInner t(1.0);

// Verify that the require-clause of alias deduction guide is transformed correctly:
//   - Occurrence T should be replaced with `int`;
//   - Occurrence V should be replaced with the Y with depth 1
//   - Depth of occurrence Y in the __is_deducible constraint should be 1
//
// CHECK:      |   `-FunctionTemplateDecl {{.*}} <deduction guide for AInner>
// CHECK-NEXT: |     |-TemplateTypeParmDecl {{.*}} typename depth 0 index 0 Y
// CHECK-NEXT: |     |-BinaryOperator {{.*}} '<dependent type>' '&&'
// CHECK-NEXT: |     | |-UnresolvedLookupExpr {{.*}} '<dependent type>' lvalue (no ADL) = 'Concept'
// CHECK-NEXT: |     | | |-TemplateArgument type 'int'
// CHECK-NEXT: |     | | | `-BuiltinType {{.*}} 'int'
// CHECK-NEXT: |     | | `-TemplateArgument type 'type-parameter-1-0'
// CHECK-NEXT: |     | |   `-TemplateTypeParmType {{.*}} 'type-parameter-1-0' dependent depth 1 index 0
// CHECK-NEXT: |     | `-TypeTraitExpr {{.*}} 'bool' __is_deducible
// CHECK-NEXT: |     |   |-DeducedTemplateSpecializationType {{.*}} 'Out2<double>::AInner' dependent
// CHECK-NEXT: |     |   | `-name: 'Out2<double>::AInner'
// CHECK-NEXT: |     |   |   `-TypeAliasTemplateDecl {{.+}} AInner{{$}}
// CHECK-NEXT: |     |   `-ElaboratedType {{.*}} 'Inner<type-parameter-1-0>' sugar dependent
// CHECK-NEXT: |     |     `-TemplateSpecializationType {{.*}} 'Inner<type-parameter-1-0>' dependent
// CHECK-NEXT: |     |       |-name: 'Inner':'Out<int>::Inner' qualified
// CHECK-NEXT: |     |       | `-ClassTemplateDecl {{.+}} Inner{{$}}
// CHECK-NEXT: |     |       `-TemplateArgument type 'type-parameter-1-0'
// CHECK-NEXT: |     |         `-SubstTemplateTypeParmType {{.*}} 'type-parameter-1-0'
// CHECK-NEXT: |     |           |-FunctionTemplate {{.*}} '<deduction guide for Inner>'
// CHECK-NEXT: |     |           `-TemplateTypeParmType {{.*}} 'type-parameter-1-0' dependent depth 1 index 0
// CHECK-NEXT: |     |-CXXDeductionGuideDecl {{.*}} <deduction guide for AInner> 'auto (type-parameter-0-0) -> Inner<type-parameter-0-0>'
// CHECK-NEXT: |     | `-ParmVarDecl {{.*}} 'type-parameter-0-0'
// CHECK-NEXT: |     `-CXXDeductionGuideDecl {{.*}} used <deduction guide for AInner> 'auto (double) -> Inner<double>' implicit_instantiation
// CHECK-NEXT: |       |-TemplateArgument type 'double'
// CHECK-NEXT: |       | `-BuiltinType {{.*}} 'double'
// CHECK-NEXT: |       `-ParmVarDecl {{.*}} 'double'

// GH92596
template <typename T0>
struct Out3 {
  template<class T1, typename T2>
  struct Foo {
    // Deduction guide:
    //   template <typename T1, typename T2, typename V>
    //   Foo(V, T1) -> Foo<T1, T2>;
    template<class V> requires Concept<T0, V> // V in require clause of Foo deduction guide: depth 1, index: 2
    Foo(V, T1);
  };
};
template<class T3>
using AFoo3 = Out3<int>::Foo<T3, T3>;
AFoo3 afoo3{0, 1};
// Verify occurrence V in the require-clause is transformed (depth: 1 => 0, index: 2 => 1) correctly.

// CHECK:      FunctionTemplateDecl {{.*}} implicit <deduction guide for AFoo3>
// CHECK-NEXT: |-TemplateTypeParmDecl {{.*}} class depth 0 index 0 T3
// CHECK-NEXT: |-TemplateTypeParmDecl {{.*}} class depth 0 index 1 V
// CHECK-NEXT: |-BinaryOperator {{.*}} '<dependent type>' '&&'
// CHECK-NEXT: | |-UnresolvedLookupExpr {{.*}} '<dependent type>' lvalue (no ADL) = 'Concept'
// CHECK-NEXT: | | |-TemplateArgument type 'int'
// CHECK-NEXT: | | | `-BuiltinType {{.*}} 'int'
// CHECK-NEXT: | | `-TemplateArgument type 'type-parameter-0-1'
// CHECK-NEXT: | |   `-TemplateTypeParmType {{.*}} 'type-parameter-0-1' dependent depth 0 index 1

template <typename... T1>
struct Foo {
  Foo(T1...);
};

template <typename...T2>
using AFoo = Foo<T2...>;
AFoo a(1, 2);
// CHECK:      |-CXXDeductionGuideDecl {{.*}} implicit <deduction guide for AFoo> 'auto (type-parameter-0-0...) -> Foo<type-parameter-0-0...>'
// CHECK-NEXT: | | `-ParmVarDecl {{.*}} 'type-parameter-0-0...' pack
// CHECK-NEXT: | `-CXXDeductionGuideDecl {{.*}} implicit used <deduction guide for AFoo> 'auto (int, int) -> Foo<int, int>' implicit_instantiation

template <typename T>
using BFoo = Foo<T, T>;
BFoo b2(1.0, 2.0);
// CHECK:      |-CXXDeductionGuideDecl {{.*}} implicit <deduction guide for BFoo> 'auto (type-parameter-0-0, type-parameter-0-0) -> Foo<type-parameter-0-0, type-parameter-0-0>'
// CHECK-NEXT: | | |-ParmVarDecl {{.*}} 'type-parameter-0-0'
// CHECK-NEXT: | | `-ParmVarDecl {{.*}} 'type-parameter-0-0'
// CHECK-NEXT: | `-CXXDeductionGuideDecl {{.*}} implicit used <deduction guide for BFoo> 'auto (double, double) -> Foo<double, double>' implicit_instantiation
