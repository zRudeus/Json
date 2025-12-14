//
// Created by rzakr on 10.12.2025.
//

#ifndef JSONPARSER_PRINTING_H
#define JSONPARSER_PRINTING_H

#include "Json.h"
#include <ostream>

std::ostream& operator<<(std::ostream& os, const Json& obj);

#endif //JSONPARSER_PRINTING_H
