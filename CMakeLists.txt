cmake_minimum_required(VERSION 3.10)
project(ASTLineMapper)

find_package(CURL REQUIRED)
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
find_package(nlohmann_json 3.2.0 REQUIRED) # Ensure nlohmann_json is found

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(ast_mapper
    main.cpp
    ASTMapper.cpp
    OutputFormatter.cpp
)

target_link_libraries(ast_mapper
  clangTooling clangBasic clangASTMatchers clangFrontend clangAST
  CURL::libcurl
  nlohmann_json::nlohmann_json # Link nlohmann_json
)
