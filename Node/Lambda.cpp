//
// Created by nikcarlson on 03.02.2021.
//

#include "Lambda.h"

Lambda::Lambda(Node* var, Node *node) : var(var), node(node) {
}

std::string Lambda::to_str() {
    std::string str = "(\\" + var->to_str() + ".";
    str += node->to_str() + ")";
    return str;
}




