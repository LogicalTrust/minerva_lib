#! /usr/bin/env python

import compiler
import getopt
import parser
import sys
import traceback
import errno

EXIT_FAILURE = 1

def _print_usage():
    print("%s -h -m[FILE] -o[OUTPUT] -I[PATH]" % (sys.argv[0]))

def _print_fatal(err):
    print("")
    print("fatal error: %s" % err)
    print("")
    traceback.print_exc()
    sys.exit(EXIT_FAILURE)

def _parse_args(args, config):
    try:
        opts,_ = getopt.getopt(args, "hm:o:I:")
    except (getopt.GetoptError, err):
        print(err)
        _print_usage()
        sys.exit()

    # empty argv list
    if len(opts) == 0:
        _print_usage()
        sys.exit()

    for opt, arg in opts:
        if opt == '-h':
            _print_help()
            sys.exit()
        elif opt == '-m':
            config['input_file'] = arg
        elif opt in '-o':
            config['output'] = arg
        elif opt in '-I':
            config['include_path'].append(arg)
        else:
             assert False, "unhandled"

def _write_file(filename, body):
    fh = open(filename, 'w')
    fh.write(body)
    fh.close

def main():
    config = {}
    config['input_file'] = None
    config['output'] = None
    config['include_path'] = [''] # empty prefix is default

    _parse_args(sys.argv[1:], config)

    if config['input_file'] is None or config['output'] is None:
        _print_usage()
        sys.exit(EXIT_FAILURE)

    functions = []
    input_files = [(config['input_file'], False)]

    while input_files != []:
        (input_file, local) = input_files.pop()

        input_content = None
        paths = config['include_path'] if not local else ['./']
        for path in paths:
            try:
                input_fh = open(path + input_file, 'r')
                input_content = input_fh.read()
                input_fh.close()
                break
            except IOError as e:
                if e.errno != errno.ENOENT:
                    _print_fatal(e)

        if input_content is None:
            _print_fatal("can't open include file: %s" % input_file)

        try:
            x = parser.parse(input_content)
        except Exception as err:
            _print_fatal(err)

        includes = reversed(list(filter(lambda x: x[0] == 'miinclude', x)))

        for include in includes:
            # check if file is local or not:
            #
            # "file" are local
            # <file> are global
            #
            # (just like in C)
            local = include[1].startswith('"')
            include = include[1][1:-1]
            if include in input_files:
                _print_fatal('cycle include on file: %s' % include)
            else:
                input_files.append((include, local))

        functions += x    

    try:
        (header, body) = compiler.compile(functions)
    except Exception as err:
        _print_fatal(err)

    _write_file(config['output']+".h", header)
    _write_file(config['output']+".c", body)

if __name__ == "__main__":
    main()
