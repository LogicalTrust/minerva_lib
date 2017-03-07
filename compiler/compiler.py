#
# this code is full of shit, you have been warned!
#

MINERVA_PREFIX = "minerva_";
ALWAYS_SKIP_TYPES = ["const","unsigned", "signed"]
def __type_to_minerva(t):
    r = "__minerva_"

    while t[0] is not None:
        r += t[0] + "_"
        t = t[1]
    r += t[1]
    
    r = r.replace("*","ptr")
    
    for type in ALWAYS_SKIP_TYPES:
        r = r.replace(type + "_","")
    
    return r + "_t"

def __type_to_str(t):
    r = ""

    while t[0] is not None:
        r += t[0] + " "
        t = t[1]
    r += t[1]
    
    for type in ALWAYS_SKIP_TYPES:
        r = r.replace(type + " ","")
    
    return r

def __compile_type_enum(types):
    r = "typedef enum {\n"
    r += ",\n".join(set(map(lambda x: "\t"+__type_to_minerva(x), types)))
    r += "\n\t,__minerva_types_no"
    r += "\n} minerva_type_t;\n"
    
    return r

def __compile_type_name(types):
    r = "const char *minerva_type_name[] = {\n"
    r += ",\n".join(set(map(lambda x: "\t\""+__type_to_str(x)+"\"", types)))
    r += "\n};\n"
    
    return r

def __compile_wrapper(funcs):

    c = []

    for f in funcs:
        r = "int\n"
        r += "__minerva_wrap_call_"+f[2]+"(minerva_var_t *new, minerva_var_t **vars) {\n"
        #skipping void args
        args = [ elem for elem in f[3] if elem[0][1] != "void" ]

        for i, a in enumerate(args):
            a = a[0]
            r += "\t"+__type_to_str(a)+" "+"__arg"+str(i)+" = "
            r += "(" + __type_to_str(a) +") "
            if '*' in __type_to_str(a):
                r += "vars["+str(i)+"]->val;\n"
            else:
                r += "*("+__type_to_str(a)+"*)vars["+str(i)+"]->val;\n"
        if f[1][1] == 'void':
                r += "\t"+f[2]+"("+",".join(["__arg"+str(i) for i in range(len(args))])+");"
        else:
            if '*' in f[1][1]:
                r += "\tnew->val = " + \
                  f[2]+"("+",".join(["__arg"+str(i) for i in range(len(args))])+");"
            else: 
                r += "\tnew->val = xcalloc(1,sizeof("+__type_to_str(f[1])+"));\n"
                r += "\tnew->flags |= F_VAR_ALLOC;\n"
                r += "\t*(("+__type_to_str(f[1])+"*)new->val) = ("+__type_to_str(f[1])+")"+\
                  f[2]+"("+",".join(["__arg"+str(i) for i in range(len(args))])+");"
     
        r += "\n\treturn 1;\n}\n\n"
        c.append(r)

    return "".join(c)

def __compile_wrapper_init(funcs):
    
    c = ["void\nminerva_funcs_init(minerva_funcs_t *funcs) {\n\tminerva_arg_t *x;\n"]

    for f in funcs:
        r = ""
        if len(f[3]) > 0:
            r += "\tx = xcalloc("+str(len(f[3]))+" ,sizeof(minerva_arg_t));\n"
        for n, t in enumerate(f[3]):
            flags = '|'.join(map(lambda x: "F_VAR_"+x, t[1])) or "0"
            r += "\tx["+str(n)+"] = (minerva_arg_t){"+__type_to_minerva(t[0])+","+flags+"};\n"
        r += "\tminerva_funcs_add(funcs, "
        r += __type_to_minerva(f[1]) + ", "
        r += "__minerva_wrap_call_"+f[2]+", "
        r += "\"" + f[2] + "\", "
        r += "0, "
        r += f[4] +", "
        r += str(len(f[3]))
        if len(f[3]) > 0:
            r += ","
            r += ",".join(map(lambda x: "&x["+str(x)+"]", range(len(f[3]))))
        r += ");\n"
        c.append(r)

    c += ["}\n\n"]

    return "".join(c)

def compile(x):
    
    header_file = """#include <minerva_var.h>
#include <minerva_func.h>
#include <xmalloc.h>
"""

    headers = reversed(list(filter(lambda x: x[0] == 'include', x)))
    functions = list(filter(lambda x: x[0] == 'function', x))

    for h in headers:
        header_file += ("#include %s\n" % h[1])

    body_file = header_file

    header_file = """#ifndef _MINERVA_TARGET_H_
#define _MINERVA_TARGET_H_

extern const char *minerva_type_name[];

"""

    types = set()

    for f in functions:
        types.add(f[1])
        for t in f[3]:
            types.add(t[0])
    header_file += "#define MINERVA_FUNC_NUM %d\n" % (len(functions))
    header_file += __compile_type_enum(types)
    body_file += __compile_wrapper(functions)
    body_file += __compile_wrapper_init(functions)
    body_file += __compile_type_name(types)

    header_file += "\n#endif\n"

    return (header_file, body_file)
