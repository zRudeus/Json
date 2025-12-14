//
// Created by rzakr on 14.12.2025.
//
#include "Parser.h"
using namespace JsonDecls;

std::string buff;
std::size_t offset;

void skip(){
    static constexpr std::string_view spaces =" \n\r\t";
    while(offset < buff.length() && spaces.contains(buff[offset])) offset++;
}
void match(std::string_view pattern){
    skip();
    if (std::string_view(buff).substr(offset, pattern.length()) == pattern) offset += pattern.length();
    else throw ParseError{};
}
char next(){
    skip();
    if (offset < buff.length()) return buff[offset];
    throw ParseError{};
}
Json parseJson();

Null parseNull(){
    match("null");
    return Null();
}
Bool parseBool(){
    if (next() == 't') {
        match("true");
        return Bool(true);
    } else {
        match("false");
        return Bool(false);
    }
}
Number parseNumber(){
    static constexpr std::string_view digits ="0123456789";
    std::size_t begin = offset;
    std::size_t size = 0;
    auto read = [&]{
        while(offset < buff.length() && digits.contains(buff[offset])) {
            size++;
            offset++;
        }
    };

    read();
    if (offset < buff.length() && buff[offset] == '.'){
        size++;
        offset++;
        read();
    }
    return stod(buff.substr(begin, size));
}
String parseString(){
    match("\"");
    std::size_t begin = offset;
    std::size_t size = 0;
    while(offset < buff.length() && buff[offset] != '\"') {
        size++;
        offset++;
    }
    match("\"");
    return buff.substr(begin, size);
}
Array parseArray(){
    match("[");
    Array result;
    if (next() != ']') {
        result.emplace_back(parseJson());
        while (next() == ',') {
            match(",");
            result.emplace_back(parseJson());
        }
    }
    match("]");
    return std::move(result);
}
Object parseObject(){
    match("{");
    Object result;
    auto match_record = [&]{
        auto key = parseString();
        match(":");
        auto value = parseJson();
        result.emplace(std::move(key), std::move(value));
    };
    if (next() != '}') {
        match_record();
        while (next() == ',') {
            match(",");
            match_record();
        }
    }
    match("}");
    return result;
}

Json parseJson(){
    switch (next()){
        case 'n':   return parseNull();
        case 't':   return parseBool();
        case 'f':   return parseBool();
        case '\"':  return parseString();
        case '{':   return parseObject();
        case '[':   return parseArray();
        default:    return parseNumber();
    }
}

Json parse(std::istream &input) {
    buff.clear();
    offset = 0;
    std::string line;
    while (std::getline(input, line)) buff += line;
    auto res = parseJson();
    skip();
    if (offset != buff.length()) throw ParseError();
    return res;
}
