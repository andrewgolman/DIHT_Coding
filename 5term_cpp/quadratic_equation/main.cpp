#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <iomanip>
#include "solve_equation.hpp"
#include "../argparse.hpp"
#include "../myio.hpp"


int main(int argc, const char* argv[]) {
    auto args_struct = ArgParser();
    args_struct.add_arg({"-h", "--help"}, "show help message", ArgParserActions::APA_STORE_TRUE);
    args_struct.add_arg(
            {"-s", "--silence"}, "do not show any output but number of roots and roots",
            ArgParserActions::APA_STORE_TRUE
    );
    args_struct.add_arg({"-l", "--logging"}, "enable logging to stderr", ArgParserActions::APA_STORE_TRUE);
    args_struct.parse_args(argc, argv);
    bool show_help = args_struct.get("-h");
    bool be_silent = args_struct.get("-s");
    bool write_log = args_struct.get("-l");

    if (show_help) {
        std::cout << "You know what it does. Command line options are:" << std::endl;
        std::cout << "-h --help : show this message" << std::endl;
        std::cout << "-s --silence : be silent" << std::endl;
        std::cout << "-l --logging : write logs to stderr" << std::endl;
        return 0;
    }
    std::ostream& verbose_stream(enable_stream(not be_silent));
    if (write_log) {
        print_log("Start script");
    }

    double a, b, c;
    verbose_stream << "Quadratic equation by golman@" << std::endl;
    verbose_stream << "Equation: a * x^2 + b * x + c == 0" << std::endl;
    verbose_stream << "Enter a, b, c:" << std::endl;
    a = read_type<double>(write_log);
    b = read_type<double>(write_log);
    c = read_type<double>(write_log);
    auto roots = solve_quadratic_equation(a, b, c);

    if (roots == SE_INF_ROOTS<double>) {
        std::cout << "All real numbers are roots" << std::endl;
    } else {
        verbose_stream << "Number of roots: ";
        std::cout << roots.size() << std::endl;
        for (size_t i = 0; i < roots.size(); ++i) {
            verbose_stream << "Root #" << i + 1 << ": ";
            std::cout << roots[i] << std::endl;
        }
    }

    if (write_log) {
        print_log("Finished successfully");
    }
    return 0;
}
