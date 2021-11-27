class Element():
    def __init__(self, repeat, element):
        self.repeat = repeat
        self.element = element

    def __str__(self):
        return "%s %s" % (self.repeat, self.element)

    def __repr__(self):
        return self.__str__()

class Rule():
    def __init__(self, rulename, operator, rule):
        self.rule = rule
        self.rulename = rulename
        self.operator = operator

    def __str__(self):
        return '%s %s %s' % (self.rulename, self.operator, self.rule)

    def __repr__(self):
        return self.__str__()

class Alternation():
    def add(self, x):
        self.alternation.insert(0, x)

    def __init__(self, x = None):
        self.alternation = []
        if x is not None:
            self.alternation.append(x)

    def __str__(self):
        s = map(str, self.alternation)
        return '|'.join(s)

    def __repr__(self):
        return self.__str__()

class Concatenation():
    def add(self, x):
        self.concat.insert(0, x)

    def __init__(self, x = None):
        self.concat = []

        if x is not None:
            self.concat.append(x)

    def __str__(self):
        s = map(str, self.concat)
        return '.'.join(s)

    def __repr__(self):
        return self.__str__()

class Group():
    def __init__(self, group):
        self.group = group

class Option():
    def __init__(self, opt):
        self.option = opt



class NumVal():
    const = None
    def __init__(self, num):
        base = {'b': 2, 'd': 10, "x": 16}[num[1]]
        num = num[2:]

        if '.' in num or '-' not in num:
            self.const = ""
            num = num.split('.')
            for p in num:
                self.const += "\\x%02x" % int(p, base)
        else:
            self.lower, self.upper = map(lambda x: int(x, base), num.split('-'))

class CharVal():
    def __init__(self, string):
        self.string = string

class ProseVal():
    def __init__(self, string):
        self.string = string

class Rulename():
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return self.name

def traverse_rule(ast, callback, global_state = None):
    # XXX: do not go into Rules?
    # XXX: POSTORDER/PREORDER switch?
    if type(ast) in [NumVal, ProseVal, CharVal, Rulename]:
        return callback(ast, None, global_state)
    elif isinstance(ast, Group):
        r = traverse_rule(ast.group, callback, global_state)
        return callback(ast, r, global_state)
    elif isinstance(ast, Option):
        r = traverse_rule(ast.option, callback, global_state)
        return callback(ast, state, global_state)
    elif isinstance(ast, Alternation):
        rs = list(map(lambda x: traverse_rule(x, callback, global_state), ast.alternation))
        return callback(ast, rs, global_state)
    elif isinstance(ast, Element):
        r = traverse_rule(ast.element, callback, global_state)
        return callback(ast, r, global_state)
    elif isinstance(ast, Concatenation):
        rs = list(map(lambda x: traverse_rule(x, callback, global_state), ast.concat))
        return callback(ast, rs, global_state)

    assert False, "AST node type not supported: {typename}".format(typename = type(ast))
