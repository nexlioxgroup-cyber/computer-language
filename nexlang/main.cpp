#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "lexer/lexer.h"
#include "perser/parser.h"
#include "analyzer/semantic_analyzer.h"
#include "engine/block_engine.h"

int main(int argc, char** argv) {
    // Get the input file path from command line arguments
    const char* filePath = (argc > 1) ? argv[1] : "example/MYcode_syntax.nex";
    
    // Read the source code from the file
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filePath << "'" << std::endl;
        return 1;
    }
    
    std::string sourceCode((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "=== NexLang Compiler ===" << std::endl;
    std::cout << "Parsing file: " << filePath << std::endl;
    
    try {
        // 1. Lexical Analysis
        std::cout << "\n--- LEXICAL ANALYSIS ---" << std::endl;
        Lexer lexer(sourceCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens generated: " << tokens.size() << std::endl;
        
        // Uncomment the following lines to see all tokens
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            std::cout << "  " << i << ": ";
            switch (token.type) {
                case TokenType::Keyword:     std::cout << "KEYWORD"; break;
                case TokenType::Identifier:  std::cout << "IDENT"; break;
                case TokenType::Number:      std::cout << "NUMBER"; break;
                case TokenType::String:      std::cout << "STRING"; break;
                case TokenType::Symbol:      std::cout << "SYMBOL"; break;
                case TokenType::Operator:    std::cout << "OP"; break;
                case TokenType::Comment:     std::cout << "COMMENT"; break;
                case TokenType::EndOfFile:   std::cout << "EOF"; break;
                default:                     std::cout << "UNKNOWN"; break;
            }
            std::cout << " '" << token.lexeme << "' at " << token.line << ":" << token.column << std::endl;
        }
        
        
        // 2. Parsing
        std::cout << "\n--- SYNTAX ANALYSIS ---" << std::endl;
        Parser parser(tokens);
        auto program = parser.parseProgram();
        std::cout << "Abstract Syntax Tree generated successfully!" << std::endl;
        
        // 3. Semantic Analysis
        std::cout << "\n--- SEMANTIC ANALYSIS ---" << std::endl;
        auto symbolTable = std::make_shared<SymbolTable>();
        SemanticAnalyzer analyzer(symbolTable);
        analyzer.analyze(std::move(program));
        std::cout << "Semantic analysis completed!" << std::endl;
        
        // Reparse to have fresh AST for execution
        Parser execParser(tokens);
        auto execProgram = execParser.parseProgram();
        
        // 4. Execution
        std::cout << "\n--- EXECUTION ---" << std::endl;
        BlockEngine engine;
        Value result = engine.executeProgram(std::move(execProgram));
        std::cout << "Program execution completed!" << std::endl;
        
        std::cout << "\n=== Compilation Successful ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during compilation: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}