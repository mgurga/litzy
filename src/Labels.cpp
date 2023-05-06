#include "Codegen.hpp"

std::string Codegen::add_label(std::string value) {
    while (value.at(0) == ' ') value.erase(0, 1);
    if (value.empty()) return "";
    if (value.at(0) == '\"') {
        std::string compactstr = value;
        compactstr.erase(remove_if(compactstr.begin(), compactstr.end(), ::isspace),
                         compactstr.end());
        compactstr.erase(std::remove(compactstr.begin(), compactstr.end(), '"'), compactstr.end());
        std::transform(compactstr.begin(), compactstr.end(), compactstr.begin(), ::toupper);

        if (compactstr.length() >= 5) {
            labels.push_back(compactstr.substr(0, 5) + " .STRINGZ " + value);
            return compactstr.substr(0, 5);
        } else {
            std::string lname = Utils::random_str(5);
            labels.push_back(lname + " .STRINGZ " + value);
            return lname;
        }
    }
    if (isdigit(value.at(0))) {
        std::string lname = "DEC_" + value;
        labels.push_back(lname + " .FILL #" + value);
        return lname;
    }
    std::cout << "could not create label from " << value << std::endl;
    return "";
}

std::string Codegen::create_labels() {
    std::string out = "";
    for (std::string s : labels) out += indent + s + "\n";
    return out;
}