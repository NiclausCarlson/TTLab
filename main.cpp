#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "Parser.h"

int main() {

    Parser parser;
    char input;
    std::string to_parse;

#ifdef _DEBUG
    freopen("test.in", "r", stdin);
#endif

    std::ios_base::sync_with_stdio(false);
    input = getchar();
    while (input != -1) {
        if (input == '\n' || input == '\r') to_parse += ' ';
        else to_parse += input;
        input = getchar();
    }
    Node *tree = parser.parse(to_parse);
    std::cout << tree->to_str() << std::endl;

    return 0;
}
