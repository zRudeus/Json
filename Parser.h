//
// Created by rzakr on 10.12.2025.
//

#ifndef JSONPARSER_PARSER_H
#define JSONPARSER_PARSER_H

#include "Json.h"
#include <istream>

class ParseError{};
Json parse(std::istream & input);

#endif //JSONPARSER_PARSER_H
