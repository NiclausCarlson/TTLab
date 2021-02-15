//
// Created by nikcarlson on 04.02.2021.
//

#include "Application.h"

Application::Application(Node *left, Node *right): left(left), right(right) {
}

std::string Application::to_str() {
    return "(" + left->to_str() + " " + right->to_str() + ")";
}

Node *&Application::get_left() {
    return left;
}

Node *&Application::get_right() {
    return right;
}
