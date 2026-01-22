#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>
#include "../runtime/value.h"

using namespace std;

// ================= BUILTIN TYPES =================
enum class BuiltinKind {
    SAY,
    OPEN_FILE,
    READ_FILE,
    WRITE_FILE,
    MATH_OP,
    STRING_OP,
    REGEX_OP,
    FILE_OP,
    NETWORK_OP,
    SIMD_OP
};

// Function signature for builtin functions
using BuiltinFunction = std::function<Value(const std::vector<Value>&)>;

// ================= BUILTIN DEFINITION =================
struct Builtin {
    std::string name;
    BuiltinKind kind;
    int argCount;  // -1 for variable arguments
    BuiltinFunction implementation;
    
    Builtin(const std::string& n, BuiltinKind k, int args, BuiltinFunction impl)
        : name(n), kind(k), argCount(args), implementation(impl) {}
};

// ================= BUILTINS REGISTRY =================
class BuiltinsRegistry {
public:
    static BuiltinsRegistry& getInstance() {
        static BuiltinsRegistry instance;
        return instance;
    }
    
    // Register a builtin function
    void registerBuiltin(const std::string& name, BuiltinKind kind, 
                        int argCount, BuiltinFunction implementation) {
        builtins_[name] = std::make_unique<Builtin>(name, kind, argCount, implementation);
    }
    
    // Get a builtin function
    Builtin* getBuiltin(const std::string& name) {
        auto it = builtins_.find(name);
        if (it != builtins_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // Check if a name is a builtin
    bool isBuiltin(const std::string& name) {
        return builtins_.find(name) != builtins_.end();
    }
    
    // Initialize all builtins
    void initializeBuiltins() {
        // SAY builtin
        registerBuiltin("Say", BuiltinKind::SAY, 1, [](const std::vector<Value>& args) -> Value {
            if (!args.empty()) {
                std::cout << args[0].toString() << std::endl;
            }
            return Value();  // Return empty value
        });
        
        // OPEN_FILE builtin
        registerBuiltin("open", BuiltinKind::OPEN_FILE, 1, [](const std::vector<Value>& args) -> Value {
            if (!args.empty()) {
                // In a real implementation, this would open a file handle
                // For now, we'll simulate it
                std::string filename = args[0].toString();
                Handle fileHandle("file", nullptr, 1);  // Simulated file handle
                return Value(fileHandle);
            }
            return Value();
        });
        
        // READ_FILE builtin
        registerBuiltin("Read", BuiltinKind::READ_FILE, 1, [](const std::vector<Value>& args) -> Value {
            if (!args.empty()) {
                // In a real implementation, this would read from a file handle
                // For now, we'll return a dummy string
                return Value("<file_contents>");
            }
            return Value();
        });
        
        // WRITE_FILE builtin
        registerBuiltin("Write", BuiltinKind::WRITE_FILE, -1, [](const std::vector<Value>& args) -> Value {
            if (args.size() >= 3) {
                // In a real implementation, this would write to a file
                // args[0]: content, args[1]: filename, args[2]: location
                // For now, we'll just return success
                return Value(true);
            }
            return Value(false);
        });
        
        // Math operations
        registerBuiltin("Add", BuiltinKind::MATH_OP, 2, [](const std::vector<Value>& args) -> Value {
            if (args.size() >= 2) {
                return args[0] + args[1];
            }
            return Value(0.0);
        });
        
        registerBuiltin("Subtract", BuiltinKind::MATH_OP, 2, [](const std::vector<Value>& args) -> Value {
            if (args.size() >= 2) {
                return args[0] - args[1];
            }
            return Value(0.0);
        });
        
        registerBuiltin("Multiply", BuiltinKind::MATH_OP, 2, [](const std::vector<Value>& args) -> Value {
            if (args.size() >= 2) {
                return args[0] * args[1];
            }
            return Value(0.0);
        });
        
        registerBuiltin("Divide", BuiltinKind::MATH_OP, 2, [](const std::vector<Value>& args) -> Value {
            if (args.size() >= 2) {
                return args[0] / args[1];
            }
            return Value(0.0);
        });
        
        // String operations
        registerBuiltin("Concat", BuiltinKind::STRING_OP, 2, [](const std::vector<Value>& args) -> Value {
            if (args.size() >= 2) {
                return args[0] + args[1];  // Uses Value's + operator for concatenation
            }
            return Value("");
        });
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Builtin>> builtins_;
    
    // Private constructor for singleton
    BuiltinsRegistry() {
        initializeBuiltins();
    }
    
    // Delete copy constructor and assignment operator
    BuiltinsRegistry(const BuiltinsRegistry&) = delete;
    BuiltinsRegistry& operator=(const BuiltinsRegistry&) = delete;
};