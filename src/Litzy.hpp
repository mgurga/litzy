#pragma once

#include <iostream>
#include <string>

#include "Codegen.hpp"
#include "Lexer.hpp"
#include "Scope.hpp"

class Litzy {
public:
    Litzy() {}

    std::string compile(std::string in) {
        Lexer l;
        l.parse(in);

        for (Scope s : l.end_lex) std::cout << s.print() << std::endl;

        Codegen c(l);
        return c.compile();
    };
};