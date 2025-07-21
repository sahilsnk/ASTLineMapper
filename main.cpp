// main.cpp
#include "ASTMapper.h"
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory MyToolCategory("ast-mapper options");

int main(int argc, const char **argv) {
  auto expectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!expectedParser) {
    llvm::errs() << expectedParser.takeError();
    return 1;
  }
  ClangTool Tool(expectedParser->getCompilations(),
                 expectedParser->getSourcePathList());
  return Tool.run(newFrontendActionFactory<ASTMappingFrontendAction>().get());
}
