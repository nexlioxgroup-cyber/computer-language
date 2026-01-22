#pragma once
#include <vector>
#include <memory>
#include <string>

#include "../lexer/lexer.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::unique_ptr<ProgramBlock> parseProgram();

    // Added to support #EXECUTE_BLOCK
    std::unique_ptr<ExecuteBlockStmt> parseExecuteBlockDirective();

private:
    // ---------- Token helpers ----------
    const Token& peek();
    const Token& advance();
    bool match(TokenType type, const std::string& lexeme = "");
    void expect(TokenType type, const std::string& lexeme = "");

    // ---------- Grammar rules ----------
    std::unique_ptr<ProgramBlock> parseBlock();
    std::unique_ptr<DataBlock> parseData();
    std::unique_ptr<OperationBlock> parseOperation();
    std::unique_ptr<FunctionBlock> parseFunction();
    std::unique_ptr<SystemCallBlock> parseSystemCall();

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseStatementWithoutSemicolon();  // For statements inside braces

private:
    const std::vector<Token>& tokens_;
    size_t pos_;
};