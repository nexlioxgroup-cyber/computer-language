#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include "../perser/ast.h"
#include "../runtime/value.h"
#include "../symbol/symbol_table.h"
#include "../builtins/builtins_registry.h"

using namespace std;

// ================= BLOCK EXECUTION CONTEXT =================
struct ExecutionContext {
    std::shared_ptr<SymbolTable> symbolTable;
    std::vector<Value> stack;
    
    ExecutionContext() : symbolTable(std::make_shared<SymbolTable>()) {}
};

// ================= BLOCK ENGINE =================
class BlockEngine {
public:
    BlockEngine() {
        // Initialize the builtins registry
        BuiltinsRegistry::getInstance();
    }
    
    // Execute a program block
    Value executeProgram(std::unique_ptr<ProgramBlock> program) {
        ExecutionContext ctx;
        
        // Process each section in the program
        for (auto& section : program->sections) {
            if (auto dataBlock = dynamic_cast<DataBlock*>(section.get())) {
                executeDataBlock(ctx, dataBlock);
            } else if (auto operationBlock = dynamic_cast<OperationBlock*>(section.get())) {
                executeOperationBlock(ctx, operationBlock);
            } else if (auto functionBlock = dynamic_cast<FunctionBlock*>(section.get())) {
                executeFunctionBlock(ctx, functionBlock);
            } else if (auto systemCallBlock = dynamic_cast<SystemCallBlock*>(section.get())) {
                executeSystemCallBlock(ctx, systemCallBlock);
            } else if (auto executeBlock = dynamic_cast<ExecuteBlockStmt*>(section.get())) {
                executeBlockDirective(ctx, executeBlock);
            }
        }
        
        return Value(); // Return a default value
    }

private:
    void executeDataBlock(ExecutionContext& ctx, DataBlock* dataBlock) {
        // Execute each statement in the data block
        for (auto& stmt : dataBlock->statements) {
            executeStatement(ctx, stmt.get());
        }
    }

    void executeOperationBlock(ExecutionContext& ctx, OperationBlock* opBlock) {
        // Execute each statement in the operation block
        for (auto& stmt : opBlock->body) {
            executeStatement(ctx, stmt.get());
        }
    }

    void executeFunctionBlock(ExecutionContext& ctx, FunctionBlock* funcBlock) {
        // Execute each statement in the function block
        for (auto& stmt : funcBlock->body) {
            executeStatement(ctx, stmt.get());
        }
    }

    void executeSystemCallBlock(ExecutionContext& ctx, SystemCallBlock* sysBlock) {
        // Execute each statement in the system call block
        for (auto& stmt : sysBlock->body) {
            executeStatement(ctx, stmt.get());
        }
    }

    void executeBlockDirective(ExecutionContext& ctx, ExecuteBlockStmt* execBlock) {
        // This would execute the specified block
        // For now, we'll just log it
        std::cout << "Executing block " << execBlock->blockId << std::endl;
        
        // Handle output routing
        for (const auto& output : execBlock->outputs) {
            std::cout << "Output: " << output << std::endl;
        }
    }

    void executeStatement(ExecutionContext& ctx, Statement* stmt) {
        if (auto letStmt = dynamic_cast<LetStmt*>(stmt)) {
            executeLetStatement(ctx, letStmt);
        } else if (auto sayStmt = dynamic_cast<SayStmt*>(stmt)) {
            executeSayStatement(ctx, sayStmt);
        } else if (auto runStmt = dynamic_cast<RunOperationStmt*>(stmt)) {
            executeRunOperationStatement(ctx, runStmt);
        } else if (auto ifStmt = dynamic_cast<IfStmt*>(stmt)) {
            executeIfStatement(ctx, ifStmt);
        } else if (auto whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
            executeWhileStatement(ctx, whileStmt);
        } else if (auto openStmt = dynamic_cast<OpenFileStmt*>(stmt)) {
            executeOpenFileStatement(ctx, openStmt);
        } else if (auto readStmt = dynamic_cast<ReadFileStmt*>(stmt)) {
            executeReadFileStatement(ctx, readStmt);
        } else if (auto writeStmt = dynamic_cast<WriteFileStmt*>(stmt)) {
            executeWriteFileStatement(ctx, writeStmt);
        } else if (auto nowStmt = dynamic_cast<NowStmt*>(stmt)) {
            executeNowStatement(ctx, nowStmt);
        } else if (auto doStmt = dynamic_cast<DoStmt*>(stmt)) {
            executeDoStatement(ctx, doStmt);
        } else if (auto untilStmt = dynamic_cast<UntilStmt*>(stmt)) {
            executeUntilStatement(ctx, untilStmt);
        }
    }

