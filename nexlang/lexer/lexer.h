#pragma once
#include <vector>
#include <string>
#include <optional>
#include <memory>

// TokenType and Token struct

enum class TokenType {
    EndOfFile,
    Unknown,
    Identifier,
    Keyword,
    Number,
    String,
    Symbol,
    Operator,
    Comment
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    // Core helpers
    bool isAtEnd() const;
    char peek() const;
    char peekNext() const;
    char advance();
    void skipWhitespace();
    
    // Scanners
    Token scanComment(int line, int col);
    Token scanIdentifierOrKeyword(int line, int col);
    Token scanNumber(int line, int col);
    Token scanString(int line, int col);
    std::optional<std::string> matchOperator();
    Token makeToken(TokenType type, const std::string& lex, int line, int col);
    
    // Members
    std::string src_;
    size_t pos_;
    int line_;
    int col_;
};