//
// Created by Nik Carlson on 09.02.2021.
//

#include "Deducer.h"
#include <iostream>

Type *Deducer::create_system(Node *&tree,
                             std::vector<Equation> &system,
                             std::map<std::string, Type *> &expressions_type,
                             std::set<std::string> &used_variables) {
    if (typeid(*tree) == typeid(Variable)) {
        std::string name = tree->to_str();
        int t = 0;
        if (used_variables.count(name) == 0) {
            if (free_variables[name]) t = free_variables[name]->type_id;
            if (t == 0) free_variables[name] = tree->type = new Type(type_idx++);
            else free_variables[name] = tree->type = new Type(t);
        } else {
            if (expressions_type[name]) t = expressions_type[name]->type_id;
            if (t == 0) expressions_type[name] = tree->type = new Type(type_idx++);
            else expressions_type[name] = tree->type = new Type(t);
        }
        return tree->type;
    } else if (typeid(*tree) == typeid(Application)) {
        auto application = dynamic_cast<Application *>(tree);
        std::map<std::string, Type *> left = expressions_type;
        std::map<std::string, Type *> right = expressions_type;
        Type *t1 = create_system(application->get_left(), system, left, used_variables);
        Type *t2 = create_system(application->get_right(), system, right, used_variables);
        for (const auto &i: free_variables) expressions_type[i.first] = i.second;
        Type *type = new Type(t2, new Type(type_idx++));
        expressions_type[application->to_str()] = type->right;
        tree->type = type->right;
        system.emplace_back(t1, type);
        return type->right;
    } else if (typeid(*tree) == typeid(Lambda)) {
        auto lambda = dynamic_cast<Lambda *>(tree);
        auto var = dynamic_cast<Variable *>(lambda->get_var());
        std::string name = var->to_str();
        used_variables.insert(name);
        create_system(lambda->get_var(), system, expressions_type, used_variables);
        Type *tv;
        if (expressions_type[name] == nullptr) expressions_type[name] = new Type(type_idx++);
        tv = expressions_type[name];
        Type *tp = create_system(lambda->get_node(), system, expressions_type, used_variables);
        tree->type = new Type(tv, tp);
        return tree->type;
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
//        for (auto e: system) if (!e.deleted) std::cout << e.left->to_str() << " = " << e.right->to_str() << std::endl;
//        std::cout << std::endl;
        bool something_changed = false;
        std::vector<Equation> new_system;
        for (auto eq = system.begin(); eq != system.end(); ++eq) {
            if (!eq->deleted) {
                if (!eq->left->is_var() && eq->right->is_var()) {
                    std::swap(eq->left, eq->right);
                    something_changed = true;
                } else if ((eq->left->is_var() && eq->right->is_var())
                           && (eq->left->type_id == eq->right->type_id)) {
                    eq->deleted = true;
                } else if (eq->left->is_var() && !eq->right->is_var()
                           || ((eq->left->is_var() && eq->right->is_var()))) {
                    if (eq->left->is_var() && occurs_check(eq->left, eq->right)) return false;
                    if (!eq->used) {
                        for (auto q = system.begin(); q != system.end(); ++q) {
                            if (!q->deleted) {
                                if (q->left->is_var() &&
                                    (q->left->type_id != q->right->type_id && occurs_check(q->left, q->right)))
                                    return false;

                                if (!q->deleted && q != eq) {
                                    if (q->left->is_var() && q->left->type_id == eq->left->type_id) {
                                        q->left = eq->right;
                                        something_changed = true;
                                    } else if (substitution(eq->left, eq->right, q->right))
                                        something_changed = true;
                                }
                            }
                        }
                        eq->used = true;
                        if (something_changed) break;
                    }
                } else if (!eq->left->is_var() && !eq->right->is_var()) {
                    eq->deleted = true;
                    new_system.emplace_back(eq->left->left, eq->right->left);
                    new_system.emplace_back(eq->left->right, eq->right->right);
                    something_changed = true;
                }
            }
        }
        if (!something_changed) break;
        for (auto i: new_system) system.push_back(i);
    }
//    for (auto e: system) if (!e.deleted) std::cout << e.left->to_str() << " = " << e.right->to_str() << std::endl;
//    std::cout << std::endl;
    return true;
}

