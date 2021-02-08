//
// Created by nikcarlson on 04.02.2021.
//

#include "Variable.h"

Variable::Variable(std::string var): var(std::move(var)) {
}

std::string Variable::to_str() {
    return var;
}
