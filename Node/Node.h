//
// Created by nikcarlson on 03.02.2021.
//

#ifndef FIRSTTASK_NODE_H
#define FIRSTTASK_NODE_H

#include "../Unification/Type.h"
#include <string>

struct Node {
    virtual std::string to_str() = 0;
    virtual ~Node() = default;
    Type* type;
};


#endif //FIRSTTASK_NODE_H
