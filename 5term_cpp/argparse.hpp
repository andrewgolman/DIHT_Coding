#pragma once

#include <vector>
#include <map>

enum class ArgParserActions {
    APA_STORE_TRUE,
    APA_STORE_FALSE
};

//------------------------------------------
//! Handler for command-line bool arguments. One day it will handle it all.
//------------------------------------------
class ArgParser {
    std::map<std::string, bool> args_to_values;
    std::map<std::string, std::string> names_map;
public:
    void add_arg(const std::vector<std::string>& names, const std::string& description, ArgParserActions action) {
        for (auto& name : names) {
            names_map[name] = names[0];
        }
        if (action == ArgParserActions::APA_STORE_TRUE) {
            args_to_values[names[0]] = false;
        }
        if (action == ArgParserActions::APA_STORE_FALSE) {
            args_to_values[names[0]] = true;
        }
    }
    void parse_args(int argc, const char* argv[]) {
        for (size_t i = 1; i < argc; ++i) { // todo: checks only arguments presence, should be rewritten in the bright future
            try {
                std::string arg_name = names_map.at(argv[i]);
                args_to_values.at(arg_name) ^= true;
            }
            catch (const std::out_of_range& exception) {
                throw std::runtime_error("Unexpected argument");
            }
        }
    }
    bool get(const std::string& s) const {
        return args_to_values.at(s);
    }
};
