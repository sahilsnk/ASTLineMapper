#include "ASTMapper.h"
#include "OutputFormatter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <array>
#include <memory>
#include <nlohmann/json.hpp>
#include <string> // For std::string
#include <vector> // For std::vector

using namespace clang;
using json = nlohmann::json;

// --- START Base64 Encoding Function (add this) ---
// Minimal Base64 encoding (for demonstration, consider a more robust library in production)
std::string base64_encode(const std::string &in) {
    std::string out;
    int val = 0, valb = -6;
    static const std::string b64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    for (char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(b64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back(b64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4) {
        out.push_back('=');
    }
    return out;
}
// --- END Base64 Encoding Function ---


// Helper function to execute shell command and get output
// Helper function to execute shell command and get output
std::string ASTMappingFrontendAction::execPythonScript(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    // ORIGINAL: std::unique_ptr<FILE, decltype(&pclose)> pipe(popen((cmd + " 2>&1").c_str(), "r"), pclose);
    // NEW: Capture ONLY stdout. stderr will go to the terminal directly.
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose); // Removed " 2>&1"
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool ASTMappingVisitor::VisitStmt(Stmt *S) {
  auto &SM = Context->getSourceManager();
  auto Loc = S->getBeginLoc();
  if (!Loc.isValid() || !SM.isWrittenInMainFile(Loc))
    return true;

  unsigned L = SM.getSpellingLineNumber(Loc);
  std::string kind = S->getStmtClassName();
  std::string code = Lexer::getSourceText(
    CharSourceRange::getTokenRange(S->getSourceRange()),
    SM, Context->getLangOpts()).str();

  // Store kind and code. Explanation will be filled later.
  Map[L].push_back({kind, code, ""});
  return true;
}

std::unique_ptr<clang::ASTConsumer> ASTMappingFrontendAction::CreateASTConsumer(
  clang::CompilerInstance &CI, llvm::StringRef) {
  return std::make_unique<ASTMappingConsumer>(&CI.getASTContext(), Map);
}

// ASTMapper.cpp (Changes only in EndSourceFileAction() for prompt generation)

// ... (existing includes and base64_encode function) ...

void ASTMappingFrontendAction::EndSourceFileAction() {
  auto &SM = getCompilerInstance().getSourceManager();

  // 1. Prepare a single prompt with all AST node info
  std::stringstream combinedPromptStream;
  // --- MODIFIED PROMPT TEXT ---
  combinedPromptStream << "Explain the following C++ AST nodes. Provide the explanation in a JSON object with a single key 'explanations', whose value is a JSON array. Each object in this array should have 'kind', 'code', and 'explanation' fields. Ensure the 'kind' and 'code' exactly match the input for easy parsing. Here's the list:\n[\n";

  bool firstNode = true;
  for (auto const& [lineNumber, nodes] : Map) {
      for (const auto& node : nodes) {
          if (!firstNode) {
              combinedPromptStream << ",\n";
          }
          // Use nlohmann::json to properly escape the 'code' string for the inner JSON
          combinedPromptStream << "  {\"kind\": \"" << node.kind << "\", \"code\": " << json(node.code).dump() << "}";
          firstNode = false;
      }
  }
  combinedPromptStream << "\n]\n"; // This ] ends the input array that the model will process
  // --- END MODIFIED PROMPT TEXT ---
  std::string combinedPrompt = combinedPromptStream.str();

  std::string encodedPrompt = base64_encode(combinedPrompt);
  std::string command = "python3 " + std::string("/path/to/groq_script.py") + " \"" + encodedPrompt + "\"";

  //std::cerr << "C++ DEBUG: Executing command: " << command << std::endl;

  std::string rawGroqResponse;
  try {
      rawGroqResponse = execPythonScript(command);
  } catch (const std::exception& e) {
      std::cerr << "Error running Python script: " << e.what() << std::endl;
      for (auto& pair : Map) {
          for (auto& node : pair.second) {
              node.explanation = "[Error querying Groq: " + std::string(e.what()) + "]";
          }
      }
      OutputFormatter::print(SM, Map);
      return;
  }
   // std::cerr << "C++ DEBUG: Raw Python (and potentially Groq) Response: " << rawGroqResponse << std::endl;

  // 3. Parse the unified JSON response
  try {
      json groqJson = json::parse(rawGroqResponse);

      // --- MODIFIED JSON PARSING ---
      if (groqJson.is_object() && groqJson.contains("explanations") && groqJson["explanations"].is_array()) {
          const json& explanationsArray = groqJson["explanations"]; // Get the array
          std::map<std::pair<std::string, std::string>, std::string> explanationsLookup;

          for (const auto& item : explanationsArray) { // Iterate through the array
              if (item.is_object() && item.contains("kind") && item.contains("code") && item.contains("explanation")) {
                  explanationsLookup[{item["kind"].get<std::string>(), item["code"].get<std::string>()}] = item["explanation"].get<std::string>();
              }
          }

          // 4. Distribute explanations back to the Map
          for (auto& pair : Map) {
              for (auto& node : pair.second) {
                  auto it = explanationsLookup.find({node.kind, node.code});
                  if (it != explanationsLookup.end()) {
                      node.explanation = it->second;
                  } else {
                      node.explanation = "[Explanation not found for this node in Groq response (mismatch or missing)]";
                  }
              }
          }
      } else {
          std::cerr << "Groq response is not a valid JSON object with an 'explanations' array. Cannot parse." << std::endl;
          for (auto& pair : Map) {
              for (auto& node : pair.second) {
                  node.explanation = "[Invalid Groq response format (expected object with 'explanations' array)]";
              }
          }
      }
      // --- END MODIFIED JSON PARSING ---

  } catch (const json::parse_error& e) {
      std::cerr << "JSON parse error in Groq response (C++ side): " << e.what() << ". Raw response was: " << rawGroqResponse << std::endl;
      for (auto& pair : Map) {
          for (auto& node : pair.second) {
              node.explanation = "[JSON parsing error (C++): " + std::string(e.what()) + "]";
          }
      }
  } catch (const std::exception& e) {
   //   std::cerr << "Error processing Groq response (C++ side): " << e.what() << std::endl;
      for (auto& pair : Map) {
              for (auto& node : pair.second) {
              node.explanation = "[Error processing Groq response (C++): " + std::string(e.what()) + "]";
          }
      }
  }

  // 5. Print the output
  OutputFormatter::print(SM, Map);
}
