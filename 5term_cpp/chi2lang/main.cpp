#include <iostream>
#include <fstream>
#include "string_parser.hpp"
#include "token_parser.hpp"
#include "../myio.hpp"
#include "node.hpp"

int main(int argc, const char* argv[]) {
//    if (argc < 2 || argc > 3) {
//        std::cout << "Usage: ./assembler input_file" << std::endl;
//        return 1;
//    }
//    string input_file = string(argv[1]);
    string output_file = "a.kickass";;
//    if (argc == 3) {
//        output_file = string(argv[2]);
//    }
//    std::cout << "Building " << input_file << "..." << std::endl;

    std::ofstream fout(output_file);

    InitTokenMap();
//    auto buf = read_file_to_buffer(input_file);
    std::string buf = "var a; a := 1; print(a);";
    auto tokens = ParseStringToTokens({buf.begin(), buf.end()});
    auto root = RecursiveParser().Parse(tokens);
    std::cout << root->Compile() << std::endl;
//    std::cout << "Succesfully built " << input_file << ". Written to " << output_file << std::endl;
}

