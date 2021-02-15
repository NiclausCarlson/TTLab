//
// Created by nikcarlson on 04.02.2021.
//

#ifndef FIRSTTASK_APPLICATION_H
#define FIRSTTASK_APPLICATION_H

#include "Node.h"

class Application : virtual public Node {
private:
    Node *left, *right;
public:
    Application(Node *left, Node *right);

    std::string to_str() override;

    Node *get_left();

    Node *get_right();
};


#endif //FIRSTTASK_APPLICATION_H
