//
// Created by Nik Carlson on 09.02.2021.
//

#ifndef FIRSTTASK_DEDUCER_H
#define FIRSTTASK_DEDUCER_H


#include <vector>
#include <map>
#include <set>
#include <functional>

#include "../Node/Node.h"
#include "../Node/Application.h"
#include "../Node/Lambda.h"
#include "../Node/Variable.h"
#include "Equation.h"

class Deducer {
private:
    struct Context {
        Context(Node *tree, Type *type) : tree(tree), type(type) {}

        Node *tree;
        Type *type;
    };

    int type_idx = 1;

    Type *create_system(Node *tree,
                        std::vector<Equation> &system,
                        std::map<std::string, Type *>& expressions_type);

    bool solver(std::vector<Equation> &system);

    bool substitution(Type *var, Type *&what, Type *&where);

    void s0(Type *&t, std::map<int, Type *> &eq);

    void print_proof(std::vector<Context> &free_variables_context,
                     std::vector<Context> &new_context,
                     Node *lambda,
                     int level);

    void set_context(std::vector<Context> &context,
                     std::set<std::string> &non_free_variables,
                     std::set<std::string> &in_context_checker,
                     Node *lambda);

    static void print_context(std::vector<Context> &free_variables_context, std::vector<Context> &new_context);

public:
    void print_proof(Node *tree);
};


#endif //FIRSTTASK_DEDUCER_H
