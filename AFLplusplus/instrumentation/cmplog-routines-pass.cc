/*
   american fuzzy lop++ - LLVM CmpLog instrumentation
   --------------------------------------------------

   Written by Andrea Fioraldi <andreafioraldi@gmail.com>

   Copyright 2015, 2016 Google Inc. All rights reserved.
   Copyright 2019-2024 AFLplusplus Project. All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at:

     https://www.apache.org/licenses/LICENSE-2.0

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <list>
#include <string>
#include <fstream>
#include <sys/time.h>
#include "llvm/Config/llvm-config.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#if LLVM_VERSION_MAJOR < 17
  #include "llvm/Transforms/IPO/PassManagerBuilder.h"
#endif
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/ValueTracking.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DebugInfo.h"

#include <set>
#include "afl-llvm-common.h"

using namespace llvm;

namespace {

class CmpLogRoutines : public PassInfoMixin<CmpLogRoutines> {

 public:
  CmpLogRoutines() {

    initInstrumentList();

  }

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);

 private:
  bool hookRtns(Module &M);

};

}  // namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {

  return {LLVM_PLUGIN_API_VERSION, "cmplogroutines", "v0.1",
          /* lambda to insert our pass into the pass pipeline. */
          [](PassBuilder &PB) {

#if LLVM_VERSION_MAJOR <= 13
            using OptimizationLevel = typename PassBuilder::OptimizationLevel;
#endif
            PB.registerOptimizerLastEPCallback([](ModulePassManager &MPM,
                                                  OptimizationLevel  OL
#if LLVM_VERSION_MAJOR >= 20
                                                  ,
                                                  ThinOrFullLTOPhase Phase
#endif
                                               ) {

              MPM.addPass(CmpLogRoutines());

            });

          }};

}

