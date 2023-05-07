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
                } else if (line.find("{") != std::string::npos &&
                           line.find("if") == std::string::npos) {  // section/function opening
                    std::string sdef = line.substr(0, line.find("{"));
                    std::vector<std::string> spart = Utils::split(sdef, ' ');
                    wscope.returns = Utils::split(spart.at(0), ',');
                    wscope.name = Utils::split(spart.at(1), '(').at(0);
                    inwscope = true;
                } else if (line.find("}") != std::string::npos &&
                           inwscope) {  // section/function closing
                    inwscope = false;
                    end_lex.push_back(wscope);
                    wscope = Scope();
                } else if (line.substr(0, 5) == "print") {
                    add_statement(Statement("Print", {line.substr(6, line.size())}));
                } else if (line.substr(0, 4) == "goto") {
                    add_statement(Statement("GoTo", {line.substr(5, line.size())}));
                } else if (line.substr(0, 6) == "header") {
                    std::vector<std::string> spart = Utils::split(line, ' ');
                    spart.erase(spart.begin());
                    add_statement(Statement("Header", spart));
                } else if (line.substr(0, 3) == "var") {
                    std::vector<std::string> spart = Utils::split(line, ' ');
                    std::string varval = Utils::split(line, '=').at(1);
                    while (varval.at(0) == ' ') varval.erase(0, 1);
                    variables.push_back(spart.at(1));
                    add_statement(Statement("SetVariable", {spart.at(1), varval}));
                } else if (line.back() == ':') {
                    add_statement(Statement("GoToLabel", {line.substr(0, line.size() - 1)}));
                } else if (line.substr(0, 2) == "if" && line.back() == '{') {
                    line = line.substr(3, line.size() - 4);
                    if (line.find("==") != std::string::npos) {
                        std::string lha = line.substr(0, line.find("=="));
                        std::string rha = line.substr(line.find("==") + 2, line.size());

                        // delete trailing spaces
                        while (lha.at(0) == ' ') lha.erase(0, 1);
                        while (rha.at(0) == ' ') rha.erase(0, 1);

                        // delete following spaces
                        while (lha.at(lha.size() - 1) == ' ') lha.pop_back();
                        while (rha.at(rha.size() - 1) == ' ') rha.pop_back();

                        add_statement(Statement("IfEquals", {lha, rha}));
                    }
                } else {
                    // check for variable commands
                    for (std::string v : variables) {
                        if (line.substr(0, v.size()) == v) {
                            line = line.substr(v.size(), line.size() - v.size());
                            if (line == "++") { add_statement(Statement("Increment", {v})); }
                            if (line == "--") { add_statement(Statement("Decrement", {v})); }
                        }
                    }
                }
            } catch (std::exception& e) {
                std::cout << "error parsing line: " << line << std::endl;
                std::cout << "reason: " << e.what() << std::endl;
            }
        }
    }
};