//
// Created by rzakr on 10.12.2025.
//

#ifndef JSONPARSER_PRINTING_H
#define JSONPARSER_PRINTING_H

#include "Json.h"
#include <ostream>

class Printer: public Json::Visitor{
    std::ostream& os;
    static inline std::string child_prefix = "+-";
    static inline std::string last_child_prefix = "*-";
    static inline std::string child_indent = "|  ";
    static inline std::string last_child_indent = "   ";
    std::vector<std::string> indents {last_child_indent};
    std::vector<std::string> prefixes {last_child_prefix};

    void print_indented(const std::string &str){
        for (auto it = indents.begin(); it != indents.end() - 1; ++it) {
            os << *it;
        }
        os << prefixes.back() << str << std::endl;
    }
    void enter_child(){
        indents.emplace_back(child_indent);
        prefixes.emplace_back(child_prefix);
    }
    void enter_last_child(){
        indents.emplace_back(last_child_indent);
        prefixes.emplace_back(last_child_prefix);
    }
    void leave_child(){
        indents.pop_back();
        prefixes.pop_back();
    }

public:
    Printer(std::ostream& os): os{os}{}
    virtual void operator()(Json::Null&) {
        print_indented("null");
    }
    virtual void operator()(Json::Bool& value) {
        print_indented("Bool: " + std::string((value ? "true" : "false")));
    }
    virtual void operator()(Json::Number& value){
        print_indented("Number: " + std::to_string(value));
    }
    virtual void operator()(Json::String& value){
        print_indented("String: \"" + value + "\"");
    }
    virtual void operator()(Json::Array& obj){
        print_indented("Array");
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (it != obj.end() - 1) {
                enter_child();
            } else {
                enter_last_child();
            }
            (*it).visit(*this);
            leave_child();
        }
    }
    virtual void operator()(Json::Object& obj){
        print_indented("Object");
        auto count = obj.size() - 1;
        for (auto it = obj.begin(); it != obj.end(); ++it, --count) {
            auto& [key, value] = *it;
            if (count) {
                enter_child();
            } else {
                enter_last_child();
            }
            print_indented(key);
            enter_last_child();
            value.visit(*this);
            leave_child();
            leave_child();
        }
    }
};

std::ostream& operator<<(std::ostream& os, const Json& obj) {
    auto visitor = Printer(os);
    obj.visit(visitor);
    return os;
}

#endif //JSONPARSER_PRINTING_H
