#pragma once

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <sstream>

enum class ArgParserActions {
    STORE_TRUE,
    STORE_FALSE
};

struct BoolArg {
    std::string description;
    bool value;
    std::vector<std::string> names;

    BoolArg(const std::string& description_, ArgParserActions action_, const std::vector<std::string>& names_)
                : names(names_), description(description_) {
        if (action_ == ArgParserActions::STORE_TRUE) {
            value = 0;
        } else if (action_ == ArgParserActions::STORE_FALSE) {
            value = 1;
        }  // exceptions should be thrown from constructors, maybe ArgParserActions should be kept from adding other options
    }
    std::string help() const {
        std::ostringstream ss;
        for (std::string name : names) {
            ss << name << " ";
        }
        ss << "\t" << description;
        return ss.str();
    }
};

//------------------------------------------
//! Handler for command-line bool arguments. One day it will handle it all.
//------------------------------------------
class ArgParser {
    std::map<std::string, std::shared_ptr<BoolArg>> names_map;

public:
    void add_arg(const std::vector<std::string>& names, const std::string& description, ArgParserActions action) {
        auto new_arg_ptr = std::make_shared<BoolArg>(description, action, names);
        for (auto& name : names) {
            names_map[name] = new_arg_ptr;
        }
    }
    void parse_args(int argc, const char* argv[]) {
        for (size_t i = 1; i < argc; ++i) { // todo: only for bool args, should be rewritten in the bright future
            try {
                names_map.at(argv[i])->value ^= true;
            }
            catch (const std::out_of_range& exception) {
                throw std::runtime_error("Unexpected argument");
            }
        }
    }
    bool get(const std::string& s) const {
        return names_map.at(s)->value;
    }
    std::string generate_help(const std::string& description) const {
        std::ostringstream ss;
        ss << description << std::endl;;
        ss << "Options:" << std::endl;
        std::set<std::string> sorted_list_arghelps;
        for (auto pair : names_map) {
            sorted_list_arghelps.insert(pair.second->help());
        }
        for (auto arg_help_str : sorted_list_arghelps) {
            ss << arg_help_str << std::endl;
        }
        return ss.str();
    }
};
