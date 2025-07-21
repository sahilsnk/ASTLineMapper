// ASTMapper.h
#pragma once

#include <map>
#include <vector>
#include <string>
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/FrontendAction.h"

struct ASTNodeInfo {
  std::string kind;
  std::string code;
  std::string explanation; // This will be filled after the Groq call
};

using LineMap = std::map<unsigned, std::vector<ASTNodeInfo>>;

class ASTMappingVisitor : public clang::RecursiveASTVisitor<ASTMappingVisitor> {
public:
  ASTMappingVisitor(clang::ASTContext *C, LineMap &M) : Context(C), Map(M) {}
  bool VisitStmt(clang::Stmt *S);

private:
  clang::ASTContext *Context;
  LineMap &Map;
};

class ASTMappingConsumer : public clang::ASTConsumer {
public:
  ASTMappingConsumer(clang::ASTContext *C, LineMap &M) : Visitor(C, M) {}
  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  ASTMappingVisitor Visitor;
};

class ASTMappingFrontendAction : public clang::ASTFrontendAction {
public:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &CI, llvm::StringRef) override;
  void EndSourceFileAction() override;

private:
  LineMap Map;
  std::string execPythonScript(const std::string& cmd); // Declare helper
};
