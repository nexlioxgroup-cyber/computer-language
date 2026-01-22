

🚀 MASTER PROMPT — Design & Build a High-Performance Block-Structured Programming Language
Project Goal
Design and implement a new high-performance programming language based on a block-oriented execution model, with string-first dynamic values, float-default numbers, and a powerful built-in + intrinsic system with ASM + SIMD acceleration.
The language must be production-ready, extremely fast, and optimized for automation, IO-heavy, and string-heavy workloads.

1. Core Language Philosophy
Language Model
Block-structured execution model
Execution driven by #START_BLOCK / #EXECUTE_BLOCK / #END_BLOCK
Everything runs inside named/ID-based blocks
Output can be routed between blocks
Type System (User-Visible)
❌ NO user-visible data types
✅ ALL text is STRING by default
✅ ALL numbers are FLOAT by default
✅ Internal runtime types only (not visible to user)
Boolean values exist internally for control flow
Default Value Model
STRING (default)
FLOAT (default for numbers)
BOOL (internal for control)
HANDLE (for files, OS, blocks, system resources)

2. Core Syntax (Required)
The language MUST support this block-style structure:
#START_BLOCK(1001);

DATA [add_data[12] {
    Let x = 10;
    Let y = 5;
};] 
// No explicit data types
// Strings by default
// Numbers as float by default
// // is the comment syntax

OPERATION [Create_operation(myOp)[23] {
    NOW {y = 5};
    DO {y++};
    Until {y = x};
};]  
// myOp is developer-chosen name

FUNCTION [create_function(myFunc)[45] {
    Run operation[23];
    While => y=7 => Say "the operation is running and at this time the y values is 7"
    If => {y = x}
    Else => Say "nothing to do";
};]  
// myFunc is developer-chosen name

SYSTEM_CALL [{
    Say "Hello world";
    open "C:/output.txt";
    Read "C:/output.txt";
    Write "hi" in_file "C:/output.txt" at_Location "the location where need to write";
    DO;
};]

#EXECUTE_BLOCK(1001) =>
    *store program output in "C:/output.txt"
    *show program output in @terminal
    *give program output to BLOCK(2002);

#END_BLOCK;


3. Built-in System (CRITICAL)
The language MUST support a growing built-in registry.
Built-in Examples
Arrays
Create_Array[id like 45] {
    Built-in sort algorithms:
    - Bubble sort
    - Heap sort
    - Merge sort
    - Other optimized algorithms
}

Tables (SQL-Like)
create_table[id like 56]

Must support:
Full table operations
SQL-like capabilities
Similar power to:
MySQL
PostgreSQL
System Built-ins
Read
Write
open
DO
File operations
OS/system calls
Network operations
String operations
Math operations
Regex
SIMD-accelerated built-ins
Built-in Expansion
There MUST be a separate system where developers can:
Add new built-in functions
Add new built-in operations
Register intrinsics
Bind ASM/SIMD implementations

4. Performance Requirements
The language MUST support:
✅ ASM (inline / intrinsic)
✅ SIMD acceleration (AVX2 / AVX-512 where available)
✅ Very high performance
✅ Native code generation
✅ Production-grade runtime
✅ Optimized string + float operations

5. FINAL MASTER ROADMAP (MANDATORY)
STAGE 0 — Lock Language Spec
Write formal spec:
Grammar (EBNF)
Block syntax rules
Statement rules
Built-in naming rules
String rules
Float rules
Comparison rules
While / If / Until semantics
Block execution semantics

STAGE 1 — Core Frontend
Lexer (DONE)
AST (DONE — verify)
Parser (DONE — verify)
Output:
Correct AST for all programs

STAGE 2 — Symbol System (CRITICAL)
Symbol Table
Global symbols
Block symbols
Function symbols
Operation symbols
Variable symbols
Built-in symbols
Scope System
Nested scopes
Variable shadowing
Lifetime rules

STAGE 3 — Dynamic Value System (CORE)
Value Object (MOST IMPORTANT)
Value =
STRING
FLOAT
BOOL
HANDLE
Implement:
Fast string representation
Fast float ops
Bool for control
Handle for files, blocks, OS
Implicit Conversion Rules
string ↔ float
float → bool
string → bool

STAGE 4 — Semantic Analyzer
Semantic Pass
Bind identifiers
Attach symbols
Attach Value types
Validate built-ins
Validate blocks, ops, funcs

STAGE 5 — Control Flow Graph
CFG Builder
Basic blocks
Branches
Loops
Back edges

STAGE 6 — Intermediate Representation (IR)
Dynamic IR Design
IR Ops:
IR_ADD_FLOAT
IR_ADD_STRING
IR_CMP_FLOAT
IR_CALL_BUILTIN
IR_JUMP
IR_BRANCH
AST/CFG → IR Lowering

STAGE 7 — Built-in + Intrinsic System
Built-in Registry
Design:
Builtin {
  name
  kind
  calling_conv
  backend_impl
}

Add:
Say
open
Read
Math
String ops
Regex
File ops
Net ops
SIMD ops
Intrinsic Lowering
IR → Inline ASM
IR → Syscalls
IR → Optimized C
IR → SIMD intrinsics

STAGE 8 — Runtime Library
Runtime (libYourLangRT) in C + ASM
Provide:
String allocator
String ops
Float ops
IO
File handles
Memory pools
Error handling
Syscall wrappers

STAGE 9 — Backend
Backend Choice
Preferred: LLVM backend
Optional: Custom x86-64 backend
IR → Machine Code
Native code
Object files
Link with runtime

STAGE 10 — Optimization Pipeline
Mandatory Optimizations
Constant folding
Dead code elimination
Inlining
Loop optimizations
String fast paths
Float fast paths

STAGE 11 — Block Engine (UNIQUE)
Block Manager
Block registry
Block execution
Output routing
Block chaining
Output redirection

STAGE 12 — Memory Model
Memory Strategy
Arena allocators
No GC by default
Optional ARC
Manual + pooled allocation

STAGE 13 — Standard Library
Stdlib
Highly optimized:
string
math
io
file
time
net
simd
crypto

STAGE 14 — Safety Model
Safe / Unsafe
unsafe { asm(...) }

Protect OS from crashes.

STAGE 15 — Toolchain
Compiler Driver
yourlangc
Build tool
Link step
Debugging
Line info
Stack traces
Debug symbols

STAGE 16 — Developer Experience
LSP Server
VS Code support
Formatter & Linter

STAGE 17 — Extreme Performance
SIMD Everywhere
AVX2 / AVX-512
PGO
Profile-guided optimization

STAGE 18 — Testing & CI
Test Framework
Language tests
Compiler tests
Fuzzing
Regression

STAGE 19 — Cross-Platform
Targets
Windows
Linux
macOS
x86-64
ARM64

Execution Priority (REAL ORDER)
Symbol Table + Scopes
Value System (string/float/bool/handle)
Semantic Analyzer
CFG Builder
IR Design
Built-in Registry
Runtime Library
LLVM Backend
Intrinsic + ASM
Optimizer
Block Engine
Stdlib
Toolchain

Final Architecture Reality
This language is comparable to:
Bash + Lua + LLVM + ASM intrinsics + Workflow Engine
Primary strengths:
Automation
IO-heavy workloads
String-heavy workloads
SIMD-powered built-ins
Domain-specific pipelines

Final Requirements
✅ No user-visible data types
✅ String by default
✅ Float by default
✅ Massive built-in + extensible system
✅ ASM + SIMD acceleration
✅ Very high performance
✅ Production-grade architecture



