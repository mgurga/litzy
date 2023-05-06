#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Litzy.hpp"

int main(int argc, char *argv[]) {
    std::cout << "litzy - pseudocode to lc3 assembly compiler" << std::endl;

    if (argc <= 1 || argc >= 3) {
        std::cout << "incorrect number of arguments." << std::endl
                  << "proper syntax is:" << std::endl;
#if WIN32
        std::cout << "\tlitzy.exe example.lz" << std::endl;
#else
        std::cout << "\tlitzy example.lz" << std::endl;
#endif
        exit(1);
    }

    std::filesystem::path filepath = argv[1];
    std::ifstream f(filepath);
    if (!f) {
        std::cout << "file does not exist." << std::endl;
        return 1;
    }
    std::stringstream ss;
    ss << f.rdbuf();
    std::string input = ss.str();

    Litzy lz;

    std::filesystem::remove("lz.asm");
    std::ofstream out("lz.asm");
    std::string lzout = lz.compile(input);
    out << lzout << std::endl;
    std::cout << lzout << std::endl;
    out.close();

    return 0;
}