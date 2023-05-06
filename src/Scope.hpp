#pragma once

#include <string>
#include <vector>

class Statement {
public:
    std::string identifier;
    std::vector<std::string> arguments;

    Statement(std::string iden) : identifier(identifier){};
    Statement(std::string iden, std::vector<std::string> args) :
        identifier(iden), arguments(args){};

    std::string print() {
        std::string out = identifier;
        for (std::string s : arguments) out += " " + s;
        return out;
    }
};

class Scope {
public:
    Scope(){};
    Scope(std::string name) : name(name) {}
    Scope(Statement s) : statement(true), name(s.identifier), arguments(s.arguments) {}

    std::string name;
    std::vector<std::string> arguments;
    std::vector<std::string> returns;
    std::vector<Statement> statements;
    bool statement = false;

    std::string print() {
        if (statement) {
            std::string out = name;
            for (std::string s : arguments) out += " " + s;
            return out;
        } else {
            std::string out;
            out += "Scope " + name + " begin";
            if (!returns.empty()) {
                out += "( returns: ";
                for (std::string r : returns) out += r + ", ";
                out += ")";
            }
            if (statements.empty()) out += "\n";
            for (Statement s : statements) out += "\n" + s.print();
            out += "\n";
            out += "Scope " + name + " end";
            return out;
        }
    }
};