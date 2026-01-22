#include "parser.h"
#include <stdexcept>

// ================= CONSTRUCTOR =================
Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), pos_(0) {}


// ================= TOKEN HELPERS =================
const Token& Parser::peek() {
    return tokens_[pos_];
}

const Token& Parser::advance() {
    return tokens_[pos_++];
}

bool Parser::match(TokenType type, const std::string& lexeme) {
    if (peek().type != type) return false;
    if (!lexeme.empty() && peek().lexeme != lexeme) return false;
    advance();
    return true;
}

void Parser::expect(TokenType type, const std::string& lexeme) {
    if (!match(type, lexeme)) {
        throw std::runtime_error(
            "Parse error near token '" + peek().lexeme +
            "' at line " + std::to_string(peek().line));
    }
}


// ================= ENTRY POINT =================
std::unique_ptr<ProgramBlock> Parser::parseProgram() {
    return parseBlock();
}


// ================= BLOCK =================
std::unique_ptr<ProgramBlock> Parser::parseBlock() {
    expect(TokenType::Keyword, "#START_BLOCK");
    expect(TokenType::Symbol, "(");

    int blockId = std::stoi(advance().lexeme);

    expect(TokenType::Symbol, ")");
    expect(TokenType::Symbol, ";");

    auto program = std::make_unique<ProgramBlock>();
    program->blockId = blockId;

    while (!(peek().type == TokenType::Keyword && (peek().lexeme == "#END_BLOCK" || peek().lexeme == "#EXECUTE_BLOCK"))) {
        // Skip comments and other non-section tokens
        while (peek().type == TokenType::Comment || peek().type == TokenType::Unknown) {
            advance(); // Skip comment or unknown token
            if (match(TokenType::Keyword, "#END_BLOCK")) {
                break; // In case END_BLOCK follows a comment
            }
        }
        
        if (peek().lexeme == "DATA")
            program->sections.push_back(parseData());
        else if (peek().lexeme == "OPERATION")
            program->sections.push_back(parseOperation());
        else if (peek().lexeme == "FUNCTION")
            program->sections.push_back(parseFunction());
        else if (peek().lexeme == "SYSTEM_CALL")
            program->sections.push_back(parseSystemCall());
        else if (peek().type == TokenType::EndOfFile)
            break; // End of file reached
        else
            throw std::runtime_error("Unknown section: " + peek().lexeme);
    }

    // Only expect semicolon if we exited due to #END_BLOCK, not #EXECUTE_BLOCK
    if (peek().type == TokenType::Keyword && peek().lexeme == "#END_BLOCK") {
        expect(TokenType::Symbol, ";");
    }
    
    // Handle #EXECUTE_BLOCK directive
    if (match(TokenType::Keyword, "#EXECUTE_BLOCK")) {
        expect(TokenType::Symbol, "(");
        int execBlockId = std::stoi(advance().lexeme);
        expect(TokenType::Symbol, ")");
        expect(TokenType::Operator, "=>");
        
        auto executeStmt = std::make_unique<ExecuteBlockStmt>();
        executeStmt->blockId = execBlockId;
        
        // Parse output directives
        while (peek().type == TokenType::Symbol && peek().lexeme == "*") {
            match(TokenType::Symbol, "*");
            std::string directive = advance().lexeme;
            std::string fullDirective = "*" + directive;
            
            if (directive == "store") {
                // Skip "program", "output", "in"
                advance(); advance(); advance();
                std::string filename = advance().lexeme;
                fullDirective += " program output in " + filename;
            } else if (directive == "show") {
                // Skip "program", "output", "in"
                advance(); advance(); advance();
                std::string target;
                // Handle @terminal case
                if (peek().type == TokenType::Symbol && peek().lexeme == "@") {
                    advance(); // consume @
                    target = "@" + advance().lexeme; // get terminal
                } else {
                    target = advance().lexeme; // regular target
                }
                fullDirective += " program output in " + target;
            } else if (directive == "give") {
                // Skip "program", "output", "to", "BLOCK"
                advance(); advance(); advance(); advance();
                expect(TokenType::Symbol, "(");
                std::string targetBlock = advance().lexeme;
                expect(TokenType::Symbol, ")");
                fullDirective += " program output to BLOCK(" + targetBlock + ")";
            }
            
            executeStmt->outputs.push_back(fullDirective);
        }
        
        program->sections.push_back(std::move(executeStmt));
    }
    
    // Expect semicolon after #EXECUTE_BLOCK section
    if (peek().type == TokenType::Symbol && peek().lexeme == ";") {
        advance(); // consume the semicolon
    }
    
    expect(TokenType::Keyword, "#END_BLOCK");
    expect(TokenType::Symbol, ";");
    return program;
}