bool Deducer::substitution(Type *var, Type *&what, Type *&where) {
    if (where->is_var() && where->type_id == var->type_id) {
        where = what;
        return true;
    } else if (!where->is_var()) {
        return substitution(var, what, where->left) |
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
    std::map<std::string, Type *> expressions_type;
    std::set<std::string> used_variables;
    create_system(tree, system, expressions_type, used_variables);
    std::map<int, Type *> types;
    std::vector<Context> free_variables_context;
    std::set<std::string> in_context_checker;
    std::set<std::string> non_free_variables;
    std::vector<Context> new_context;
    if (system.empty()) {
        set_context(free_variables_context, non_free_variables, in_context_checker, types, tree);
        print_proof(free_variables_context, new_context, types, in_context_checker, tree, 0);
    } else if (solver(system)) {
        for (auto eq:system)
            if (!eq.deleted) types[eq.left->type_id] = eq.right;

        set_context(free_variables_context, non_free_variables, in_context_checker, types, tree);
        print_proof(free_variables_context, new_context, types, in_context_checker, tree, 0);
    } else std::cout << "Expression has no type\n";
}

void Deducer::print_proof(std::vector<Context> &free_variables_context,
                          std::vector<Context> &new_context,
                          std::map<int, Type *> &types,
                          std::set<std::string> &in_context_checker,
                          Node *lambda,
                          int level) {
    for (int i = 0; i < level; ++i) std::cout << "*   ";
    print_context(free_variables_context, new_context, in_context_checker);
    std::string name = lambda->to_str();
    std::string type;
    if (types.empty()) type = lambda->type->to_str();
    else {
        s0(lambda->type, types);
        type = lambda->type->to_str();
    }
    if (typeid(*lambda) == typeid(Variable)) {
        std::cout << name << " : " << type << " [rule #1]\n";
    } else if (typeid(*lambda) == typeid(Application)) {
        std::cout << name << " : " << type << " [rule #2]\n";
        auto apl = dynamic_cast<Application *>(lambda);
        print_proof(free_variables_context, new_context, types, in_context_checker, apl->get_left(), level + 1);
        print_proof(free_variables_context, new_context, types, in_context_checker, apl->get_right(), level + 1);
    } else if (typeid(*lambda) == typeid(Lambda)) {
        std::cout << name << " : " << type << " [rule #3]\n";
        auto l = dynamic_cast<Lambda *>(lambda);
        std::vector<Context> tmp_context(new_context);
        s0(l->get_var()->type, types);
        Context c = Context(l->get_var(), l->get_var()->type);
        c.set_type_str();
        tmp_context.push_back(c);
        print_proof(free_variables_context, tmp_context, types, in_context_checker, l->get_node(), level + 1);
    }
}

void Deducer::set_context(std::vector<Context> &context,
                          std::set<std::string> &non_free_variables,
                          std::set<std::string> &in_context_checker,
                          std::map<int, Type *> &types,
                          Node *lambda) {
    if (typeid(*lambda) == typeid(Variable)) {
        std::string var_name = lambda->to_str();
        if (non_free_variables.count(var_name) == 0 && in_context_checker.count(var_name) == 0) {
            in_context_checker.insert(var_name);
            s0(lambda->type, types);
            Context c = Context(lambda, lambda->type);
            c.set_type_str();
            context.push_back(c);
        }
    }
    auto l = dynamic_cast<Lambda *>(lambda);
    if (l != nullptr) {
        non_free_variables.insert(l->get_var()->to_str());
        set_context(context, non_free_variables, in_context_checker, types, l->get_node());
    }
    auto apl = dynamic_cast<Application *>(lambda);
    if (apl != nullptr) {
        set_context(context, non_free_variables, in_context_checker, types, apl->get_left());
        set_context(context, non_free_variables, in_context_checker, types, apl->get_right());
    }
}

void Deducer::print_context(std::vector<Context> &free_variables_context,
                            std::vector<Context> &new_context,
                            std::set<std::string> &in_context_checker) {
    std::set<std::string> used;
    std::string to_print;
    if (!new_context.empty()) {
        for (auto i = new_context.begin(); i != new_context.end() - 1; ++i) {
            to_print = i->tree_str;
            if (in_context_checker.count(to_print)) used.insert(to_print);
            std::cout << to_print + " : " << i->type_str << ", ";
        }
        to_print = (new_context.end() - 1)->tree_str;
        if (in_context_checker.count(to_print)) used.insert(to_print);
        std::cout << to_print << " : " <<
                  (new_context.end() - 1)->type_str;
        if (free_variables_context.empty()) std::cout << " ";
    }
    if (!free_variables_context.empty()) {
        if (!new_context.empty()) std::cout << ", ";
        for (auto i = free_variables_context.begin(); i != free_variables_context.end() - 1; ++i) {
            to_print = i->tree_str;
            if (used.count(to_print) == 0)
                std::cout << to_print << " : " << i->type_str << ", ";
        }
        to_print = (free_variables_context.end() - 1)->tree_str;
        if (used.count(to_print) == 0)
            std::cout << to_print << " : " <<
                      (free_variables_context.end() - 1)->type_str << " ";
    }
    std::cout << "|- ";
}