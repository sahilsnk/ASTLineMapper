// OutputFormatter.cpp (No change needed)
#include "OutputFormatter.h"
#include <fstream>
#include <iostream>

void OutputFormatter::print(const clang::SourceManager &SM, const LineMap &M) {
  auto path = SM.getFileEntryForID(SM.getMainFileID())->getName();
  std::ifstream in(path.str());
  std::string line;
  unsigned L = 1;

  while (std::getline(in, line)) {
    std::cout << L << ": " << line;
    auto it = M.find(L);
    if (it != M.end()) {
      std::cout << "    // ";
      for (auto &node : it->second) {
        std::cout << node.kind << ": " << node.code
                  << " -> " << node.explanation << " | ";
      }
    }
    std::cout << "\n";
    L++;
  }
}
