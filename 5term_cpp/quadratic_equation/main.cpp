#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <iomanip>
#include "solve_equation.hpp"
#include "../argparse.hpp"
#include "../myio.hpp"


int main(int argc, const char* argv[]) {
    auto args_struct = ArgParser();  // todo constructor of init list
    args_struct.add_arg(
            {"-h", "--help"}, "show help message",
            ArgParserActions::STORE_TRUE);
    args_struct.add_arg(
            {"-s", "--silence"}, "do not show any output but number of roots and roots",
            ArgParserActions::STORE_TRUE
    );
    args_struct.add_arg({"-l", "--logging"}, "enable logging to stderr", ArgParserActions::STORE_TRUE);
    args_struct.parse_args(argc, argv);
    bool show_help = args_struct.get("-h");
    bool be_silent = args_struct.get("-s");
    bool write_log = args_struct.get("-l");

    if (show_help) {
        std::cout << args_struct.generate_help("You know what it does.");
        return 0;
    }
    std::ostream& verbose_stream(enable_stream(not be_silent));
    if (write_log) {  // not on streams, on buffers
        print_log("Start script");
    }

    verbose_stream << "Quadratic equation by golman@" << std::endl;
    verbose_stream << "Equation: a * x^2 + b * x + c == 0" << std::endl;
    verbose_stream << "Enter a, b, c:" << std::endl;
    double a = 0, b = 0, c = 0;
    try {
        a = read_double();
        b = read_double();
        c = read_double();
    }
    catch(std::runtime_error&) {
        if (write_log) {
            print_log(BAD_INPUT_MSG);
        }
        throw;
    }

    auto roots = solve_quadratic_equation(a, b, c);
    if (roots == SE_INF_ROOTS<double>) {
        std::cout << "All real numbers are roots" << std::endl;
    } else {
        verbose_stream << "Number of roots: ";
        std::cout << std::get<0>(roots) << std::endl;
        if (std::get<0>(roots) >= 1) {
            verbose_stream << "Root #1: ";
            std::cout << std::get<1>(roots) << std::endl;
        }
        if (std::get<0>(roots) >= 2) {
            verbose_stream << "Root #2: ";
            std::cout << std::get<2>(roots) << std::endl;
        }
    }
    if (write_log) {
        print_log("Finished successfully");
    }
    return 0;
}
