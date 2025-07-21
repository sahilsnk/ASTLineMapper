// OutputFormatter.h (No change needed)
#pragma once

#include "clang/Basic/SourceManager.h"
#include "ASTMapper.h"

struct OutputFormatter {
  static void print(const clang::SourceManager &SM, const LineMap &M);
};
