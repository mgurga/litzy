#pragma once

#include <random>
#include <sstream>
#include <string>
#include <vector>

class Utils {
public:
    static std::vector<std::string> split(std::string s, char split) {
        std::stringstream in(s);
        std::string seg;
        std::vector<std::string> out;
        while (std::getline(in, seg, split)) { out.push_back(seg); }
        return out;
    }

    static std::string random_str(int length) {
        std::string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(alpha.begin(), alpha.end(), generator);
        std::string out;

        for (int i = 0; i < length; i++) {
            out += alpha.at(0);
            std::shuffle(alpha.begin(), alpha.end(), generator);
        }

        return out;
    }

    static void remove_spaces(std::string& s) {
        s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
    }
};