# 🚀 ASTLineMapper

ASTLineMapper is a **Clang-based CLI tool** that visualizes and explains C++ Abstract Syntax Trees (ASTs) by embedding **AI-powered annotations** directly into your source code. Powered by the **Groq API**, this tool helps you understand how the compiler interprets each line of your C++ program.

---

## ✨ Features

- ✅ **Clang Integration**: Uses Clang's AST parsing to identify node types.
- 🤖 **Groq-Powered NLP**: Queries Groq for natural language explanations of AST nodes.
- 🧠 **Single Batching API Call**: Efficiently sends all AST nodes in one request.
- 💬 **Inline Comments**: Adds AI-generated annotations next to source lines.
- 🔍 **Line-by-Line Mapping**: Each source line is paired with its AST structure and description.
- 🛡️ **Robust Error Handling**: Gracefully handles script or API failures.
- 🧾 **Base64 Encoding**: Ensures clean transfer of prompts across CLI boundaries.

---

## 📚 Use Cases

- **Learn ASTs**: Visualize how your C++ code translates into an AST.
- **Debug Compiler Issues**: See what the compiler sees.
- **Code Comprehension**: Understand complex C++ constructs.
- **Teaching Tool**: Great for students learning compilers or parsing.

---

## 🔧 Prerequisites

Make sure the following are installed:

```bash
sudo apt update
sudo apt install clang-18 llvm-18-dev libclang-18-dev nlohmann-json3-dev libcurl4-openssl-dev python3 pip
pip install groq
