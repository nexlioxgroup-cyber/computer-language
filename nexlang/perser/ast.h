#pragma once
#include <string>
#include <vector>
#include <memory>

// ================= BASE NODE =================
struct ASTNode {
    virtual ~ASTNode() = default;
};

// ================= STATEMENTS =================
struct Statement : ASTNode {};

struct LetStmt : Statement {
    std::string name;
    std::string value;
};

struct SayStmt : Statement {
    std::string message;
};

struct RunOperationStmt : Statement {
    int operationId;
};

struct IfStmt : Statement {
    std::string condition;
    std::vector<std::unique_ptr<Statement>> thenBody;
    std::vector<std::unique_ptr<Statement>> elseBody;
};

struct WhileStmt : Statement {
    std::string condition;
    std::vector<std::unique_ptr<Statement>> body;
};

struct OpenFileStmt : Statement {
    std::string filename;
};

struct ReadFileStmt : Statement {
    std::string filename;
};

struct WriteFileStmt : Statement {
    std::string content;
    std::string filename;
    std::string location;
};

struct NowStmt : Statement {
    std::vector<std::unique_ptr<Statement>> body;
};

struct DoStmt : Statement {
    std::vector<std::unique_ptr<Statement>> body;
};

struct UntilStmt : Statement {
    std::string condition;
    std::vector<std::unique_ptr<Statement>> body;
};

struct AssignmentStmt : Statement {
    std::string variable;
    std::string value;  // This could be a variable name or literal
};

struct IncrementStmt : Statement {
    std::string variable;
    bool isIncrement;  // true for ++, false for --
};

struct ExecuteBlockStmt : Statement {
    int blockId;
    std::vector<std::string> outputs; // How to handle the outputs
};

// ================= SECTIONS =================
struct DataBlock : ASTNode {
    std::string name;
    int id;
    std::vector<std::unique_ptr<Statement>> statements;
};

struct OperationBlock : ASTNode {
    std::string name;
    int id;
    std::vector<std::unique_ptr<Statement>> body;
};

struct FunctionBlock : ASTNode {
    std::string name;
    int id;
    std::vector<std::unique_ptr<Statement>> body;
};

struct SystemCallBlock : ASTNode {
    std::vector<std::unique_ptr<Statement>> body;
};

// ================= ROOT PROGRAM =================
struct ProgramBlock : ASTNode {
    int blockId;
    std::vector<std::unique_ptr<ASTNode>> sections;
};
