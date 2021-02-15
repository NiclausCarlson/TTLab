//
// Created by Nik Carlson on 09.02.2021.
//

#ifndef FIRSTTASK_EQUATION_H
#define FIRSTTASK_EQUATION_H

#include "Type.h"

struct Equation {
    Type *left, *right;
    bool deleted = false;
    bool used = false;
    Equation(Type *left, Type *right) : left(left), right(right) {};
};

#endif //FIRSTTASK_EQUATION_H