// ================= DATA =================
std::unique_ptr<DataBlock> Parser::parseData() {
    expect(TokenType::Keyword, "DATA");
    expect(TokenType::Symbol, "[");

    std::string name = advance().lexeme;

    expect(TokenType::Symbol, "[");
    int id = std::stoi(advance().lexeme);
    expect(TokenType::Symbol, "]");
    expect(TokenType::Symbol, "{");

    auto data = std::make_unique<DataBlock>();
    data->name = name;
    data->id = id;

    while (!match(TokenType::Symbol, "}")) {
        // Skip semicolons that might appear between statements
        if (peek().type == TokenType::Symbol && peek().lexeme == ";") {
            advance();
            continue;
        }
        data->statements.push_back(parseStatement());
    }

    expect(TokenType::Symbol, ";");
    expect(TokenType::Symbol, "]");
    return data;
}


// ================= OPERATION =================
std::unique_ptr<OperationBlock> Parser::parseOperation() {
    expect(TokenType::Keyword, "OPERATION");
    expect(TokenType::Symbol, "[");

    advance(); // Create_operation
    expect(TokenType::Symbol, "(");
    std::string name = advance().lexeme;
    expect(TokenType::Symbol, ")");
    expect(TokenType::Symbol, "[");

    int id = std::stoi(advance().lexeme);
    expect(TokenType::Symbol, "]");
    expect(TokenType::Symbol, "{");

    auto op = std::make_unique<OperationBlock>();
    op->name = name;
    op->id = id;

    while (!match(TokenType::Symbol, "}")) {
        // Skip semicolons that might appear between statements
        if (peek().type == TokenType::Symbol && peek().lexeme == ";") {
            advance();
            continue;
        }
        op->body.push_back(parseStatement());
    }

    expect(TokenType::Symbol, ";");
    expect(TokenType::Symbol, "]");
    return op;
}


// ================= FUNCTION =================
std::unique_ptr<FunctionBlock> Parser::parseFunction() {
    expect(TokenType::Keyword, "FUNCTION");
    expect(TokenType::Symbol, "[");

    advance(); // create_function
    expect(TokenType::Symbol, "(");
    std::string name = advance().lexeme;
    expect(TokenType::Symbol, ")");
    expect(TokenType::Symbol, "[");

    int id = std::stoi(advance().lexeme);
    expect(TokenType::Symbol, "]");
    expect(TokenType::Symbol, "{");

    auto fn = std::make_unique<FunctionBlock>();
    fn->name = name;
    fn->id = id;

    while (!match(TokenType::Symbol, "}")) {
        // Skip semicolons that might appear between statements
        if (peek().type == TokenType::Symbol && peek().lexeme == ";") {
            advance();
            continue;
        }
        fn->body.push_back(parseStatement());
    }

    expect(TokenType::Symbol, ";");
    expect(TokenType::Symbol, "]");
    return fn;
}


// ================= SYSTEM_CALL =================
std::unique_ptr<SystemCallBlock> Parser::parseSystemCall() {
    expect(TokenType::Keyword, "SYSTEM_CALL");
    expect(TokenType::Symbol, "[");
    expect(TokenType::Symbol, "{");

    auto sys = std::make_unique<SystemCallBlock>();

    while (!match(TokenType::Symbol, "}")) {
        // Skip semicolons that might appear between statements
        if (peek().type == TokenType::Symbol && peek().lexeme == ";") {
            advance();
            continue;
        }
        sys->body.push_back(parseStatement());
    }

    expect(TokenType::Symbol, ";");
    expect(TokenType::Symbol, "]");
    return sys;
}


