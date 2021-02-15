//
// Created by Nik Carlson on 09.02.2021.
//

#include "Deducer.h"
#include <iostream>

Type *Deducer::create_system(Node *tree, std::vector<Equation> &system) {
    if (typeid(*tree) == typeid(Variable)) {
        auto var = dynamic_cast<Variable *>(tree);
        std::string name = var->to_str();
        int t = 0;
        if (variables_type[name]) t = variables_type[name]->type_id;
        if (t == 0) {
            expressions_type[name] = variables_type[name] = new Type(type_idx);
            return new Type(type_idx++);
        } else {
            Type *type = new Type(t);
            expressions_type[name] = type;
            return type;
        }
    } else if (typeid(*tree) == typeid(Application)) {
        auto application = dynamic_cast<Application *>(tree);
        Type *t1 = create_system(application->get_left(), system);
        Type *t2 = create_system(application->get_right(), system);
        Type *type = new Type(t2, new Type(type_idx));
        expressions_type[application->to_str()] = type->right;
        system.emplace_back(t1, type);
        return new Type(type_idx++);
    } else if (typeid(*tree) == typeid(Lambda)) {
        auto lambda = dynamic_cast<Lambda *>(tree);
        auto var = dynamic_cast<Variable *>(lambda->get_var());
        std::string name = var->to_str();
        Type *t1 = create_system(lambda->get_node(), system);
        int t = 0;
        Type *type;
        if (variables_type[name]) t = variables_type[name]->type_id;
        if (t != 0) {
            Type *free_var_type = new Type(t);
            type = new Type(free_var_type, t1);
            expressions_type[name] = free_var_type;
            expressions_type[lambda->to_str()] = type;
            return type;
        }
        expressions_type[name] = variables_type[name] = new Type(type_idx);
        type = new Type(new Type(type_idx++), t1);
        expressions_type[lambda->to_str()] = type;
        return type;
    }
    return nullptr;
}

bool Deducer::solver(std::vector<Equation> &system) {
    std::function<bool(Type *, Type *)> occurs_check = [&occurs_check](Type *var, Type *t) {
        if (!t->is_var()) {
            bool left = occurs_check(var, t->left);
            bool right = occurs_check(var, t->right);
            return left || right;
        }
        return var->type_id == t->type_id;
    };
    while (true) {
        bool something_changed = false;
        std::vector<Equation> new_system;
        for (auto eq = system.begin(); eq != system.end(); ++eq) {
            if (!eq->deleted) {
                if (!eq->left->is_var() && eq->right->is_var()) {
                    std::swap(eq->left, eq->right);
                    something_changed = true;
                } else if (eq->left->is_var() && !eq->right->is_var()
                           || ((eq->left->is_var() && eq->right->is_var())
                               && (eq->left->type_id != eq->right->type_id))) {
                    if (occurs_check(eq->left, eq->right)) return false;
                    for (auto q = system.begin(); q != system.end(); ++q) {
                        if (q->left->is_var() && occurs_check(q->left, q->right)) return false;
                        if (!q->deleted && q != eq) {
                            if (q->left->is_var() && q->left->type_id == eq->left->type_id) {
                                q->left = eq->right;
                                something_changed = true;
                            } else if (substitution(eq->left, eq->right, q->right)) something_changed = true;
                        }
                    }
                    if (something_changed) break;
                } else if ((eq->left->is_var() && eq->right->is_var())
                           && (eq->left->type_id != eq->right->type_id)) {
                    eq->deleted = true;
                } else if (!eq->left->is_var() && !eq->right->is_var()) {
                    eq->deleted = true;
                    new_system.emplace_back(eq->left->left, eq->right->left);
                    new_system.emplace_back(eq->right->left, eq->right->right);
                    something_changed = true;
                }
            }
        }
        if (!something_changed) break;
        for (auto i:new_system) system.push_back(i);
    }
    return true;
}

bool Deducer::substitution(Type *var, Type *&what, Type *&where) {
    if (where->is_var() && where->type_id == var->type_id) {
        where = what;
        return true;
    } else if (!where->is_var()) {
        return substitution(var, what, where->left) ||
               substitution(var, what, where->right);
    }
    return false;
}

void Deducer::s0(Type *&t, std::map<int, Type *> &eq) {
    if (!t->is_var()) {
        s0(t->left, eq);
        s0(t->right, eq);
    }
    int idx = t->type_id;
    if (eq.count(idx) != 0) t = eq[idx];
}

