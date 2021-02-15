//
// Created by nikcarlson on 03.02.2021.
//

#ifndef FIRSTTASK_LAMBDA_H
#define FIRSTTASK_LAMBDA_H


#include <string>
#include <vector>
#include "Node.h"

class Lambda : virtual public Node {
    Node *node;
    Node *var;
public:
    Lambda(Node *var, Node *node);

    std::string to_str() override;

    Node* get_var();

    Node* get_node();
};


#endif //FIRSTTASK_LAMBDA_H