// ================= STATEMENTS =================
std::unique_ptr<Statement> Parser::parseStatement() {

    // Let x = 10;
    if (match(TokenType::Keyword, "Let")) {
        auto stmt = std::make_unique<LetStmt>();
        stmt->name = advance().lexeme;
        expect(TokenType::Operator, "=");
        stmt->value = advance().lexeme;
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // Say "Hello";
    if (match(TokenType::Keyword, "Say")) {
        auto stmt = std::make_unique<SayStmt>();
        stmt->message = advance().lexeme;
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // Run operation[23];
    if (match(TokenType::Keyword, "Run")) {
        auto stmt = std::make_unique<RunOperationStmt>();
        advance(); // operation
        expect(TokenType::Symbol, "[");
        stmt->operationId = std::stoi(advance().lexeme);
        expect(TokenType::Symbol, "]");
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // If => {condition} => statement
    if (match(TokenType::Keyword, "If") && match(TokenType::Operator, "=>")) {
        auto stmt = std::make_unique<IfStmt>();
        expect(TokenType::Symbol, "{");
        // Parse condition - could be single token or expression
        std::string condition;
        bool first = true;
        while (peek().lexeme != "}" && peek().type != TokenType::EndOfFile) {
            if (!first) {
                condition += " ";
            }
            condition += advance().lexeme;
            first = false;
        }
        stmt->condition = condition;
        
        expect(TokenType::Symbol, "}");
        // Optionally parse then-body
        if (match(TokenType::Operator, "=>")) {
            stmt->thenBody.push_back(parseStatementWithoutSemicolon());
        }
        if (match(TokenType::Keyword, "Else") && match(TokenType::Operator, "=>")) {
            stmt->elseBody.push_back(parseStatementWithoutSemicolon());
        }
        return stmt;
    }

    // While => condition => statement
    if (match(TokenType::Keyword, "While") && match(TokenType::Operator, "=>")) {
        auto stmt = std::make_unique<WhileStmt>();
        
        // Parse condition - could be single token or expression
        std::string condition;
        bool first = true;
        // Collect tokens until we hit the => operator
        while (!(peek().type == TokenType::Operator && peek().lexeme == "=>") && 
               peek().type != TokenType::EndOfFile) {
            if (!first) {
                condition += " ";
            }
            condition += advance().lexeme;
            first = false;
        }
        stmt->condition = condition;
        
        expect(TokenType::Operator, "=>");
        stmt->body.push_back(parseStatementWithoutSemicolon());
        return stmt;
    }

    // open "filename";
    if (match(TokenType::Keyword, "open")) {
        auto stmt = std::make_unique<OpenFileStmt>();
        stmt->filename = advance().lexeme;
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // Read "filename";
    if (match(TokenType::Keyword, "Read")) {
        auto stmt = std::make_unique<ReadFileStmt>();
        stmt->filename = advance().lexeme;
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // Write "content" in_file "filename" at_Location "location";
    if (match(TokenType::Keyword, "Write")) {
        auto stmt = std::make_unique<WriteFileStmt>();
        stmt->content = advance().lexeme;
        expect(TokenType::Keyword, "in_file");
        stmt->filename = advance().lexeme;
        expect(TokenType::Keyword, "at_Location");
        stmt->location = advance().lexeme;
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // NOW { ... };
    if (match(TokenType::Keyword, "NOW")) {
        expect(TokenType::Symbol, "{");
        auto stmt = std::make_unique<NowStmt>();
        // Parse statements inside the braces until we hit the closing brace
        while (peek().type != TokenType::EndOfFile && peek().lexeme != "}") {
            stmt->body.push_back(parseStatementWithoutSemicolon());
        }
        expect(TokenType::Symbol, "}");
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // DO { ... }; or DO;
    if (match(TokenType::Keyword, "DO")) {
        auto stmt = std::make_unique<DoStmt>();
        
        // Check if this is a compound DO statement with braces
        if (match(TokenType::Symbol, "{")) {
            // Parse statements inside the braces until we hit the closing brace
            while (peek().type != TokenType::EndOfFile && peek().lexeme != "}") {
                stmt->body.push_back(parseStatementWithoutSemicolon());
            }
            expect(TokenType::Symbol, "}");
        }
        // Both compound and simple DO statements end with semicolon
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // Until {condition};
    if (match(TokenType::Keyword, "Until")) {
        expect(TokenType::Symbol, "{");
        auto stmt = std::make_unique<UntilStmt>();
        
        // Parse condition - could be single token or expression
        std::string condition;
        bool first = true;
        while (peek().lexeme != "}" && peek().type != TokenType::EndOfFile) {
            if (!first) {
                condition += " ";
            }
            condition += advance().lexeme;
            first = false;
        }
        stmt->condition = condition;
        
        expect(TokenType::Symbol, "}");
        expect(TokenType::Symbol, ";");
        return stmt;
    }

    // Assignment: variable = value;
    if (peek().type == TokenType::Identifier) {
        std::string varName = peek().lexeme;
        // Look ahead to see if this is an assignment
        size_t tempPos = pos_;
        advance(); // consume variable name
        if (match(TokenType::Operator, "=")) {
            auto stmt = std::make_unique<AssignmentStmt>();
            stmt->variable = varName;
            stmt->value = advance().lexeme;  // get the value
            expect(TokenType::Symbol, ";");
            return stmt;
        }
        // Restore position if not an assignment
        pos_ = tempPos;
    }

    // Increment/Decrement: variable++ or variable--;
    if (peek().type == TokenType::Identifier) {
        std::string varName = peek().lexeme;
        // Look ahead to see if this is increment/decrement
        size_t tempPos = pos_;
        advance(); // consume variable name
        if (match(TokenType::Operator, "++")) {
            auto stmt = std::make_unique<IncrementStmt>();
            stmt->variable = varName;
            stmt->isIncrement = true;
            expect(TokenType::Symbol, ";");
            return stmt;
        } else if (match(TokenType::Operator, "--")) {
            auto stmt = std::make_unique<IncrementStmt>();
            stmt->variable = varName;
            stmt->isIncrement = false;
            expect(TokenType::Symbol, ";");
            return stmt;
        }
        // Restore position if not increment/decrement
        pos_ = tempPos;
    }

    throw std::runtime_error("Unknown statement near: " + peek().lexeme);
}

// Parse statement without expecting a semicolon (for use inside braces)
std::unique_ptr<Statement> Parser::parseStatementWithoutSemicolon() {
    // Let x = 10
    if (match(TokenType::Keyword, "Let")) {
        auto stmt = std::make_unique<LetStmt>();
        stmt->name = advance().lexeme;
        expect(TokenType::Operator, "=");
        stmt->value = advance().lexeme;
        return stmt;
    }

    // Say "Hello"
    if (match(TokenType::Keyword, "Say")) {
        auto stmt = std::make_unique<SayStmt>();
        stmt->message = advance().lexeme;
        return stmt;
    }

    // Assignment: variable = value
    if (peek().type == TokenType::Identifier) {
        std::string varName = peek().lexeme;
        // Look ahead to see if this is an assignment
        size_t tempPos = pos_;
        advance(); // consume variable name
        if (match(TokenType::Operator, "=")) {
            auto stmt = std::make_unique<AssignmentStmt>();
            stmt->variable = varName;
            stmt->value = advance().lexeme;  // get the value
            return stmt;
        }
        // Restore position if not an assignment
        pos_ = tempPos;
    }

    // Increment/Decrement: variable++ or variable--
    if (peek().type == TokenType::Identifier) {
        std::string varName = peek().lexeme;
        // Look ahead to see if this is increment/decrement
        size_t tempPos = pos_;
        advance(); // consume variable name
        if (match(TokenType::Operator, "++")) {
            auto stmt = std::make_unique<IncrementStmt>();
            stmt->variable = varName;
            stmt->isIncrement = true;
            return stmt;
        } else if (match(TokenType::Operator, "--")) {
            auto stmt = std::make_unique<IncrementStmt>();
            stmt->variable = varName;
            stmt->isIncrement = false;
            return stmt;
        }
        // Restore position if not increment/decrement
        pos_ = tempPos;
    }

    throw std::runtime_error("Unknown statement near: " + peek().lexeme);
}