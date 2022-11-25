from abnf_ast import Option, Group, Concatenation, Alternation, Rule, Element, Rulename, NumVal, CharVal, ProseVal
import ply.yacc as yacc
import ply.lex as lex

# Parser of ABNF definied in the RFC5234
# https://datatracker.ietf.org/doc/html/rfc5234

tokens = (
    'BIN_VAL', 'DEC_VAL', 'HEX_VAL', 'STRING', 'NOT_EQUAL', 'COMMENT',
    'NUMBER', 'PROSE_VAL', 'CHAR_VAL', 'newline'
)

def t_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

t_CHAR_VAL = r'"[^"]*"'
t_PROSE_VAL = r'<[^>]+>' 
t_NUMBER = r'[0-9]+'
d_BIN = r'[01]+'
d_DEC = r'[0-9]+'
d_HEX = r'[0-9A-Fa-f]+'

def create_num_token(prefix, definition):
    return r'%' + prefix + definition + '(-' + definition + '|(\.' + definition + ')*)'

t_BIN_VAL = create_num_token('b', d_BIN)
t_DEC_VAL = create_num_token('d', d_DEC)
t_HEX_VAL = create_num_token('x', d_HEX)
t_STRING = r'[A-Za-z_-][A-Za-z0-9_-]+'
t_NOT_EQUAL = r'=/'
t_COMMENT = r';[^\n]*'
t_ignore = "\n\t " # ignore spaces and tabs

literals = ['=' , '<', '>', '(', ')', '[', ']', '/', '*']

def t_error(t):
    print("Illegal character '%s' at %d line" % (t.value[0], t.lexer.lineno))

lex.lex()


def p_rulelist(t):
    '''rulelist : rulelist rule
                | rule'''

    if len(t) == 2:
        t[0] = [t[1]]
        return

    t[1].append(t[2])
    t[0] = t[1]

def p_rule(t):
    '''rule : rulename defined_as elements
            | rulename defined_as elements COMMENT'''
    
    t[0] = Rule(t[1], t[2], t[3])

def p_rulename(t): 
    '''rulename : STRING'''
    t[0] = Rulename(t[1].replace('-','_'))

def p_defined_as(t):
    '''defined_as : operator '''
    t[0] = t[1]

def p_elements(t):
    '''elements : alternation '''
    t[0] = t[1]

def p_operator(t):
    '''operator : '='
                | NOT_EQUAL '''
    t[0] = t[1]

def p_alternation(t):
    '''alternation : concatenation 
                   | concatenation '/' alternation'''

    if len(t) == 2:
        t[0] = t[1]
    else:
        if isinstance(t[3], Alternation):
            t[3].add(t[1])
            t[0] = t[3]
        else:
            t[0] = Alternation(t[3])
            t[0].add(t[1])

def p_concatenation(t):
    '''concatenation : repetition
                     | repetition concatenation'''

    if len(t) == 2:
        t[0] = t[1]
    else:
        if isinstance(t[2], Concatenation):
            t[2].add(t[1])
            t[0] = t[2]
        else:
            t[0] = Concatenation(t[2])
            t[0].add(t[1])

def p_repetition(t):
    '''repetition : repeat element
                  | element'''
    if len(t) == 2:
        t[0] = Element(None, t[1])
    else:
        t[0] = Element(t[1], t[2])

def p_repeat(t):
    '''repeat : NUMBER '*'
              | NUMBER '*' NUMBER
              | '*' NUMBER
              | '*' 
              | NUMBER '''

    if len(t) == 3:
        if t[1] == '*':
            t[0] = ('repeat', None, t[2])
        else:
            t[0] = ('repeat', t[1], None)

    if len(t) == 4:
        t[0] = ('repeat', t[1], t[3])
    
    if len(t) == 2:
        if t[1] == '*':
            t[0] = ('repeat', None, None)
        else:
            t[0] = ('exact', t[1])

def p_element(t):
    '''element : rulename
               | group
               | option
               | char_val
               | num_val
               | prose_val'''
    t[0] = t[1]

def p_char_val(t):
    '''char_val : CHAR_VAL'''
    t[0] = CharVal(t[1].replace("\\", "\\\\"))

def p_prose_val(t):
    '''prose_val : PROSE_VAL'''
    t[0] = ProseVal(t[1])

def p_group(t):
    '''group : '(' alternation ')' '''
    t[0] = Group(t[2])

def p_option(t):
    '''option : '[' alternation ']' '''
    t[0] = Option(t[2])

def p_num_val(t):
    '''num_val : BIN_VAL
               | DEC_VAL
               | HEX_VAL'''
    t[0] = NumVal(t[1])

def p_error(t):
    print("Syntax error '%s' at line %d" % (t.value, t.lexer.lineno))

yacc.yacc()

if __name__ == '__main__':
    import sys
    import compiler
    if len(sys.argv) > 2:
        p = yacc.parse(open(sys.argv[1]).read())
        compiler.compile(p, sys.argv[2])
    else:
        while 1:
            try:
                line = input('parser_test> ')
                line = line + '\n'
                print(yacc.parse(line))
            except EOFError:
                break
