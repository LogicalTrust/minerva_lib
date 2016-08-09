# **minerva_lib a.k.a Polish Fuzzy Lop**

# Disclaimer

**Minerva_lib is still in its early development stage, therefore it is full of bugs. It also lacks many features. Since dumb fuzzing is just dumb fuzzing please don't expect any sort of intelligence from it. You're using it at your own risk. You've been warned.**

**If you would like to contribute to the project, please feel free to submit pull requests, bug reports, letters of complain (or love) etc. Any support is appreciated.**

# What am I dealing with?
PFL is a fuzzer designed mainly for torturing stuff (mainly libraries or APIs, sometimes co-workers). It uses minerva algorithm (please refer to an article listed as [1] in reading secton for any further information). Minerva_lib is able to fuzz any piece of code, as long as it can be linked against its core. 

PFL can also be treated as an animal. Feed it with C function prototypes, supply a simple Makefile and it will treat you with a new shiny fuzzer.
Since, as mentioned earlier on, PFL is in its early development stage, most cases you will have to provide generators though.

**Let's make dumb fuzzing great again!**

# How do I add new targets?
We tried to make this process as painless as possible. It is on you to provide config file (target.mi) and magic makefile (refer to a few target examples we provided in case of any problems). Than, PFL generates target.c and target.h, which supported by minerva_core results in target_bin (your shiny fuzzer, that is). 
## Generation process

     +-----------+                 +----------------+
     | target.mi |<----------------| magic Makefile |
     | (config)  |                 +----------------+
     +-----------+                         |
          |                                |      things you provide
     -- 8< - 8< - 8< - 8< - 8< - 8< - 8< - 8< - 8< ------------------
          |                                |      stuff that is done 
          |                                |      automatically
          | (mi2c compiler)                |
          |                                |
          v                                v
     +------------+               +--------------+
     | target.[ch]|-------------->| minerva core |
     +------------+               +--------------+
           |                              |
           \--------------+---------------/
                          |
                          v
                    +------------+
                    | target_bin | (your shiny fuzzer)
                    +------------+
                    
