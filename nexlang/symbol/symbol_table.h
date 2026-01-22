#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <stack>
#include <vector>
#include <stdexcept>
#include "../runtime/value.h"

// ================= SYMBOL TYPES =================
enum class SymbolType {
    VARIABLE,
    FUNCTION,
    OPERATION,
    BLOCK,
    BUILTIN
};

// Base symbol class
class Symbol {
public:
    std::string name;
    SymbolType type;
    Value value;
    
    Symbol(const std::string& n, SymbolType t, const Value& v = Value())
        : name(n), type(t), value(v) {}
    virtual ~Symbol() = default;
};

// Variable symbol
class VariableSymbol : public Symbol {
public:
    bool isMutable;  // Let variables are mutable
    
    VariableSymbol(const std::string& n, const Value& v, bool mut = true)
        : Symbol(n, SymbolType::VARIABLE, v), isMutable(mut) {}
};

// Function symbol
class FunctionSymbol : public Symbol {
public:
    std::vector<std::string> paramNames;
    
    FunctionSymbol(const std::string& n, const std::vector<std::string>& params)
        : Symbol(n, SymbolType::FUNCTION), paramNames(params) {}
};

// Operation symbol
class OperationSymbol : public Symbol {
public:
    std::vector<std::string> paramNames;
    
    OperationSymbol(const std::string& n, const std::vector<std::string>& params)
        : Symbol(n, SymbolType::OPERATION), paramNames(params) {}
};

// Block symbol
class BlockSymbol : public Symbol {
public:
    int blockId;
    
    BlockSymbol(const std::string& n, int id)
        : Symbol(n, SymbolType::BLOCK), blockId(id) {}
};

// Built-in symbol
class BuiltInSymbol : public Symbol {
public:
    std::string implementation;
    
    BuiltInSymbol(const std::string& n, const std::string& impl)
        : Symbol(n, SymbolType::BUILTIN), implementation(impl) {}
};

// ================= SCOPE =================
class Scope {
public:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
    std::shared_ptr<Scope> parent;
    
    Scope(std::shared_ptr<Scope> p = nullptr) : parent(p) {}
    
    // Define a symbol in this scope
    void define(const std::string& name, std::shared_ptr<Symbol> symbol) {
        symbols[name] = symbol;
    }
    
    // Look up a symbol (check this scope first, then parent scopes)
    std::shared_ptr<Symbol> lookup(const std::string& name) {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return it->second;
        }
        
        if (parent) {
            return parent->lookup(name);
        }
        
        return nullptr;
    }
    
    // Check if symbol exists in current scope only
    bool hasLocal(const std::string& name) {
        return symbols.find(name) != symbols.end();
    }
};

// ================= SYMBOL TABLE =================
class SymbolTable {
public:
    std::shared_ptr<Scope> currentScope;
    
    SymbolTable() {
        // Create global scope
        currentScope = std::make_shared<Scope>();
        initializeBuiltIns();
    }
    
    // Enter a new scope
    void enterScope() {
        currentScope = std::make_shared<Scope>(currentScope);
    }
    
    // Exit current scope
    void exitScope() {
        if (currentScope->parent) {
            currentScope = currentScope->parent;
        }
    }
    
    // Define a symbol in current scope
    void define(std::shared_ptr<Symbol> symbol) {
        currentScope->define(symbol->name, symbol);
    }
    
    // Look up a symbol in all scopes
    std::shared_ptr<Symbol> lookup(const std::string& name) {
        return currentScope->lookup(name);
    }
    
    // Define a variable
    void defineVariable(const std::string& name, const Value& value, bool isMutable = true) {
        auto var = std::make_shared<VariableSymbol>(name, value, isMutable);
        define(var);
    }
    
    // Define a function
    void defineFunction(const std::string& name, const std::vector<std::string>& paramNames) {
        auto func = std::make_shared<FunctionSymbol>(name, paramNames);
        define(func);
    }
    
    // Define an operation
    void defineOperation(const std::string& name, const std::vector<std::string>& paramNames) {
        auto op = std::make_shared<OperationSymbol>(name, paramNames);
        define(op);
    }
    
    // Define a block
    void defineBlock(const std::string& name, int blockId) {
        auto block = std::make_shared<BlockSymbol>(name, blockId);
        define(block);
    }
    
    // Check if variable exists
    bool hasVariable(const std::string& name) {
        auto sym = lookup(name);
        return sym && sym->type == SymbolType::VARIABLE;
    }
    
    // Update variable value (if mutable)
    bool updateVariable(const std::string& name, const Value& newValue) {
        auto sym = lookup(name);
        if (sym && sym->type == SymbolType::VARIABLE) {
            auto varSym = std::dynamic_pointer_cast<VariableSymbol>(sym);
            if (varSym && varSym->isMutable) {
                varSym->value = newValue;
                return true;
            }
        }
        return false;
    }
    
private:
    // Initialize built-in functions
    void initializeBuiltIns() {
        // Define common built-ins
        define(std::make_shared<BuiltInSymbol>("Say", "print"));
        define(std::make_shared<BuiltInSymbol>("open", "file_open"));
        define(std::make_shared<BuiltInSymbol>("Read", "file_read"));
        define(std::make_shared<BuiltInSymbol>("Write", "file_write"));
        define(std::make_shared<BuiltInSymbol>("DO", "execute"));
    }
};