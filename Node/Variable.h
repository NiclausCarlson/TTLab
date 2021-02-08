//
// Created by nikcarlson on 04.02.2021.
//

#ifndef FIRSTTASK_VARIABLE_H
#define FIRSTTASK_VARIABLE_H


#include "Node.h"

class Variable : virtual public Node {
private:
    std::string var;
public:
    Variable(std::string);

    std::string to_str() override;
};


#endif //FIRSTTASK_VARIABLE_H
