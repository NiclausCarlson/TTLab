//
// Created by nikcarlson on 03.02.2021.
//

#ifndef FIRSTTASK_PARSER_H
#define FIRSTTASK_PARSER_H

#include<string>

#include"Node/Node.h"

class Parser {
private:
    enum Token {
        ABSTRACTION,
        OPEN_BRACKET,
        CLOSE_BRACKET,
        VARIABLE,
        FINISHED_CLOSE_BRACKET,
        ERROR,
        END
    };

    std::string get_token(Token t) {
        switch (t) {
            case ABSTRACTION:
                return "ABSTRACTION";
            case OPEN_BRACKET:
                return "OPEN_BRACKET";
            case CLOSE_BRACKET:
                return "CLOSE_BRACKET";
            case VARIABLE:
                return "VARIABLE";
            case FINISHED_CLOSE_BRACKET:
                return "FINISHED_CLOSE_BRACKET";
            case ERROR:
                return "ERROR";
            case END:
                return "END";
        }
    }

    static std::string trim(const std::string& t);

    size_t idx{};
    std::string expression;
    std::string cur_str;
    Token token, prev_token;

    void skip_whitespace();

    std::string next_token();

    Node *parse_atom();

    Node *parse_application();

    Node *parse_expression();

public:
    Parser();

    Node *parse(const std::string &);
};


#endif //FIRSTTASK_PARSER_H
