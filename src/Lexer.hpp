#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Scope.hpp"
#include "Utils.hpp"

class Lexer {
public:
    Lexer(){};

    std::vector<Scope> end_lex;

    // this is a terrible implementation. DO NOT IMITATE
    void parse(std::string in) {
        std::istringstream input;
        input.str(in);

        Scope wscope = Scope();  // working scope
        bool inwscope = false;
        std::vector<std::string> variables;

        auto add_statement = [&](Statement s) {
            if (inwscope)
                wscope.statements.push_back(s);
            else
                end_lex.push_back(s);
        };

        for (std::string line; std::getline(input, line);) {
            try {
                std::cout << "parsing line: " << line << std::endl;
                if (line.empty()) continue;
                while (line.at(0) == ' ') line.erase(0, 1);            // consume whitesapce
                if (line.at(0) == '/' && line.at(1) == '/') continue;  // comment line
                if (line.at(0) == ';') {
                    add_statement(Statement("Lc3Comment", {line.substr(0, line.size())}));
                    continue;
                }
                // section/function opening
                if (line.find("{") != std::string::npos && line.find("if") == std::string::npos) {
                    std::string sdef = line.substr(0, line.find("{"));
                    std::vector<std::string> spart = Utils::split(sdef, ' ');
                    wscope.returns = Utils::split(spart.at(0), ',');
                    wscope.name = Utils::split(spart.at(1), '(').at(0);
                    inwscope = true;
                    continue;
                }
                // section/function closing
                if (line.find("}") != std::string::npos) {
                    inwscope = false;
                    end_lex.push_back(wscope);
                    wscope = Scope();
                    continue;
                }
                if (line.substr(0, 5) == "print") {
                    add_statement(Statement("Print", {line.substr(5, line.size())}));
                    continue;
                }
                if (line.substr(0, 6) == "header") {
                    std::vector<std::string> spart = Utils::split(line, ' ');
                    spart.erase(spart.begin());
                    add_statement(Statement("Header", spart));
                }
                if (line.substr(0, 3) == "var") {
                    std::vector<std::string> spart = Utils::split(line, ' ');
                    std::string varval = Utils::split(line, '=').at(1);
                    while (varval.at(0) == ' ') varval.erase(0, 1);
                    variables.push_back(spart.at(1));
                    add_statement(Statement("SetVariable", {spart.at(1), varval}));
                }
                // check for variable commands
                for (std::string v : variables) {
                    if (line.substr(0, v.size()) == v) {
                        line = line.substr(v.size(), line.size() - v.size());
                        if (line == "++") { add_statement(Statement("Increment", {v})); }
                        if (line == "--") { add_statement(Statement("Decrement", {v})); }
                    }
                }
            } catch (std::exception& e) {
                std::cout << "error parsing line: " << line << std::endl;
                std::cout << "reason: " << e.what() << std::endl;
            }
        }
    }
};