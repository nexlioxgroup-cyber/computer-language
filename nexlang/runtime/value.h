#pragma once
#include <string>
#include <memory>
#include <variant>
#include <stdexcept>
#include <cmath>

using namespace std;

// ================= VALUE TYPES =================
enum class ValueType {
    STRING,
    FLOAT,
    BOOL,
    HANDLE
};

// Forward declaration for handle type
struct Handle {
    std::string type;      // e.g., "file", "block", "socket"
    void* ptr;             // pointer to the actual resource
    int id;                // unique identifier
    
    Handle(std::string t, void* p, int i) : type(t), ptr(p), id(i) {}
};

// Value object that represents all possible runtime values
class Value {
public:
    Value() : type_(ValueType::STRING), data_(std::string("")) {}
    
    explicit Value(const std::string& s) : type_(ValueType::STRING), data_(s) {}
    explicit Value(double f) : type_(ValueType::FLOAT), data_(f) {}
    explicit Value(bool b) : type_(ValueType::BOOL), data_(b) {}
    explicit Value(Handle h) : type_(ValueType::HANDLE), data_(h) {}
    
    // Type getters
    ValueType getType() const { return type_; }
    
    const std::string& getString() const { 
        return std::get<std::string>(data_); 
    }
    
    double getFloat() const { 
        return std::get<double>(data_); 
    }
    
    bool getBool() const { 
        return std::get<bool>(data_); 
    }
    
    const Handle& getHandle() const { 
        return std::get<Handle>(data_); 
    }
    
    // Implicit conversions
    Value convertTo(ValueType targetType) const {
        if (type_ == targetType) return *this;
        
        switch (targetType) {
            case ValueType::STRING:
                return Value(toString());
            case ValueType::FLOAT:
                return Value(toFloat());
            case ValueType::BOOL:
                return Value(toBool());
            case ValueType::HANDLE:
                // Cannot convert other types to handle
                throw std::runtime_error("Cannot convert to handle type");
        }
        return Value();
    }
    
    // String representation
    std::string toString() const {
        switch (type_) {
            case ValueType::STRING:
                return std::get<std::string>(data_);
            case ValueType::FLOAT:
                return std::to_string(std::get<double>(data_));
            case ValueType::BOOL:
                return std::get<bool>(data_) ? "true" : "false";
            case ValueType::HANDLE:
                return "<handle:" + std::get<Handle>(data_).type + ":" + 
                       std::to_string(std::get<Handle>(data_).id) + ">";
        }
        return "";
    }
    
    // Convert to float
    double toFloat() const {
        switch (type_) {
            case ValueType::STRING:
                try {
                    return std::stod(std::get<std::string>(data_));
                } catch (...) {
                    return 0.0;
                }
            case ValueType::FLOAT:
                return std::get<double>(data_);
            case ValueType::BOOL:
                return std::get<bool>(data_) ? 1.0 : 0.0;
            case ValueType::HANDLE:
                return static_cast<double>(std::get<Handle>(data_).id);
        }
        return 0.0;
    }
    
    // Convert to bool
    bool toBool() const {
        switch (type_) {
            case ValueType::STRING:
                return !std::get<std::string>(data_).empty();
            case ValueType::FLOAT:
                return std::abs(std::get<double>(data_)) > 1e-10;
            case ValueType::BOOL:
                return std::get<bool>(data_);
            case ValueType::HANDLE:
                return std::get<Handle>(data_).ptr != nullptr;
        }
        return false;
    }
    
    // Arithmetic operations
    Value operator+(const Value& other) const {
        if (type_ == ValueType::STRING || other.type_ == ValueType::STRING) {
            // String concatenation
            return Value(this->toString() + other.toString());
        } else {
            // Numeric addition
            return Value(this->toFloat() + other.toFloat());
        }
    }
    
    Value operator-(const Value& other) const {
        return Value(this->toFloat() - other.toFloat());
    }
    
    Value operator*(const Value& other) const {
        return Value(this->toFloat() * other.toFloat());
    }
    
    Value operator/(const Value& other) const {
        double divisor = other.toFloat();
        if (std::abs(divisor) < 1e-10) {
            throw std::runtime_error("Division by zero");
        }
        return Value(this->toFloat() / divisor);
    }
    
    // Comparison operations
    bool operator==(const Value& other) const {
        if (type_ == ValueType::STRING || other.type_ == ValueType::STRING) {
            return this->toString() == other.toString();
        } else {
            return std::abs(this->toFloat() - other.toFloat()) < 1e-10;
        }
    }
    
    bool operator!=(const Value& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Value& other) const {
        if (type_ == ValueType::STRING || other.type_ == ValueType::STRING) {
            return this->toString() < other.toString();
        } else {
            return this->toFloat() < other.toFloat();
        }
    }
    
    bool operator<=(const Value& other) const {
        return *this < other || *this == other;
    }
    
    bool operator>(const Value& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const Value& other) const {
        return !(*this < other);
    }

private:
    ValueType type_;
    std::variant<std::string, double, bool, Handle> data_;
};