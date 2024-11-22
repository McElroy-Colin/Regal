#include <variant>
#include <vector>
#include <string>
#include <map>
#include "../inc_parser/parser.hpp"

#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

using String = std::string;

using VarMap = std::map<String, Action>;

std::vector<size_t> number_types = {
    0 /* Integer */
};

#endif