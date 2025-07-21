ASTLineMapper
A Clang-based tool that visualizes C++ Abstract Syntax Trees (ASTs) by embedding explanations of AST nodes directly into your source code, powered by the Groq API.

🚀 Overview
ASTLineMapper is a powerful development tool that leverages the Clang compiler's AST capabilities and the Groq AI API to provide on-the-fly explanations of your C++ code's structure. It processes your C++ source file, identifies Abstract Syntax Tree nodes, queries Groq for concise explanations of each node's role, and then outputs your code with these explanations as inline comments.

This tool is invaluable for:

Learning C++ ASTs: Understand how your code is parsed into an AST.

Debugging Compiler Errors: Gain insight into the compiler's perspective of your code.

Code Comprehension: Get quick explanations of complex C++ constructs.

Educational Purposes: A hands-on way to explore compiler internals.

✨ Features
Clang Integration: Utilizes Clang's robust AST parsing capabilities.

Groq API Powered Explanations: Sends AST node information to Groq for intelligent, contextual explanations.

Single AI Request: Optimizes API usage by batching all AST node information into a single request to Groq.

Inline Code Comments: Embeds explanations directly into the generated source code output.

Line-by-Line Mapping: Explanations are mapped back to their original source code lines.

Robust Error Handling: Includes mechanisms to catch and report issues from Python script execution, JSON parsing, and API failures.

Base64 Encoding: Safely transmits complex data (like C++ code snippets) via command-line arguments to the Python script.

🛠️ Prerequisites
Before you begin, ensure you have the following installed:

CMake (version 3.10 or higher)

LLVM & Clang development libraries (Version 18 or compatible is recommended. On Ubuntu, llvm-18-dev and libclang-18-dev).

sudo apt install clang-18 llvm-18-dev libclang-18-dev

nlohmann/json C++ library: A header-only JSON library.

sudo apt install nlohmann-json3-dev (On Debian/Ubuntu)

Alternatively, you can manually include it or use CMake's FetchContent.

libcurl development libraries: For HTTP requests (used by nlohmann_json's internal fetching if not system-installed, and useful generally).

sudo apt install libcurl4-openssl-dev

Python 3

groq Python library:

pip install groq

Groq API Key: You'll need an API key from Groq Cloud.

🚀 Getting Started
Follow these steps to build and run ASTLineMapper:

1. Clone the Repository
Bash

git clone https://github.com/your-username/ASTLineMapper.git # Replace with your actual repo URL
cd ASTLineMapper
2. Configure Groq API Key
Open groq_script.py and replace "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" with your actual Groq API key:

Python

# groq_script.py
client = Groq(api_key="YOUR_GROQ_API_KEY_HERE")
Security Note: Never commit your API keys directly into your repository in a production environment. For personal projects, this might be acceptable, but for larger projects, consider using environment variables.

3. Build the Project
Bash

mkdir build
cd build
cmake ..
make
4. Run the Tool
Once built, you can run ast_mapper on any C++ source file.

Bash

./ast_mapper <your_source_file.cpp> --
Example:

Let's assume you have a file named my_code.cpp:

C++

// my_code.cpp
int main() {
    int x = 10;
    if (x > 5) {
        return x;
    }
    return 0;
}
Run the tool:

Bash

./ast_mapper my_code.cpp --
Expected Output (with explanations):

1: int main(){    // FunctionDecl: main -> A declaration of the main function, the entry point of the program. | CompoundStmt: {
int x = 10;
if (x > 5) {
    return x;
}
return 0;} -> A compound statement, representing a block of code enclosed in curly braces. |
2:     int x = 10; // DeclStmt: int x = 10; -> A declaration statement that declares an integer variable 'x' and initializes it to 10. | IntegerLiteral: 10 -> An integer literal representing the value 10. |
3:     if (x > 5) { // IfStmt: if (x > 5) {
    return x;
} -> An if statement, which conditionally executes a block of code if the given condition is true. | BinaryOperator: x > 5 -> A binary operator performing a greater-than comparison between 'x' and '5'. | DeclRefExpr: x -> A reference to a declared variable 'x'. | IntegerLiteral: 5 -> An integer literal representing the value 5. | CompoundStmt: {
    return x;
} -> A compound statement, representing a block of code enclosed in curly braces. |
4:         return x; // ReturnStmt: return x -> A return statement, which exits the current function and returns the value of 'x' to the caller. | DeclRefExpr: x -> A reference to a declared variable 'x'. |
5:     }
6:     return 0; // ReturnStmt: return 0 -> A return statement, which exits the current function and returns the value 0 to the caller. | IntegerLiteral: 0 -> An integer literal representing the value 0. |
7: }
📂 Project Structure
main.cpp: Entry point for the Clang Tooling application.

ASTMapper.h, ASTMapper.cpp: Contains the core logic for traversing the AST, collecting node information, performing Base64 encoding/decoding, interacting with the Python script, and parsing the AI response.

OutputFormatter.h, OutputFormatter.cpp: Handles printing the source code with embedded explanations.

groq_script.py: The Python script responsible for calling the Groq API and returning the AI-generated explanations.

CMakeLists.txt: CMake build configuration for the project.

🤝 Contributing
Contributions are welcome! If you have suggestions for improvements, bug fixes, or new features, please feel free to:

Fork the repository.

Create a new branch (git checkout -b feature/your-feature-name).

Make your changes.

Commit your changes (git commit -m 'feat: Add new feature').

Push to the branch (git push origin feature/your-feature-name).

Create a Pull Request.

📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
