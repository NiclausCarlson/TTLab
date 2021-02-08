#include <iostream>
#include "Parser.h"

int main() {
    Parser parser;
    std::string input, to_parse;
    while (std::getline(std::cin, input) && !input.empty()) to_parse += input + ' ' + '\n';
    Node *tree = parser.parse(to_parse);
    std::cout << tree->to_str() << std::endl;
    return 0;
}