bool CmpLogRoutines::hookRtns(Module &M) {

  std::vector<CallInst *> calls, llvmStdStd, llvmStdC, gccStdStd, gccStdC,
      Memcmp, Strcmp, Strncmp;
  LLVMContext &C = M.getContext();

  Type *VoidTy = Type::getVoidTy(C);
  // PointerType *VoidPtrTy = PointerType::get(VoidTy, 0);
  IntegerType *Int8Ty = IntegerType::getInt8Ty(C);
  IntegerType *Int64Ty = IntegerType::getInt64Ty(C);
  PointerType *i8PtrTy = PointerType::get(Int8Ty, 0);

  FunctionCallee c =
      M.getOrInsertFunction("__cmplog_rtn_hook", VoidTy, i8PtrTy, i8PtrTy);
  FunctionCallee cmplogHookFn = c;

  FunctionCallee c1 = M.getOrInsertFunction(
      "__cmplog_rtn_llvm_stdstring_stdstring", VoidTy, i8PtrTy, i8PtrTy);
  FunctionCallee cmplogLlvmStdStd = c1;

  FunctionCallee c2 = M.getOrInsertFunction(
      "__cmplog_rtn_llvm_stdstring_cstring", VoidTy, i8PtrTy, i8PtrTy);
  FunctionCallee cmplogLlvmStdC = c2;

  FunctionCallee c3 = M.getOrInsertFunction(
      "__cmplog_rtn_gcc_stdstring_stdstring", VoidTy, i8PtrTy, i8PtrTy);
  FunctionCallee cmplogGccStdStd = c3;

  FunctionCallee c4 = M.getOrInsertFunction(
      "__cmplog_rtn_gcc_stdstring_cstring", VoidTy, i8PtrTy, i8PtrTy);
  FunctionCallee cmplogGccStdC = c4;

  FunctionCallee c5 = M.getOrInsertFunction("__cmplog_rtn_hook_n", VoidTy,
                                            i8PtrTy, i8PtrTy, Int64Ty);
  FunctionCallee cmplogHookFnN = c5;

  FunctionCallee c6 = M.getOrInsertFunction("__cmplog_rtn_hook_strn", VoidTy,
                                            i8PtrTy, i8PtrTy, Int64Ty);
  FunctionCallee cmplogHookFnStrN = c6;

  FunctionCallee c7 =
      M.getOrInsertFunction("__cmplog_rtn_hook_str", VoidTy, i8PtrTy, i8PtrTy);
  FunctionCallee cmplogHookFnStr = c7;

  GlobalVariable *AFLCmplogPtr = M.getNamedGlobal("__afl_cmp_map");

  if (!AFLCmplogPtr) {

    AFLCmplogPtr = new GlobalVariable(M, PointerType::get(Int8Ty, 0), false,
                                      GlobalValue::ExternalWeakLinkage, 0,
                                      "__afl_cmp_map");

  }

  Constant *Null = Constant::getNullValue(PointerType::get(Int8Ty, 0));

  /* iterate over all functions, bbs and instruction and add suitable calls */
  for (auto &F : M) {

    if (!isInInstrumentList(&F, MNAME)) continue;

    for (auto &BB : F) {

      for (auto &IN : BB) {

        CallInst *callInst = nullptr;

        if ((callInst = dyn_cast<CallInst>(&IN))) {

          Function *Callee = callInst->getCalledFunction();
          if (!Callee) continue;
          if (callInst->getCallingConv() != llvm::CallingConv::C) continue;

          FunctionType *FT = Callee->getFunctionType();
          std::string   FuncName = Callee->getName().str();

          bool isPtrRtn = FT->getNumParams() >= 2 &&
                          !FT->getReturnType()->isVoidTy() &&
                          FT->getParamType(0) == FT->getParamType(1) &&
                          FT->getParamType(0)->isPointerTy();

          bool isPtrRtnN = FT->getNumParams() >= 3 &&
                           !FT->getReturnType()->isVoidTy() &&
                           FT->getParamType(0) == FT->getParamType(1) &&
                           FT->getParamType(0)->isPointerTy() &&
                           FT->getParamType(2)->isIntegerTy();
          if (isPtrRtnN) {

            auto intTyOp =
                dyn_cast<IntegerType>(callInst->getArgOperand(2)->getType());
            if (intTyOp) {

              if (intTyOp->getBitWidth() != 32 &&
                  intTyOp->getBitWidth() != 64) {

                isPtrRtnN = false;

              }

            }

          }

          bool isMemcmp =
              (!FuncName.compare("memcmp") || !FuncName.compare("bcmp") ||
               !FuncName.compare("CRYPTO_memcmp") ||
               !FuncName.compare("OPENSSL_memcmp") ||
               !FuncName.compare("memcmp_const_time") ||
               !FuncName.compare("memcmpct"));
          isMemcmp &= FT->getNumParams() == 3 &&
                      FT->getReturnType()->isIntegerTy(32) &&
                      FT->getParamType(0)->isPointerTy() &&
                      FT->getParamType(1)->isPointerTy() &&
                      FT->getParamType(2)->isIntegerTy();

          bool isStrcmp =
              (!FuncName.compare("strcmp") || !FuncName.compare("xmlStrcmp") ||
               !FuncName.compare("xmlStrEqual") ||
               !FuncName.compare("g_strcmp0") ||
               !FuncName.compare("curl_strequal") ||
               !FuncName.compare("strcsequal") ||
               !FuncName.compare("strcasecmp") ||
               !FuncName.compare("stricmp") ||
               !FuncName.compare("ap_cstr_casecmp") ||
               !FuncName.compare("OPENSSL_strcasecmp") ||
               !FuncName.compare("xmlStrcasecmp") ||
               !FuncName.compare("g_strcasecmp") ||
               !FuncName.compare("g_ascii_strcasecmp") ||
               !FuncName.compare("Curl_strcasecompare") ||
               !FuncName.compare("Curl_safe_strcasecompare") ||
               !FuncName.compare("cmsstrcasecmp") ||
               !FuncName.compare("strstr") ||
               !FuncName.compare("g_strstr_len") ||
               !FuncName.compare("ap_strcasestr") ||
               !FuncName.compare("xmlStrstr") ||
               !FuncName.compare("xmlStrcasestr") ||
               !FuncName.compare("g_str_has_prefix") ||
               !FuncName.compare("g_str_has_suffix"));
          isStrcmp &=
              FT->getNumParams() == 2 && FT->getReturnType()->isIntegerTy(32) &&
              FT->getParamType(0) == FT->getParamType(1) &&
              FT->getParamType(0) ==
                  IntegerType::getInt8Ty(M.getContext())->getPointerTo(0);

          bool isStrncmp = (!FuncName.compare("strncmp") ||
                            !FuncName.compare("xmlStrncmp") ||
                            !FuncName.compare("curl_strnequal") ||
                            !FuncName.compare("strncasecmp") ||
                            !FuncName.compare("strnicmp") ||
                            !FuncName.compare("ap_cstr_casecmpn") ||
                            !FuncName.compare("OPENSSL_strncasecmp") ||
                            !FuncName.compare("xmlStrncasecmp") ||
                            !FuncName.compare("g_ascii_strncasecmp") ||
                            !FuncName.compare("Curl_strncasecompare") ||
                            !FuncName.compare("g_strncasecmp"));
          isStrncmp &=
              FT->getNumParams() == 3 && FT->getReturnType()->isIntegerTy(32) &&
              FT->getParamType(0) == FT->getParamType(1) &&
              FT->getParamType(0) ==
                  IntegerType::getInt8Ty(M.getContext())->getPointerTo(0) &&
              FT->getParamType(2)->isIntegerTy();

          bool isGccStdStringStdString =
              Callee->getName().find("__is_charIT_EE7__value") !=
                  std::string::npos &&
              Callee->getName().find(
                  "St7__cxx1112basic_stringIS2_St11char_traits") !=
                  std::string::npos &&
              FT->getNumParams() >= 2 &&
              FT->getParamType(0) == FT->getParamType(1) &&
              FT->getParamType(0)->isPointerTy();

          bool isGccStdStringCString =
              Callee->getName().find(
                  "St7__cxx1112basic_stringIcSt11char_"
                  "traitsIcESaIcEE7compareEPK") != std::string::npos &&
              FT->getNumParams() >= 2 && FT->getParamType(0)->isPointerTy() &&
              FT->getParamType(1)->isPointerTy();

          bool isLlvmStdStringStdString =
              Callee->getName().find("_ZNSt3__1eqI") != std::string::npos &&
              Callee->getName().find("_12basic_stringI") != std::string::npos &&
              Callee->getName().find("_11char_traits") != std::string::npos &&
              FT->getNumParams() >= 2 && FT->getParamType(0)->isPointerTy() &&
              FT->getParamType(1)->isPointerTy();

          bool isLlvmStdStringCString =
              Callee->getName().find("_ZNSt3__1eqI") != std::string::npos &&
              Callee->getName().find("_12basic_stringI") != std::string::npos &&
              FT->getNumParams() >= 2 && FT->getParamType(0)->isPointerTy() &&
              FT->getParamType(1)->isPointerTy();

          /*
                    {

                       fprintf(stderr, "F:%s C:%s argc:%u\n",
                       F.getName().str().c_str(),
             Callee->getName().str().c_str(), FT->getNumParams());
                       fprintf(stderr, "ptr0:%u ptr1:%u ptr2:%u\n",
                              FT->getParamType(0)->isPointerTy(),
                              FT->getParamType(1)->isPointerTy(),
                              FT->getNumParams() > 2 ?
             FT->getParamType(2)->isPointerTy() : 22 );

                    }

          */

          if (isGccStdStringCString || isGccStdStringStdString ||
              isLlvmStdStringStdString || isLlvmStdStringCString || isMemcmp ||
              isStrcmp || isStrncmp) {

            isPtrRtnN = isPtrRtn = false;

          }

          if (isPtrRtnN) { isPtrRtn = false; }

          if (isPtrRtn) { calls.push_back(callInst); }
          if (isMemcmp || isPtrRtnN) { Memcmp.push_back(callInst); }
          if (isStrcmp) { Strcmp.push_back(callInst); }
          if (isStrncmp) { Strncmp.push_back(callInst); }
          if (isGccStdStringStdString) { gccStdStd.push_back(callInst); }
          if (isGccStdStringCString) { gccStdC.push_back(callInst); }
          if (isLlvmStdStringStdString) { llvmStdStd.push_back(callInst); }
          if (isLlvmStdStringCString) { llvmStdC.push_back(callInst); }

        }

      }

    }

  }

  if (!calls.size() && !gccStdStd.size() && !gccStdC.size() &&
      !llvmStdStd.size() && !llvmStdC.size() && !Memcmp.size() &&
      !Strcmp.size() && !Strncmp.size())
    return false;

  /*
    if (!be_quiet)
      errs() << "Hooking " << calls.size()
             << " calls with pointers as arguments\n";
  */

  for (auto &callInst : calls) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);

    IRB.CreateCall(cmplogHookFn, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  for (auto &callInst : Memcmp) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1),
          *v3P = callInst->getArgOperand(2);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    Value               *v3Pbitcast = IRB.CreateBitCast(
        v3P, IntegerType::get(C, v3P->getType()->getPrimitiveSizeInBits()));
    Value *v3Pcasted =
        IRB.CreateIntCast(v3Pbitcast, IntegerType::get(C, 64), false);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);
    args.push_back(v3Pcasted);

    IRB.CreateCall(cmplogHookFnN, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  for (auto &callInst : Strcmp) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);

    IRB.CreateCall(cmplogHookFnStr, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  for (auto &callInst : Strncmp) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1),
          *v3P = callInst->getArgOperand(2);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    Value               *v3Pbitcast = IRB.CreateBitCast(
        v3P, IntegerType::get(C, v3P->getType()->getPrimitiveSizeInBits()));
    Value *v3Pcasted =
        IRB.CreateIntCast(v3Pbitcast, IntegerType::get(C, 64), false);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);
    args.push_back(v3Pcasted);

    IRB.CreateCall(cmplogHookFnStrN, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  for (auto &callInst : gccStdStd) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);

    IRB.CreateCall(cmplogGccStdStd, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  for (auto &callInst : gccStdC) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);

    IRB.CreateCall(cmplogGccStdC, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  for (auto &callInst : llvmStdStd) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);

    IRB.CreateCall(cmplogLlvmStdStd, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  for (auto &callInst : llvmStdC) {

    Value *v1P = callInst->getArgOperand(0), *v2P = callInst->getArgOperand(1);

    IRBuilder<> IRB2(callInst->getParent());
    IRB2.SetInsertPoint(callInst);

    LoadInst *CmpPtr =
        IRB2.CreateLoad(PointerType::get(Int8Ty, 0), AFLCmplogPtr);
    CmpPtr->setMetadata(M.getMDKindID("nosanitize"),
#if LLVM_MAJOR >= 20
                        MDNode::get(C, {}));
#else
                        MDNode::get(C, None));
#endif
    auto is_not_null = IRB2.CreateICmpNE(CmpPtr, Null);
    auto ThenTerm = SplitBlockAndInsertIfThen(is_not_null, callInst, false);

    IRBuilder<> IRB(ThenTerm);

    std::vector<Value *> args;
    Value               *v1Pcasted = IRB.CreatePointerCast(v1P, i8PtrTy);
    Value               *v2Pcasted = IRB.CreatePointerCast(v2P, i8PtrTy);
    args.push_back(v1Pcasted);
    args.push_back(v2Pcasted);

    IRB.CreateCall(cmplogLlvmStdC, args);

    // errs() << callInst->getCalledFunction()->getName() << "\n";

  }

  return true;

}

PreservedAnalyses CmpLogRoutines::run(Module &M, ModuleAnalysisManager &MAM) {

  if (getenv("AFL_QUIET") == NULL)
    printf("Running cmplog-routines-pass by andreafioraldi@gmail.com\n");
  else
    be_quiet = 1;
  bool ret = hookRtns(M);
  verifyModule(M);

  if (ret == false)
    return PreservedAnalyses::all();
  else
    return PreservedAnalyses();

}