    void executeLetStatement(ExecutionContext& ctx, LetStmt* stmt) {
        // Convert the value string to a Value object
        Value value;
        try {
            double numVal = std::stod(stmt->value);
            value = Value(numVal);
        } catch (...) {
            value = Value(stmt->value);
        }
        
        // Define the variable in the current scope
        ctx.symbolTable->defineVariable(stmt->name, value);
    }

    void executeSayStatement(ExecutionContext& ctx, SayStmt* stmt) {
        // Check if the message is a variable reference or a literal
        auto symbol = ctx.symbolTable->lookup(stmt->message);
        if (symbol) {
            // It's a variable reference
            std::cout << symbol->value.toString() << std::endl;
        } else {
            // It's a string literal
            std::cout << stmt->message << std::endl;
        }
    }

    void executeRunOperationStatement(ExecutionContext& ctx, RunOperationStmt* stmt) {
        // In a full implementation, this would run the specified operation
        // For now, we'll just log it
        std::cout << "Running operation " << stmt->operationId << std::endl;
    }

    void executeIfStatement(ExecutionContext& ctx, IfStmt* stmt) {
        // Evaluate the condition
        bool conditionResult = false;
        
        // For now, we'll assume the condition is a variable name
        auto symbol = ctx.symbolTable->lookup(stmt->condition);
        if (symbol) {
            conditionResult = symbol->value.toBool();
        } else {
            // If it's not a variable, treat it as a string comparison
            conditionResult = !stmt->condition.empty();
        }
        
        if (conditionResult) {
            // Execute the then body
            for (auto& thenStmt : stmt->thenBody) {
                executeStatement(ctx, thenStmt.get());
            }
        } else {
            // Execute the else body
            for (auto& elseStmt : stmt->elseBody) {
                executeStatement(ctx, elseStmt.get());
            }
        }
    }

    void executeWhileStatement(ExecutionContext& ctx, WhileStmt* stmt) {
        // For now, we'll execute the body once
        // In a full implementation, this would loop based on the condition
        for (auto& bodyStmt : stmt->body) {
            executeStatement(ctx, bodyStmt.get());
        }
    }

    void executeOpenFileStatement(ExecutionContext& ctx, OpenFileStmt* stmt) {
        // In a real implementation, this would open a file handle
        // For now, we'll just log it
        std::cout << "Opening file: " << stmt->filename << std::endl;
    }

    void executeReadFileStatement(ExecutionContext& ctx, ReadFileStmt* stmt) {
        // In a real implementation, this would read from a file
        // For now, we'll just log it
        std::cout << "Reading file: " << stmt->filename << std::endl;
    }

    void executeWriteFileStatement(ExecutionContext& ctx, WriteFileStmt* stmt) {
        // In a real implementation, this would write to a file
        // For now, we'll just log it
        std::cout << "Writing to file: " << stmt->filename 
                  << " content: " << stmt->content 
                  << " at location: " << stmt->location << std::endl;
    }

    void executeNowStatement(ExecutionContext& ctx, NowStmt* stmt) {
        // Execute the NOW block statements
        for (auto& bodyStmt : stmt->body) {
            executeStatement(ctx, bodyStmt.get());
        }
    }

    void executeDoStatement(ExecutionContext& ctx, DoStmt* stmt) {
        // Execute the DO block statements
        for (auto& bodyStmt : stmt->body) {
            executeStatement(ctx, bodyStmt.get());
        }
    }

    void executeUntilStatement(ExecutionContext& ctx, UntilStmt* stmt) {
        // For now, we'll just evaluate the condition
        // In a full implementation, this would loop until the condition is met
        auto symbol = ctx.symbolTable->lookup(stmt->condition);
        if (symbol) {
            std::cout << "Until condition evaluated: " << symbol->value.toBool() << std::endl;
        } else {
            std::cout << "Until condition: " << stmt->condition << std::endl;
        }
    }
};