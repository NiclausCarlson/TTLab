//
// Created by Nik Carlson on 09.02.2021.
//

#ifndef FIRSTTASK_TYPE_H
#define FIRSTTASK_TYPE_H

#include <string>

struct Type {
    int type_id = -1;
    Type *left, *right;

    explicit Type(int type_id) : type_id(type_id), left(nullptr), right(nullptr) {}

    Type(Type *left, Type *right) : left(left), right(right) {};

    std::string to_str() const {
        if (left == nullptr && right == nullptr) return "t" + std::to_string(type_id);
        return "(" + left->to_str() + " -> " + right->to_str() + ")";
    }

    bool is_var() const {
        return left == nullptr && right == nullptr;
    }
};

#endif //FIRSTTASK_TYPE_H
