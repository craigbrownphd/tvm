/*!
 *  Copyright (c) 2016 by Contributors
 *  Implementation of API functions related to IR build
 * \file api_ir.cc
 */
#include <tvm/expr.h>
#include <tvm/ir.h>
#include <ir/IROperator.h>
#include <tvm/api_registry.h>
#include <tvm/ir_operator.h>

namespace tvm {
namespace ir {

TVM_REGISTER_API("_Var")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    *ret = Variable::make(args[1], args[0]);
  });

TVM_REGISTER_API("make.abs")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    *ret = tvm::abs(args[0]);
  });

TVM_REGISTER_API("make._range_by_min_extent")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    *ret = Range::make_by_min_extent(args[0], args[1]);
  });

TVM_REGISTER_API("make.For")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    *ret = For::make(args[0],
                     args[1],
                     args[2],
                     static_cast<ForType>(args[3].operator int()),
                     static_cast<HalideIR::DeviceAPI>(args[4].operator int()),
                     args[5]);
  });

TVM_REGISTER_API("make.Load")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    Type t = args[0];
    if (args.size() == 3) {
      *ret = Load::make(t, args[1], args[2], const_true(t.lanes()));
    } else {
      *ret = Load::make(t, args[1], args[2], args[3]);
    }
  });

TVM_REGISTER_API("make.Store")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    Expr value = args[1];
    if (args.size() == 3) {
      *ret = Store::make(args[0], value, args[2], const_true(value.type().lanes()));
    } else {
      *ret = Store::make(args[0], value, args[2], args[3]);
    }
  });

TVM_REGISTER_API("make.Realize")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    *ret = Realize::make(args[0],
                         args[1],
                         args[2],
                         args[3],
                         args[4],
                         args[5]);
  });


TVM_REGISTER_API("make.Call")
.set_body([](TVMArgs args,  TVMRetValue *ret) {
    *ret = Call::make(args[0],
                      args[1],
                      args[2],
                      static_cast<Call::CallType>(args[3].operator int()),
                      args[4],
                      args[5]);
  });

TVM_REGISTER_API("make.CommReducer")
.set_body([](TVMArgs args, TVMRetValue *ret) {
    *ret = CommReducerNode::make(args[0],
                                 args[1],
                                 args[2],
                                 args[3]);
  });

// make from two arguments
#define REGISTER_MAKE1(Node)                                 \
  TVM_REGISTER_API("make."#Node)                             \
  .set_body([](TVMArgs args,  TVMRetValue *ret) {            \
      *ret = Node::make(args[0]);                            \
    })                                                       \

#define REGISTER_MAKE2(Node)                                 \
  TVM_REGISTER_API("make."#Node)                             \
  .set_body([](TVMArgs args,  TVMRetValue *ret) {            \
      *ret = Node::make(args[0], args[1]);                   \
    })                                                       \

#define REGISTER_MAKE3(Node)                                 \
  TVM_REGISTER_API("make."#Node)                             \
  .set_body([](TVMArgs args,  TVMRetValue *ret) {            \
      *ret = Node::make(args[0], args[1], args[2]);          \
    })                                                       \

#define REGISTER_MAKE4(Node)                                            \
  TVM_REGISTER_API("make."#Node)                                        \
  .set_body([](TVMArgs args,  TVMRetValue *ret) {                       \
      *ret = Node::make(args[0], args[1], args[2], args[3]);            \
    })                                                                  \

#define REGISTER_MAKE5(Node)                                            \
  TVM_REGISTER_API("make."#Node)                                        \
  .set_body([](TVMArgs args,  TVMRetValue *ret) {                       \
      *ret = Node::make(args[0], args[1], args[2], args[3], args[4]);   \
    })                                                                  \

#define REGISTER_MAKE_BINARY_OP(Node, Func)                  \
  TVM_REGISTER_API("make."#Node)                             \
  .set_body([](TVMArgs args,  TVMRetValue *ret) {            \
      Expr a = args[0], b = args[1];                         \
      *ret = (Func(a, b));                                   \
    })

#define REGISTER_MAKE_BIT_OP(Node, Func)                                \
  TVM_REGISTER_API("make."#Node)                                        \
  .set_body([](TVMArgs args,  TVMRetValue *ret) {                       \
      bool lhs_is_int = args[0].type_code() == kDLInt;                  \
      bool rhs_is_int = args[1].type_code() == kDLInt;                  \
      if (lhs_is_int) {                                                 \
        *ret = (Func(args[0].operator int(), args[1].operator Expr())); \
      } else if (rhs_is_int) {                                          \
        *ret = (Func(args[0].operator Expr(), args[1].operator int())); \
      } else {                                                          \
        *ret = (Func(args[0].operator Expr(), args[1].operator Expr())); \
      }                                                                 \
    })

REGISTER_MAKE5(Reduce);
REGISTER_MAKE4(AttrStmt);

REGISTER_MAKE2(IntImm);
REGISTER_MAKE2(UIntImm);
REGISTER_MAKE2(FloatImm);
REGISTER_MAKE1(StringImm);
REGISTER_MAKE_BINARY_OP(Add, operator+);
REGISTER_MAKE_BINARY_OP(Sub, operator-);
REGISTER_MAKE_BINARY_OP(Mul, operator*);
REGISTER_MAKE_BINARY_OP(Div, operator/);
REGISTER_MAKE_BINARY_OP(Mod, operator%);
REGISTER_MAKE_BINARY_OP(Min, min);
REGISTER_MAKE_BINARY_OP(Max, max);
REGISTER_MAKE_BINARY_OP(EQ, operator==);
REGISTER_MAKE_BINARY_OP(NE, operator!=);
REGISTER_MAKE_BINARY_OP(LT, operator<); // NOLINT(*)
REGISTER_MAKE_BINARY_OP(LE, operator<=); // NOLINT(*)
REGISTER_MAKE_BINARY_OP(GT, operator>);  // NOLINT(*)
REGISTER_MAKE_BINARY_OP(GE, operator>=);
REGISTER_MAKE_BINARY_OP(And, operator&&);
REGISTER_MAKE_BINARY_OP(Or, operator||);
REGISTER_MAKE_BIT_OP(bitwise_and, operator&);
REGISTER_MAKE_BIT_OP(bitwise_or, operator|);
REGISTER_MAKE_BIT_OP(bitwise_xor, operator^);
REGISTER_MAKE_BIT_OP(left_shift, operator<<); // NOLINT(*)
REGISTER_MAKE_BIT_OP(right_shift, operator>>);
REGISTER_MAKE1(Not);
REGISTER_MAKE3(Select);
REGISTER_MAKE3(Ramp);
REGISTER_MAKE2(Cast);
REGISTER_MAKE2(Broadcast);
REGISTER_MAKE3(Let);
REGISTER_MAKE3(LetStmt);
REGISTER_MAKE3(AssertStmt);
REGISTER_MAKE3(ProducerConsumer);
REGISTER_MAKE5(Allocate);
REGISTER_MAKE4(Provide);
REGISTER_MAKE4(Prefetch);
REGISTER_MAKE1(Free);
REGISTER_MAKE2(Block);
REGISTER_MAKE3(IfThenElse);
REGISTER_MAKE1(Evaluate);

}  // namespace ir
}  // namespace tvm
