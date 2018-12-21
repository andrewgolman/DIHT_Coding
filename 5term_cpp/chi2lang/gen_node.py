MIN_PRIOR = -5
MAX_PRIOR = 10


class Struct:
    def __init__(self, name, token, post_cmds=(), pre_cmds=(), mid_cmds=(), priority=None, right_assoc=False,
                 pass_state_by_value=(False, False), code=None):
        self.name = name
        self.token = token
        self.priority = priority
        self.associativity = right_assoc

        self.pre_cmds = pre_cmds
        self.mid_cmds = mid_cmds
        self.post_cmds = post_cmds

        self.pass_state_by_value = pass_state_by_value
        self.code = code

        self.normalize_cmds(pre_cmds)
        self.normalize_cmds(mid_cmds)
        self.normalize_cmds(post_cmds)

    def normalize_cmds(self, cmds):
        for i in range(len(cmds)):
            cmds[i] = cmds[i].replace("M1", '" + mark1 + "')
            cmds[i] = cmds[i].replace("M2", '" + mark2 + "')
            cmds[i] = cmds[i].replace("L1", '" + last_mark + "')

    def declare(self):
        return """
        struct {}Node;
        """.format(self.name).strip()

    def gen_compile(self):
        iftype = f"\tif (type == TokenType::{self.name.upper()}) {{\n\t\t\t"
        lines = list()
        if self.code:
            lines.append(self.code)
        counter = 0
        for cmd in self.pre_cmds:
            lines.append(f'std::string s{counter} = "{cmd}\\n";')
            counter += 1
        lines.append(f'std::string s{counter} = left ? left->Compile{"Copy" if self.pass_state_by_value[0] else ""}(state) : "";')
        counter += 1
        for cmd in self.mid_cmds:
            lines.append(f'std::string s{counter} = "{cmd}\\n";')
            counter += 1
        lines.append(
            f'std::string s{counter} = right ? right->Compile{"Copy" if self.pass_state_by_value[0] else ""}(state) : "";')
        counter += 1
        for cmd in self.post_cmds:
            lines.append(f'std::string s{counter} = "{cmd}\\n";')
            counter += 1
        lines.append('return {};'.format(' + '.join(f"s{i}" for i in range(counter))))
        return iftype + "\n\t\t\t".join(lines) + "\n\t\t}"


class NonStructToken:
    def __init__(self, name, token):
        self.name = name
        self.token = token


structs = [
    Struct("Const", None, [
        'set r8 " + std::to_string(value) + "',
        "push r1",
    ], priority="MAX_PRIOR"),
    Struct("Name", None, [
        'load r1 " + std::to_string(state.var_values[value]) + "',
        "push r1"
    ], priority="MAX_PRIOR"),
    Struct("Plus", "+", [
        "pop r2",
        "pop r1",
        "add r1 r2",
        "push r1"
    ], priority=6),
    Struct("Minus", "-", [
        "pop r2",
        "pop r1",
        "sub r1 r2",
        "push r1"
    ], priority=6),
    Struct("Mult", "*", [
        "pop r2",
        "pop r1",
        "mul r1 r2",
        "push r1"
    ], priority=8),
    Struct("Div", "/", [
        "pop r2",
        "pop r1",
        "div r1 r2",
        "push r1"
    ], priority=8),
    Struct("Mod", "%", [
        "pop r2",
        "pop r1",
        "mod r1 r2",
        "push r1"
    ], priority=7),
    Struct("LAnd", "&", [
        "pop r2",
        "pop r1",
        "and r1 r2",
        "push r1"
    ], priority=3),
    Struct("LOr", "|", [
        "pop r2",
        "pop r1",
        "or r1 r2",
        "push r1"
    ], priority=1),
    Struct("Xor", "^", [
        "pop r2",
        "pop r1",
        "xor r1 r2",
        "push r1"
    ], priority=2),
    Struct("Less", "<", [
        "pop r2",
        "pop r1",
        "less r1 r2",
        "jmpncmp M1",
        "set r1 1",
        "jmp M2",
        "M1",
        "set r1 0",
        "M2",
        "push r1"
    ], priority=5),
    Struct("Eq", "=", [
        "pop r2",
        "pop r1",
        "eq r1 r2",
        "jmpncmp M1",
        "set r1 1",
        "jmp M2",
        "M1",
        "set r1 0",
        "M2",
        "push r1"
    ], priority=4),
    Struct("Assign", ":=", mid_cmds=[
        "pop r2",
        "pop r1",
        'set r3 " + std::to_string(value) + "',
        "store r3 r2",
        "push r2"
    ], right_assoc=True, priority=0),
    Struct("If", "if", [
    ]),
    Struct("If_Left", None, mid_cmds=[
        "pop r1",
        "set r2 1",
        "cmp r1 r2",
        "jmpncmp M1",
    ], post_cmds=[
        "jmp M2",
        "M1"
    ], pass_state_by_value=(False, True)),
    Struct("If_Right", None, mid_cmds=[
        "jmp M1",
        "L1",
    ], post_cmds=[
        "M1",
    ], pass_state_by_value=(True, False)),
    Struct("While", "while", [
    ]),
    Struct("While_Left", None, pre_cmds=["M1"], mid_cmds=[
        "pop r1",
        "set r2 1",
        "cmp r1 r2",
        "jmpcmp M2",
    ], post_cmds=[
        "jmp M1"
        "M2",
    ], pass_state_by_value=(False, True)),
    Struct("While_Right", None, [

    ]),
    Struct("Def", "def", pre_cmds=[
        "jmp M1",
        "M2",
        # *["pop r1"]
    ], post_cmds=[
        "M1",
    ]),
    Struct("DeclVar", "var", code="state.AddVar(value);"),
    Struct("EndInstr", ";", [
        "pop r1"
    ]),
    Struct("Print", "print", [
        "pop r1",
        "out_int r1",
    ], priority=-1),
    Struct("Nop", "nop", [
    ])
]


