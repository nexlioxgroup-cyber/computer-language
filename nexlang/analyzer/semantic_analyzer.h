#pragma once
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include "../perser/ast.h"
#include "../symbol/symbol_table.h"
#include "../runtime/value.h"

using namespace std;

class SemanticAnalyzer {
public:
    SemanticAnalyzer(std::shared_ptr<SymbolTable> symTable) 
        : symbolTable(symTable) {}

    void analyze(std::unique_ptr<ProgramBlock> program) {
        visitProgram(program.get());
    }

private:
    std::shared_ptr<SymbolTable> symbolTable;

    void visitProgram(ProgramBlock* program) {
        symbolTable->enterScope();  // Global scope
        
        for (auto& section : program->sections) {
            if (auto dataBlock = dynamic_cast<DataBlock*>(section.get())) {
                visitDataBlock(dataBlock);
            } else if (auto operationBlock = dynamic_cast<OperationBlock*>(section.get())) {
                visitOperationBlock(operationBlock);
            } else if (auto functionBlock = dynamic_cast<FunctionBlock*>(section.get())) {
                visitFunctionBlock(functionBlock);
            } else if (auto systemCallBlock = dynamic_cast<SystemCallBlock*>(section.get())) {
                visitSystemCallBlock(systemCallBlock);
            } else if (auto executeBlock = dynamic_cast<ExecuteBlockStmt*>(section.get())) {
                visitExecuteBlock(executeBlock);
            }
        }
        
        symbolTable->exitScope();  // Exit global scope
    }

    void visitDataBlock(DataBlock* dataBlock) {
        // Enter data block scope
        symbolTable->enterScope();
        
        for (auto& stmt : dataBlock->statements) {
            visitStatement(stmt.get());
        }
        
        // Exit data block scope
        symbolTable->exitScope();
    }

    void visitOperationBlock(OperationBlock* opBlock) {
        // Define the operation in the symbol table
        symbolTable->defineOperation(opBlock->name, {});
        
        // Enter operation scope
        symbolTable->enterScope();
        
        for (auto& stmt : opBlock->body) {
            visitStatement(stmt.get());
        }
        
        // Exit operation scope
        symbolTable->exitScope();
    }

    void visitFunctionBlock(FunctionBlock* funcBlock) {
        // Define the function in the symbol table
        symbolTable->defineFunction(funcBlock->name, {});
        
        // Enter function scope
        symbolTable->enterScope();
        
        for (auto& stmt : funcBlock->body) {
            visitStatement(stmt.get());
        }
        
        // Exit function scope
        symbolTable->exitScope();
    }

    void visitSystemCallBlock(SystemCallBlock* sysBlock) {
        // Enter system call scope
        symbolTable->enterScope();
        
        for (auto& stmt : sysBlock->body) {
            visitStatement(stmt.get());
        }
        
        // Exit system call scope
        symbolTable->exitScope();
    }

    void visitExecuteBlock(ExecuteBlockStmt* execBlock) {
        // Validate that the block exists
        // In a full implementation, we would check if the target block exists
    }

    void visitStatement(Statement* stmt) {
        if (auto letStmt = dynamic_cast<LetStmt*>(stmt)) {
            visitLetStatement(letStmt);
        } else if (auto sayStmt = dynamic_cast<SayStmt*>(stmt)) {
            visitSayStatement(sayStmt);
        } else if (auto runStmt = dynamic_cast<RunOperationStmt*>(stmt)) {
            visitRunOperationStatement(runStmt);
        } else if (auto ifStmt = dynamic_cast<IfStmt*>(stmt)) {
            visitIfStatement(ifStmt);
        } else if (auto whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
            visitWhileStatement(whileStmt);
        } else if (auto openStmt = dynamic_cast<OpenFileStmt*>(stmt)) {
            visitOpenFileStatement(openStmt);
        } else if (auto readStmt = dynamic_cast<ReadFileStmt*>(stmt)) {
            visitReadFileStatement(readStmt);
        } else if (auto writeStmt = dynamic_cast<WriteFileStmt*>(stmt)) {
            visitWriteFileStatement(writeStmt);
        } else if (auto nowStmt = dynamic_cast<NowStmt*>(stmt)) {
            visitNowStatement(nowStmt);
        } else if (auto doStmt = dynamic_cast<DoStmt*>(stmt)) {
            visitDoStatement(doStmt);
        } else if (auto untilStmt = dynamic_cast<UntilStmt*>(stmt)) {
            visitUntilStatement(untilStmt);
        }
    }

    void visitLetStatement(LetStmt* stmt) {
        // Convert the value string to a Value object
        Value value;
        // Try to parse as number first, otherwise treat as string
        try {
            double numVal = std::stod(stmt->value);
            value = Value(numVal);
        } catch (...) {
            value = Value(stmt->value);
        }
        
        // Define the variable in the current scope
        symbolTable->defineVariable(stmt->name, value);
    }

    void visitSayStatement(SayStmt* stmt) {
        // Check if the message is a known variable or literal
        auto symbol = symbolTable->lookup(stmt->message);
        if (!symbol) {
            // It's a string literal, which is valid
        }
        // Otherwise, it's a variable reference, which is also valid
    }

    void visitRunOperationStatement(RunOperationStmt* stmt) {
        // In a full implementation, we would verify the operation exists
        // For now, just accept it
    }

    void visitIfStatement(IfStmt* stmt) {
        // Enter if block scope
        symbolTable->enterScope();
        
        // Analyze the condition
        // For now, we assume it's valid
        
        // Analyze then body
        for (auto& thenStmt : stmt->thenBody) {
            visitStatement(thenStmt.get());
        }
        
        // Analyze else body if present
        for (auto& elseStmt : stmt->elseBody) {
            visitStatement(elseStmt.get());
        }
        
        // Exit if block scope
        symbolTable->exitScope();
    }

    void visitWhileStatement(WhileStmt* stmt) {
        // Enter while block scope
        symbolTable->enterScope();
        
        // Analyze the condition
        // For now, we assume it's valid
        
        // Analyze the body
        for (auto& bodyStmt : stmt->body) {
            visitStatement(bodyStmt.get());
        }
        
        // Exit while block scope
        symbolTable->exitScope();
    }

    void visitOpenFileStatement(OpenFileStmt* stmt) {
        // Validate file path if needed
        // For now, just accept it
    }

    void visitReadFileStatement(ReadFileStmt* stmt) {
        // Validate file path if needed
        // For now, just accept it
    }

    void visitWriteFileStatement(WriteFileStmt* stmt) {
        // Validate file path and content if needed
        // For now, just accept it
    }

    void visitNowStatement(NowStmt* stmt) {
        // Enter NOW block scope
        symbolTable->enterScope();
        
        // Analyze the body
        for (auto& bodyStmt : stmt->body) {
            visitStatement(bodyStmt.get());
        }
        
        // Exit NOW block scope
        symbolTable->exitScope();
    }

    void visitDoStatement(DoStmt* stmt) {
        // Enter DO block scope
        symbolTable->enterScope();
        
        // Analyze the body
        for (auto& bodyStmt : stmt->body) {
            visitStatement(bodyStmt.get());
        }
        
        // Exit DO block scope
        symbolTable->exitScope();
    }

    void visitUntilStatement(UntilStmt* stmt) {
        // Enter UNTIL block scope
        symbolTable->enterScope();
        
        // Analyze the condition
        // For now, we assume it's valid
        
        // Exit UNTIL block scope
        symbolTable->exitScope();
    }
};