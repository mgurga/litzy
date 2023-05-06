#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Lexer.hpp"
#include "Scope.hpp"

class Bindings {
public:
    Bindings() {}

    const std::vector<std::string> registers{"R1", "R2", "R3", "R4", "R5"};

    std::unordered_map<std::string /* variable name */, std::string /* register */> bindings;

    std::string create_binding(std::string varname) {
        if (bindings.size() >= 6) return "";
        bindings[varname] = registers.at(bindings.size());
        return get_var_binding(varname);
    }

    std::string get_var_binding(std::string varname) {
        if (bindings.count(varname) == 0) return "";
        return bindings[varname];
    }

    bool contains_var(std::string varname) {
        for (auto m : bindings)
            if (m.first == varname) return true;
        return false;
    }

    void clear_bindings() { bindings.clear(); }
};

class Codegen {
public:
    Codegen(Lexer l) : lex(l) {}

    std::string indent = "    ";
    std::vector<std::string> labels;
    Lexer lex;
    Bindings b;

    std::string add_label(std::string value);
    std::string create_labels();

    std::string compile() {
        std::string out;
        unsigned sectionnum = 3000;

        for (Scope s : lex.end_lex) {
            // out += s.print() + "\n";
            if (s.statement) {
                out += compute_statement(s.name, s.arguments) + "\n";
            } else {
                out += ".ORIG x" + std::to_string(sectionnum) + "\n";
                out += indent + "LD R6, STACK_ADDR\n";
                out += backup_registers() + "\n";
                for (Statement st : s.statements)
                    out += compute_statement(st.identifier, st.arguments) + "\n";
                out += restore_registers();
                if (s.name == "main")
                    out += indent + "HALT\n\n";
                else
                    out += indent + "RET\n\n";
                out += indent + "NEWLINE .FILL x0A\n";
                out += indent + "STACK_ADDR .FILL xFE00\n";
                out += create_labels();
                labels.clear();
                b.clear_bindings();
                out += "\n.END\n";
                sectionnum += 200;
            }
        }
        return out;
    }

    std::string compute_statement(std::string name, std::vector<std::string> args) {
        std::string out = "";

        if (name == "Print") {
            out += backup();
            std::string printval = args.at(0);
            while (printval.at(0) == ' ') printval.erase(0, 1);
            if (b.contains_var(printval)) {
                out += indent + "ADD R0, " + b.get_var_binding(printval) + ", #0\n";
                out += indent + "; convert " + b.get_var_binding(printval) + " to ascii\n";
                out += indent + "ADD R0, R0, #12\n";
                out += indent + "ADD R0, R0, #12\n";
                out += indent + "ADD R0, R0, #12\n";
                out += indent + "ADD R0, R0, #12\n";
                out += indent + "OUT\n";
            } else {
                out += indent + "LEA R0, " + add_label(printval) + "\n";
                out += indent + "PUTS\n";
            }
            out += restore();
        } else if (name == "SetVariable") {
            std::string reg = b.create_binding(args.at(0));
            out += indent + "LD " + reg + ", " + add_label(args.at(1)) + "\n";
        } else if (name == "Increment") {
            std::string reg = b.get_var_binding(args.at(0));
            out += indent + "ADD " + reg + ", " + reg + ", #1";
        } else if (name == "Decrement") {
            std::string reg = b.get_var_binding(args.at(0));
            out += indent + "ADD " + reg + ", " + reg + ", #-1";
        } else if (name == "GoToLabel") {
            std::string label = args.at(0);
            std::transform(label.begin(), label.end(), label.begin(), ::toupper);
            out += label;
        } else if (name == "GoTo") {
            std::string label = args.at(0);
            std::transform(label.begin(), label.end(), label.begin(), ::toupper);
            out += indent + "BR " + label;
        }

        return out;
    }

    std::string backup(std::string reg = "R0") {
        std::string out = "\n";
        out += indent + "ADD R6, R6, #-1\n";
        out += indent + "STR " + reg + ", R6, #0\n";
        return out;
    }

    std::string restore(std::string reg = "R0") {
        std::string out = "";
        out += indent + "LDR " + reg + ", R6, #0\n";
        out += indent + "ADD R6, R6, #1\n";
        return out;
    }

    std::string backup_registers() {
        std::string out = "";
        std::string decrease_stack = "ADD R6, R6, #-1";
        out += "\n";
        out += indent + decrease_stack + "\n";
        out += indent + "STR R7, R6, #0\n";
        out += indent + decrease_stack + "\n";
        out += indent + "STR R5, R6, #0\n";
        out += indent + decrease_stack + "\n";
        out += indent + "STR R4, R6, #0\n";
        out += indent + decrease_stack + "\n";
        out += indent + "STR R3, R6, #0\n";
        out += indent + decrease_stack + "\n";
        out += indent + "STR R2, R6, #0\n";
        out += indent + decrease_stack + "\n";
        out += indent + "STR R1, R6, #0\n";
        out += indent + decrease_stack + "\n";
        out += indent + "STR R0, R6, #0\n";
        return out;
    }

    std::string restore_registers() {
        std::string out = "";
        std::string increase_stack = "ADD R6, R6, #1";
        out += indent + "LDR R0, R6, #0\n";
        out += indent + increase_stack + "\n";
        out += indent + "LDR R1, R6, #0\n";
        out += indent + increase_stack + "\n";
        out += indent + "LDR R2, R6, #0\n";
        out += indent + increase_stack + "\n";
        out += indent + "LDR R3, R6, #0\n";
        out += indent + increase_stack + "\n";
        out += indent + "LDR R4, R6, #0\n";
        out += indent + increase_stack + "\n";
        out += indent + "LDR R5, R6, #0\n";
        out += indent + increase_stack + "\n";
        out += indent + "LDR R7, R6, #0\n";
        out += indent + increase_stack + "\n";
        return out;
    }
};