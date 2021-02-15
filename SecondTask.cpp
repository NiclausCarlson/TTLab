//
// Created by Nik Carlson on 09.02.2021.
//

#include "Parser/Parser.h"
#include "Unification/Deducer.h"
#include "Node/Lambda.h"

int main() {
    Parser parser;
    char input;
    std::string to_parse;

#ifdef _DEBUG
    freopen("test.in", "r", stdin);
#endif


    input = getchar();
    while (input != -1) {
        if (input == '\n' || input == '\r') to_parse += ' ';
        else to_parse += input;
        input = getchar();
    }
    Node *tree = parser.parse(to_parse);
    Deducer d;
    d.print_proof(tree);
    return 0;
}