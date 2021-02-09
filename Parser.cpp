//
// Created by nikcarlson on 03.02.2021.
//

#include "Parser.h"
#include "Node/Variable.h"
#include "Node/Lambda.h"
#include "Node/Application.h"

void Parser::skip_whitespace() {
    while (idx < expression.size() && isspace(expression[idx])) ++idx;
}

Node *Parser::parse_atom() {
    prev_token = token;
    std::string t = trim(next_token());
    cur_str = t;
    if (token == VARIABLE) return new Variable(t);
    if (token == OPEN_BRACKET) return parse_expression();
    return nullptr;
}

Node *Parser::parse_application() {
    Node *left = parse_atom();
    while (true) {
        Node *right = nullptr;
        if (token == VARIABLE || token == FINISHED_CLOSE_BRACKET) right = parse_atom();
        if (right != nullptr) left = new Application(left, right);
        else return left;
    }
}

Node *Parser::parse_expression() {
    Node *apl = parse_application();
    while (true) {
        if (token == ABSTRACTION && !(prev_token == VARIABLE || prev_token == FINISHED_CLOSE_BRACKET))
            return new Lambda(new Variable(cur_str), parse_expression());
        else if (token == ABSTRACTION && (prev_token == VARIABLE || prev_token == FINISHED_CLOSE_BRACKET))
            return new Application(apl, new Lambda(new Variable(cur_str), parse_expression()));
        if (token == CLOSE_BRACKET) token = FINISHED_CLOSE_BRACKET;
        return apl;
    }
}

Node *Parser::parse(const std::string &expr) {
    this->idx = 0;
    this->expression = expr;
    return parse_expression();
}

std::string Parser::next_token() {
    skip_whitespace();
    size_t prev_idx = idx;
    while (idx < expression.size()) {
        if (expression[idx] == '.') {
            token = ABSTRACTION;
            has_lambda = false;
            if (expression[prev_idx] == '\\') ++prev_idx;
            return expression.substr(prev_idx, ++idx - prev_idx - 1);
        } else if (expression[idx] == '(') {
            if (!isalpha(expression[prev_idx]) && !isdigit(expression[idx] && expression[idx] != '\'')) {
                token = OPEN_BRACKET;
                return expression.substr(prev_idx, idx++ - prev_idx);
            } else {
                token = VARIABLE;
                return expression.substr(prev_idx, idx - prev_idx);
            }
        } else if (expression[idx] == ')') {
            if (!isalpha(expression[prev_idx]) && !isdigit(expression[idx] && expression[idx] != '\'')) {
                token = CLOSE_BRACKET;
                return expression.substr(prev_idx, idx++ - prev_idx);
            } else {
                token = VARIABLE;
                return expression.substr(prev_idx, idx - prev_idx);
            }
        } else {
            if (expression[idx] == '\\') {
                has_lambda = true;
                if (idx != prev_idx) {
                    token = VARIABLE;
                    return expression.substr(prev_idx, idx++ - prev_idx);
                }
            } else if ((!has_lambda && isspace(expression[idx]))
                       || idx + 1 >= expression.size()
                       || (expression[idx] == '\\' && prev_idx < idx)) {
                token = VARIABLE;
                if (idx + 1 >= expression.size()) return expression.substr(prev_idx, idx++ - prev_idx + 1);
                else return expression.substr(prev_idx, idx++ - prev_idx);
            }
        }
        ++idx;
    }
    if (idx >= expression.size()) token = END;
    else token = ERROR;
    return "";
}

Parser::Parser() : token(ERROR), prev_token(ERROR) {
}

std::string Parser::trim(const std::string &t) {
    size_t f = 0, l = 0;
    for (char i : t) {
        if (isspace(i)) ++f;
        else break;
    }
    for (size_t i = f; i < t.size(); ++i) {
        if (!isspace(t[i])) ++l;
        else break;
    }
    return t.substr(f, l);
}
