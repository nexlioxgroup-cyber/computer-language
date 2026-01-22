// lexer.cpp implementation file
#include "lexer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>
#include <optional>

// ================= KEYWORDS =================

static const std::unordered_set<std::string> kKeywords = {
    "#START_BLOCK",
    "#END_BLOCK",
    "#EXECUTE_BLOCK",

    "DATA",
    "OPERATION",
    "FUNCTION",
    "SYSTEM_CALL",

    "Let",
    "NOW",
    "DO",
    "Until",
    "Run",
    "If",
    "Else",
    "While",
    "Say",
    "open",
    "Read",
    "Write",
    "in_file",
    "at_Location",
    "DO",

    "Create_operation",
    "create_function"
};

// ================= OPERATORS =================

static const std::vector<std::string> kOperators = {
    "=>",
    "==",
    "++",
    "--",
    "=",
    "+",
    "-",
    "*",
    "/",
    "%"    
};

// ================= SYMBOLS =================

static const std::unordered_set<char> kSymbols = {
    '(', ')', '{', '}', '[', ']', ';', ',', '*', '@', '.', '/'
};

// Constructor
Lexer::Lexer(std::string source)
    : src_(std::move(source)), pos_(0), line_(1), col_(1) {}

// Tokenize method
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;

        int startLine = line_;
        int startCol  = col_;
        char c = peek();

        // ---------- COMMENT ----------
        if (c == '/' && peekNext() == '/') {
            tokens.push_back(scanComment(startLine, startCol));
        }
        // ---------- STRING ----------
        else if (c == '"') {
            tokens.push_back(scanString(startLine, startCol));
        }
        // ---------- NUMBER ----------
        else if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(scanNumber(startLine, startCol));
        }
        // ---------- IDENTIFIER / KEYWORD ----------
        else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '#') {
            tokens.push_back(scanIdentifierOrKeyword(startLine, startCol));
        }
        // ---------- OPERATOR ----------
        else if (auto op = matchOperator()) {
            tokens.push_back(makeToken(TokenType::Operator, *op, startLine, startCol));
        }
        // ---------- SYMBOL ----------
        else if (kSymbols.count(c)) {
            std::string lex(1, advance());
            tokens.push_back(makeToken(TokenType::Symbol, lex, startLine, startCol));
        }
        // ---------- UNKNOWN ----------
        else {
            std::string bad(1, advance());
            tokens.push_back(makeToken(TokenType::Unknown, bad, startLine, startCol));
        }
    }

    tokens.push_back(makeToken(TokenType::EndOfFile, "", line_, col_));
    return tokens;
}

// ---------- CORE HELPERS ----------

bool Lexer::isAtEnd() const { return pos_ >= src_.size(); }

char Lexer::peek() const { return isAtEnd() ? '\0' : src_[pos_]; }

char Lexer::peekNext() const { 
    if (pos_ + 1 >= src_.size()) return '\0'; 
    return src_[pos_ + 1]; 
}

char Lexer::advance() {
    char c = src_[pos_++];
    if (c == '\n') {
        line_++;
        col_ = 1;
    } else {
        col_++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(static_cast<unsigned char>(peek()))) {
        advance();
    }
}

// ---------- SCANNERS ----------

Token Lexer::scanComment(int line, int col) {
    advance(); // consume first /
    advance(); // consume second /
    std::string lex;
    
    while (!isAtEnd() && peek() != '\n') {
        lex.push_back(advance());
    }
    
    return makeToken(TokenType::Comment, lex, line, col);
}

Token Lexer::scanIdentifierOrKeyword(int line, int col) {
    std::string lex;
    while (!isAtEnd()) {
        char c = peek();
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '#') {
            lex.push_back(advance());
        } else break;
    }

    if (kKeywords.count(lex)) {
        return makeToken(TokenType::Keyword, lex, line, col);
    }
    return makeToken(TokenType::Identifier, lex, line, col);
}

Token Lexer::scanNumber(int line, int col) {
    std::string lex;
    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        lex.push_back(advance());
    }
    return makeToken(TokenType::Number, lex, line, col);
}

Token Lexer::scanString(int line, int col) {
    advance(); // consume opening quote
    std::string lex;

    while (!isAtEnd()) {
        char c = advance();
        if (c == '"') {
            return makeToken(TokenType::String, lex, line, col);
        }
        if (c == '\n') break;
        lex.push_back(c);
    }
    return makeToken(TokenType::Unknown, lex, line, col);
}

std::optional<std::string> Lexer::matchOperator() {
    for (const auto& op : kOperators) {
        if (src_.compare(pos_, op.size(), op) == 0) {
            for (size_t i = 0; i < op.size(); ++i) advance();
            return op;
        }
    }
    return std::nullopt;
}

Token Lexer::makeToken(TokenType type, const std::string& lex, int line, int col) {
    return Token{type, lex, line, col};
}