non_struct_tokens = [
    NonStructToken("Else", "else"),
    NonStructToken("Open_Par", "("),
    NonStructToken("Close_Par", ")"),
    NonStructToken("Open_Brace", "{"),
    NonStructToken("Close_Brace", "}"),
]


def autonode(fn):
    pretext = """
#pragma once

#include "../utils.hpp"
#include "ltoken_type.hpp"
#include "compile_state.hpp"

using std::string;
using std::unique_ptr;
using std::make_unique;

struct Node;
using Nptr = std::unique_ptr<Node>;

    """.strip()
    lines = list()
    struct_code = """

struct Node {{
    TokenType type;
    int value;
    Nptr left, right;

    explicit Node() = default;
    explicit Node(TokenType type, Nptr left = nullptr, Nptr right = nullptr, double val = 0) :
            type(type), left(std::move(left)), right(std::move(right)), value(val) {{}}

    std::string Compile() const {{
		CompileState st;
		return Compile(st);
    }}
    
    std::string CompileCopy(CompileState& state) const {{
    	CompileState st = state.Copy();
    	return Compile(st);
    }}
    
    std::string Compile(CompileState& state) const {{
        std::string last_mark = state.LastMark();
        std::string mark1 = state.NewMark();
        std::string mark2 = state.NewMark();
    {}
    }}
}};
        """.strip()
    s = struct_code.format("\n\t".join([s.gen_compile() for s in structs]))
    write_file(fn, pretext, [s])


def token_type(fn):
    pretext = """
#pragma once

#include <map>

const int MIN_PRIOR = {};
const int MAX_PRIOR = {};

""".format(MIN_PRIOR, MAX_PRIOR)
    types = ",\n\t".join([s.name.upper() for s in structs])
    non_struct_types = ",\n\t".join([s.name.upper() for s in non_struct_tokens])

    token_types = "".join(["""
    token_types["{}"] = TokenType::{};""".format(
        s.token, s.name.upper()
    ) for s in structs if s.token is not None])

    non_struct_token_types = "".join(["""
    token_types["{}"] = TokenType::{};""".format(
        s.token, s.name.upper()
    ) for s in non_struct_tokens if s.token is not None])

    associativity = "".join(["""
    associativity[TokenType::{}] = {};""".format(
        s.name.upper(), int(s.associativity)
    ) for s in structs if s.associativity is not None])

    priority = "".join(["""
    priority[TokenType::{}] = {};""".format(
        s.name.upper(), s.priority
    ) for s in structs if s.priority is not None])
    lines = ["""
enum struct TokenType {{
    {types},
    {non_struct_types}
}};

std::map<std::string, TokenType> token_types;
std::map<TokenType, int> associativity;
std::map<TokenType, int> priority;

void InitTokenMap() {{
    {token_types}
    {non_struct_token_types}
    {associativity}
    {priority}
}}

    """.format(types=types, token_types=token_types, non_struct_types=non_struct_types,
               non_struct_token_types=non_struct_token_types, associativity=associativity, priority=priority)]
    write_file(fn, pretext, lines)


def construct_node(fn):
    pretext = """
#pragma once

#include "node.hpp" 

"""
    node_by_type = "".join(["""
            case TokenType::{up}:
                return std::make_unique<{name}Node>({name}Node(nullptr, nullptr, value));""".format(
        up=s.name.upper(), name=s.name) for s in structs])

    lines = [("""
std::unique_ptr<Node> ConstructNode(TokenType type, int value) {{
    switch(type) {{
        {}
        default:
            throw std::runtime_error("Invalid token");
    }}
}}
""".format(node_by_type))]
    write_file(fn, pretext, lines)


def write_file(fn, pretext, lines):
    warn_msg = """
/// ATTENTION!
/// This file has been automatically generated by the script gen_node.py.
/// There is no point editing it, try changing gen_node.py.
    
"""
    print("Generating {}...".format(fn))
    file = open(fn, "w")
    file.write(warn_msg)
    file.write(pretext + "\n")
    for line in lines:
        file.write(line + "\n")
    print("Done")


if __name__ == "__main__":
    autonode("node.hpp")
    token_type("ltoken_type.hpp")
    # for i in structs:
    #     print("case: " )