## Configuration
The format used in configuration file is very similar to C format. To help you understand it we provide the following scheme.

    include files (eg. #include <header_file.h>
    
    C prototypes => check function; (eg. int foo(int x) => generic_success;

### Toy target
In order to help you get the basics we created a toy target(You can find it in /target/toy directory). Taking toy api's declarations from toy.h:

    int zero(void);
    int add_one(int x);
    int crashme(int x);

A configuration file (toy.mi) would look like that:

    #include <minerva_generic.h>
    #include <toy.h>
    --this a comment
    int zero() => generic_success;
    int add_one(int x) => generic_success;
    int crashme(int x) => generic_success;


For the sake of PFL working, minerva includes generic functions in order to check the result of particular call.
Take a look at include/minerva_generic.h for the complete list of these.


###Variables can also have annotations

    void BN_CTX_free(BN_CTX *c {DESTROY}) => generic_void;
DESTROY indicates that call destroys the variable, so fuzzer wont use any further.


    int mutate_int_and(int a {MUTATE}, int b {MUTATE}) => generic_success;

MUTATE indicates that particular variable is mutated by the function. Fuzzer uses it for 
mutation phase (see var_mut shell command).

## Building

Create your own subdirectory in target/ directory, it should contain
configuration file (as explained above) and Makefile. That's it,  a minimal set of files
needed to generate fuzzer. Example Makefile looks as follows
(/target/toy/Makefile):

    TARGET=toy
    LOCAL_SRC= \
        toy.c
    
    include ../../mk/minerva.mk

TARGET - is a target name (could be any name).
LOCAL_SRC - is a set of files that is additionally linked with the fuzzer (helper functions etc).

Makefiles may also include LDFLAGS in order to link against other libraries (see target/openssl/Makefile):

    TARGET=openssl
    LOCAL_SRC= \
    	minerva_vars_init.c
    
    LDFLAGS=-lssl -lcrypto
    
    include ../../mk/minerva.mk

the include of minerva.mk is mandatory (it does magic to generate fuzzer for
you). Building system is mostly inspired by BSD ports.

Makefile also supports a few parameters, as follows:

 

 - DEBUG=1 - build without optimizations, additional debug features are
   turned on. 
 - ASAN=1 - build with address sanitizer

## Fuzzing

Fuzzing should be as easy as running compiled binary.

    target/toy/ $ lldb ./bin/minerva-toy-toy
    seed: 2946546160
    \o/ found crash *yay* \o/
    
    target/toy/ $ lldb ./bin/minerva-toy-toy
    (lldb) target create "./bin/minerva-toy-toy"
    Current executable set to './bin/minerva-toy-toy' (x86_64).
    (lldb) r
    Process 58757 launched: './bin/minerva-toy-toy' (x86_64)
    seed: 799555025
    Process 58757 stopped
    * thread #1: (...) toy.c:22, queue = 'com.apple.main-thread',
      stop reason = EXC_BAD_ACCESS (code=1, address=0x0)
      frame #0: 0x000000010000809d minerva-toy-toy`crashme(x=5) + 29 at toy.c:22
    
       19  	{
       20  	    int *y = NULL;
       21  	    if (x > 4)
    -> 22  	        return *y;
       23  	    else
       24  	        return x+2;
       25  	}

## Shell mode

If you want to run the fuzzing in shell mode, execute the binary with -r option:

    target/toy/ $ ./bin/minerva-toy-toy -r
    seed: 100837884
    pfl> help
    fuzz([iter, [ctx]]) - fuzz: does iter iterations, uses ctx
    var_mut(iter, ctx) - does iter mutations of variables stored in ctx
    var_stat(ctx) - shows statistics about variables stored in ctx
    func_stat(ctx) - shows statistics about functions calls stored in ctx
    play(trace) - plays trace or context
    restore("filename") - restores trace from file
    save(trace, "filename") - saves trace to file
    min(trace) - minimizes trace
    show(trace) - shows trace
    quit, exit - quits

In order to perform fuzzing, being in shell mode, use fuzz command:

    target/toy/ $ ./bin/minerva-toy-toy -r
    seed: 3366955315
    pfl> x = fuzz(100)
    Fuzzing |====================================================| ETA: 0h00m00s
    pfl> 

## Test case management

Fuzzing process saves traces, which can be replayed later on in order to reproduce
particular execution. It uses simple format to save the function calls:

    target/toy/ $ ./bin/minerva-toy-toy -T /dev/stdout
    seed: 2631490055
    \o/ found crash *yay* \o/
    0 = zero()
    1 = zero()
    2 = crashme(1)
    3 = add_one(2)
    4 = crashme(3)
    5 = crashme(0)
    6 = crashme(2)
    7 = zero()
    8 = zero()
    9 = zero()
    10 = crashme(3)
    11 = add_one(5)
    12 = crashme(4)
    
    target/toy/ $ ./bin/minerva-toy-toy -T tmp
    seed: 3370842429
    \o/ found crash *yay* \o/
    target/toy/ $ ./bin/minerva-toy-toy -p tmp
    seed: 1519824755
    Replay |===================================================  | ETA: 0h00m00s
    target/toy/ $ lldb ./bin/minerva-toy-toy 
    r -p tmp
    (lldb) target create "./bin/minerva-toy-toy"
    Current executable set to './bin/minerva-toy-toy' (x86_64).
    (lldb) r -p tmp
    Process 12467 launched: './bin/minerva-toy-toy' (x86_64)
    seed: 904045424
    Process 12467 stopped======================================= | ETA: 0h00m00s
    * thread #1: tid = 0x2bca2, 0x000000010000823d minerva-toy-toy`crashme + 29,
        queue = 'com.apple.main-thread', stop reason = EXC_BAD_ACCESS (code=1,
        address=0x0)

## F.A.Q

###Q: My API function requires some pre-conditions to be satisfied.
A: Write a simple wrapper to make sure that those conditions are met.

###Q: My API function requires some var initialization / depends on internal state
A: Write wrapper or introduce new types that indicate internal state. Here's an quick example:

    STH_new *STH_new(void) => generic_not_null;
    STH *STH_init(STH_new *) => generic_not_null;

###Q: Where I can get help?
A: Use -h switch or help command in the shell mode.

###Q: Does function input variables depend on each other?
A: You have to wrap function yourself, sorry.

###Q: How can I help you?
A: Take a look at TODO list, fill bug reports, send PRs. Your help is more than welcome.

###Q: What are the dependencies?
A: Python, GNU Make, C compiler, Bison, Flex, py-yacc, libprogressbar.

###Q: Is the name - Polish Fuzzy Loop a joke?
A: No, it's a coincidence.

###Q: Have it found any bugs, ever?
A: Yes, including various minor bugs in OpenSSL, LibreSSL and OpenSSH.

###Q: What platforms are supported?
A: We successfully ran this software on Linux, FreeBSD, NetBSD, OpenBSD and Mac OS X.

###Q: When are you going to implement more features (like coverage)?
A: We don't know. However, You're more than welcome to develop it on your own and share itwith us.

## Reading material

1. http://php-security.org/2010/05/11/mops-submission-05-the-minerva-php-fuzzer/
2. http://www.slideshare.net/logicaltrust/torturing-the-php-interpreter

## Credits

 - @akat1_pl - http://akat1.pl/
 - n1x0n
 - s1m0n
 - PSi
 - Zeruś
