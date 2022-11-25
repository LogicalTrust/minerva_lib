from abnf_ast import Option, Group, Concatenation, Alternation, Rule, Element, Rulename, NumVal, CharVal, ProseVal, traverse_rule

def rule_to_type(name, ptr = False):
    return "{name}_t{ptr}".format(name = name, ptr = " *" if ptr else "")

def variable_to_decl(var, prefix = 'v'):
    var_num, var_type = var

    return "{var_type}{prefix}{number}".format(var_type = rule_to_type(var_type, ptr = True), prefix = prefix, number = var_num)

def add_tab(s):
    s = s.split('\n')
    s = map(lambda x: '\t' + x, s)
    return '\n'.join(s)

def compile_ast(ast, state, global_state):
    
    if isinstance(state, list):
        state = list(map(add_tab, state))
    elif state is not None:
        state = add_tab(state)


    if isinstance(ast, NumVal):
        if ast.const is not None:
            return "minerva_concat(&r, \"%s\");" % ast.const
        else:
            return "{char x[2]; x[1] = 0; x[0] = (__rand() %% %d) + %d; minerva_concat(&r, x); }\n" % (ast.upper-ast.lower+1, ast.lower)
    elif isinstance(ast, CharVal):
        return "minerva_concat(&r, {text});\n".format(text = ast.string)
    elif isinstance(ast, ProseVal):
        return "minerva_concat(&r, \"{text}\");\n".format(text = ast.string)
    elif isinstance(ast, Rulename):
        return "minerva_concat(&r, {fname}());".format(fname = ast.name)
    elif isinstance(ast, Element):
        if ast.repeat is None:
            return state

        r = ["{ int num = 0;\n"]
        if ast.repeat[0] == 'exact':
            r.append(" num = %s;\n" % ast.repeat[1])
        else:
            if ast.repeat[1] is not None:
                r.append(" num = %s;\n" % ast.repeat[1])
            else:
                r.append(" num = 0;")

            if ast.repeat[2] is None:
                upper = "10" # XXX: arbitrary?
            else:
                upper = ast.repeat[2]

            r.append(" num += (__rand() %% %s);" % upper) 
        r.append("while(num--)")
        r.append("{")
        r.append(state)
        r.append("}")
        r.append("}")
        return "\n".join(r)
    elif isinstance(ast, Alternation):
        r = ["switch(__rand() % {branches}) ".format(branches = len(state))]
        r.append("{")
        for n, code in enumerate(state):
            r.append("\tcase {num}: {code} break;".format(num = n, code = code.strip()))
        r.append("}")
        return "\n".join(r)
    elif isinstance(ast, Option):
        # XXX: does it make sense?
        r = ["if (__rand() % 2) {\n"]
        r.append(state)
        r.append("}")
        return "\n".join(r)
    elif isinstance(ast, Concatenation):
        return "\n".join(state)
    else:
        return state or ""

def compile_rule(rule):
    # header
    r = []
    r.append("")
    r.append("{return_type}".format(return_type = rule_to_type(rule.rulename, ptr = True)))
    r.append("{name}()".format(name = rule.rulename))
    r.append("{");
    r.append("char *r = \"\";")
    r.append(traverse_rule(rule.rule, compile_ast))
    r.append("return r;")
    r.append("}");

    return '\n'.join(r)

def compile_include(rules):
    # create typedefs
    r = []
    
    r.append("#include \"minerva_concat.h\"")

    # typedefs
    for rule in rules:
        r.append("typedef char {name}_t;".format(name = rule.rulename))
    
    r.append("")

    # GENERATE INCLUDE    
    for rule in rules:
        r.append("{return_type}{name}();".format(return_type = rule_to_type(rule.rulename, ptr = True), name = rule.rulename))

    return '\n'.join(r) + "\n"


def compile_miconfig(rules, name):
    r = []
    r.append("#include <minerva_generic.h>")
    r.append("#include \"minerva_concat.h\"")
    r.append("#include \"{}.h\"".format(name))

    # GENERATE MINERVA RULES
    for rule in rules:
        r.append("{return_type}{name}() => generic_not_null;".format(return_type = rule_to_type(rule.rulename, ptr = True), name = rule.rulename))
        r.append("{return_type} -> minerva_generic_stringify_string;".format(return_type =
                rule_to_type(rule.rulename, ptr = True)))

    return '\n'.join(r) + "\n"

def compile_rules(rules, name):
    r = ["#include <random.h>"]
    r.append("#include <stddef.h>")
    r.append("#include \"{}.h\"".format(name))
    r.append("")

    return '\n'.join(r) + ''.join(map(compile_rule, rules)) + "\n"

def compile(rules, name):
    with open("{}.h".format(name), "w") as include_file:
        include_file.write(compile_include(rules))

    with open("{}.mi".format(name), "w") as miconfig_file:
        miconfig_file.write(compile_miconfig(rules, name))

    with open("{}.c".format(name), "w") as code_file:
        code_file.write(compile_rules(rules, name))