void Deducer::print_proof(Node *tree) {
    std::vector<Equation> system;
    Type *t = create_system(tree, system);
    std::map<int, Type *> types;
    std::vector<Context> free_variables_context;
    std::set<std::string> in_context_checker;
    std::set<std::string> non_free_variables;
    std::vector<Context> new_context;
    if (system.empty()) {
        set_context(free_variables_context, non_free_variables, in_context_checker, tree);
        print_proof(free_variables_context, new_context, in_context_checker, tree, 0);
    } else if (solver(system)) {
        for (auto eq:system) if (!eq.deleted) types[eq.left->type_id] = eq.right;
        for (auto p: variables_type) {
            s0(p.second, types);
            variables_type[p.first] = p.second;
        }
        for (auto p: expressions_type) {
            s0(p.second, types);
            expressions_type[p.first] = p.second;
        }
        set_context(free_variables_context, non_free_variables, in_context_checker, tree);
        print_proof(free_variables_context, new_context, in_context_checker, tree, 0);
    } else std::cout << "Expression has no type\n";
}

void Deducer::print_proof(std::vector<Context> &free_variables_context,
                          std::vector<Context> &new_context,
                          std::set<std::string> &in_context_checker,
                          Node *lambda,
                          int level) {
    for (int i = 0; i < level; ++i) std::cout << "*   ";
    print_context(free_variables_context, new_context);
    std::string name = lambda->to_str();
    if (typeid(*lambda) == typeid(Variable)) {
        std::cout << name << " : " << expressions_type[name]->to_str() << " [rule #1]\n";
    } else if (typeid(*lambda) == typeid(Application)) {
        std::cout << name << " : " << expressions_type[name]->to_str() << " [rule #2]\n";
        auto apl = dynamic_cast<Application *>(lambda);
        print_proof(free_variables_context, new_context, in_context_checker, apl->get_left(), level + 1);
        print_proof(free_variables_context, new_context, in_context_checker, apl->get_right(), level + 1);
    } else if (typeid(*lambda) == typeid(Lambda)) {
        std::cout << name << " : " << expressions_type[name]->to_str() << " [rule #3]\n";
        auto abstr = dynamic_cast<Lambda *>(lambda);
        std::vector<Context> tmp_context;
        for (auto i: new_context) tmp_context.push_back(i);
        std::string var_name = abstr->get_var()->to_str();
        tmp_context.emplace_back(abstr->get_var(), expressions_type[var_name]);
        print_proof(free_variables_context, tmp_context, in_context_checker, abstr->get_node(), level + 1);
    }
}

void Deducer::set_context(std::vector<Context> &context,
                          std::set<std::string> &non_free_variables,
                          std::set<std::string> &in_context_checker,
                          Node *lambda) {
    if (typeid(*lambda) == typeid(Variable)) {
        std::string var_name = lambda->to_str();
        if (non_free_variables.count(var_name) == 0) {
            in_context_checker.insert(var_name);
            context.emplace_back(lambda, variables_type[var_name]);
        }
    }
    auto l = dynamic_cast<Lambda *>(lambda);
    if (l != nullptr) {
        non_free_variables.insert(l->get_var()->to_str());
        set_context(context, non_free_variables, in_context_checker, l->get_node());
    }
    auto apl = dynamic_cast<Application *>(lambda);
    if (apl != nullptr) {
        set_context(context, non_free_variables, in_context_checker, apl->get_left());
        set_context(context, non_free_variables, in_context_checker, apl->get_right());
    }
}

void Deducer::print_context(std::vector<Context> &free_variables_context,
                            std::vector<Context> &new_context) {
    if (!new_context.empty()) {
        for (auto i = new_context.begin(); i != new_context.end() - 1; ++i)
            std::cout << i->tree->to_str() + " : " << i->type->to_str() << ", ";
        std::cout << (new_context.end() - 1)->tree->to_str() << " : " <<
                  (new_context.end() - 1)->type->to_str();
        if (free_variables_context.empty()) std::cout << " ";
    }
    if (!free_variables_context.empty()) {
        if (!new_context.empty()) std::cout << ", ";
        for (auto i = free_variables_context.begin(); i != free_variables_context.end() - 1; ++i)
            std::cout << i->tree->to_str() << " : " << i->type->to_str() << ", ";
        std::cout << (free_variables_context.end() - 1)->tree->to_str() << " : " <<
                  (free_variables_context.end() - 1)->type->to_str() << " ";
    }
    std::cout << "|- ";
}
