import ply.yacc as yacc
import ply.lex as lex

# XXXSHM: this is lame ~C funcion prototype parser

### LEXER

tokens = (
        'LCHEVRON', 'RCHEVRON',
        'LBRACKET', 'RBRACKET',
        'STRUCT', 'PTR', 'UNION','STANDARDTYPE', 'TYPE_QUALIFIER',
        'COMMA', 'SEPARATOR', 'COMMENT', 'STRING',
        'MIINCLUDE', 'INCLUDE', 'INCLUDEFILE', 'INCLUDEFILELOCAL', 'ARROW'
)

def t_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

def t_UNION(t): r'\bunion\b'; return t
def t_STRUCT(t): r'\bstruct\b'; return t
def t_TYPE_QUALIFIER(t): r'\bconst\b'; return t; #skipped volatile
t_LCHEVRON = r'\{'
t_RCHEVRON = r'\}'
t_LBRACKET = r'\('
t_RBRACKET = r'\)'
t_COMMA = r','
t_SEPARATOR = r';'
t_PTR = r'\*'
t_ARROW = r'\=\>'
t_COMMENT = r'--[^\n]*'
t_STRING = r'[a-zA-Z][a-zA-Z0-9_\-\.\/]*'
t_STANDARDTYPE = r'\b(void|char|short|int|long|float|double|signed|unsigned)\b'
t_MIINCLUDE = r'%include'
t_INCLUDE = r'\#include'
t_INCLUDEFILE = r'<'+t_STRING+r'>'
t_INCLUDEFILELOCAL = r'"'+t_STRING+r'"'
t_ignore = "\t " # ignore spaces and tabs

def t_error(t):
    print("Illegal character '%s' at %d line" % (t.value[0], t.lexer.lineno))

lex.lex()

### PARSER 
def p_decls(t):
    '''decls : decl decls
             | decl
    '''

    if len(t) == 2:
        t[0] = []
    else:
        t[0] = t[2] 
      
    if t[1] is not None:
        t[0].append(t[1])

def p_decl(t):
    '''decl : miinclude
            | include
            | function
            | comment'''
    if t[1] != None:
        t[0] = t[1]

def p_comment(t):
    '''comment : COMMENT'''
    
def p_include(t):
    '''include : INCLUDE INCLUDEFILE
               | INCLUDE INCLUDEFILELOCAL'''
    t[0] = ('include', t[2])

def p_miinclude(t):
    '''miinclude : MIINCLUDE INCLUDEFILELOCAL'''

    t[0] = ('miinclude', t[2])

def p_function(t):
    '''function : type STRING LBRACKET args RBRACKET ARROW STRING SEPARATOR
                | type STRING LBRACKET RBRACKET ARROW STRING SEPARATOR'''
    if len(t) == 9:
        t[0] = ('function', t[1],t[2],t[4],t[7])
    else:
        t[0] = ('function', t[1],t[2],[],t[6])

def p_args(t):
    '''args : arg COMMA args
            | arg'''

    if len(t) == 2:
        t[0] = []
    else:
        t[0] = t[3] 
    
    t[0] = [t[1]] + t[0]

def p_arg(t):
    '''arg : type STRING var_flags
           | type var_flags'''
        
    if len(t) == 3:
        t[0] = (t[1], t[2])
    else:
        t[0] = (t[1], t[3])


def p_var_flags(t):
    '''var_flags : LCHEVRON var_arg_flags RCHEVRON
                 | empty'''
    
    if len(t) == 2:
        t[0] = []
    else:
        t[0] = t[2]


def p_var_arg_flags(t):
    '''var_arg_flags : STRING COMMA var_arg_flags
                     | STRING'''

    if len(t) == 2:
        t[0] = []
    else:
        t[0] = t[3] 
    
    t[0] = [t[1]] + t[0]


def p_type(t):
    '''type : type_list
            | user_definied_type'''
    if len(t) == 2:
        t[0] = t[1]
    else:
        t[0] = (t[1],t[2])

def p_user_definied_type(t):
    '''user_definied_type : TYPE_QUALIFIER STRING ptr_list 
                          | UNION STRING ptr_list
                          | STRUCT STRING ptr_list
                          | struct_things
                          | STRING ptr_list'''
    # I dont like it...
    if len(t) == 4:
        t[0] = (t[1],(t[2],t[3]))
    elif len(t) == 3:
        t[0] = (t[1],t[2])
    else:
        t[0] = t[1]

def p_struct_things(t):
    '''struct_things : TYPE_QUALIFIER STRING
                     | STRUCT STRING
					 | UNION STRING
                     | STRING'''
    if len(t) == 2 :
        t[0] = (None,t[1])
    else:
        t[0] = (t[1],(None,t[2]))

def p_ptr_list(t):
    '''ptr_list : PTR ptr_list
                | PTR'''
    if len(t) == 2:
        t[0] = (None,t[1])
    else:
        t[0] = (t[1],t[2])

def p_type_list(t):
    '''type_list :  basic_type
                  | basic_type type_list'''

    if len(t) == 2:
        t[0] = (None, t[1])
    else:
        t[0] = (t[1], t[2])

def p_basic_type(t):
    '''basic_type : TYPE_QUALIFIER
                  | STANDARDTYPE
                  | PTR'''
    #skipped storage_class_specifier (auto,register,static, extern, typedef)
    t[0] = t[1]

def p_empty(p):
    'empty :'
    pass

def p_error(t):
    print("Syntax error '%s' at line %d" % (t.value, t.lexer.lineno))

yacc.yacc()

def parse(content):
    return yacc.parse(content)

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        print(yacc.parse(open(sys.argv[1]).read()))
    else:
        while 1:
            try:
                line = raw_input('parser_test> ')
                print(yacc.parse(line))
            except EOFError:
                break
