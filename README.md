# **minerva_lib a.k.a Polish Fuzzy Lop**

# disclamer

**This stuff is highly experimental, so beware it's full of bugs, lacking of many features and dumb in many ways. You're using it at your own risk.**

**If you want to make the PFL a better fuzzer for you and me, then feel free to submit PRs, bugs, complaints, send your love or hate.**

# what is it?

It's a fuzzer dedicated mainly for torturing stuff like libraries or APIs, using minerva algorithm which is described in the [1] article. It can fuzz any piece of code that can be linked against its core. The idea is that everything you need is to feed minerva with C function prototypes and simple Makefile, and then you get a shiny fuzzer (but in most cases you have to provide generators).  **Let's make dumb fuzzing great again!**

# adding new target

## generation process

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

## configuration file

Configuration file has following format:

    include files (eg. #include <header_file.h>
    
    C prototypes => check function; (eg. int foo(int x) => generic_success;

for toy API:

    int zero(void);
    int add_one(int x);
    int crashme(int x);

configuration file (toy.mi) looks like this:

    #include <minerva_generic.h>
    #include <toy.h>
    
    int zero() => generic_success;
    int add_one(int x) => generic_success;
    int crashme(int x) => generic_success;

(you can find this example in /target/toy/ directory)

Minerva includes generic functions that check the result of particular call, see include/minerva_generic.h for their list.

Variables can have annotations:

    void BN_CTX_free(BN_CTX *c {DESTROY}) => generic_void;

DESTROY means that call destroys variable (means it will be not used in the
next calls).

Another example is MUTATE:

    int mutate_int_and(int a {MUTATE}, int b {MUTATE}) => generic_success;

which means that particular variable is mutated by the function. It's used by
mutation phase (see var_mut shell command).

## building

Create your own subdirectory in target/ directory, it should contain
configuration file (see above) and Makefile (it's minimal set of files
needed to generate fuzzer). Example Makefile looks as follows
(/target/toy/Makefile):

    TARGET=toy
    LOCAL_SRC= \
        toy.c
    
    include ../../mk/minerva.mk

TARGET - is a target name (could be any name).
LOCAL_SRC - is a set of file that is additionally linked with the fuzzer (so helper functions

Makefiles may also include LDFLAGS in order to link against other libraries (see target/openssl/Makefile):

    TARGET=openssl
    LOCAL_SRC= \
    	minerva_vars_init.c
    
    LDFLAGS=-lssl -lcrypto
    
    include ../../mk/minerva.mk

the include of minerva.mk is mandatory as it does magic to generate fuzzer for
you. Building system is mostly inspired by BSD ports.

Following parameters to Makefile are supported:

 

 - DEBUG=1 - build without optimizations, additional debug features are
   turned on. 
 - ASAN=1 - build with address sanitizer

## fuzzing

Fuzzing should be as easy as run compiled binary.

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

## shell mode

For shell mode, run binary with -r option:

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

You can run fuzzing here by using fuzz command:

    target/toy/ $ ./bin/minerva-toy-toy -r
    seed: 3366955315
    pfl> x = fuzz(100)
    Fuzzing |====================================================| ETA: 0h00m00s
    pfl> 

## test case management

Fuzzing process saves traces which can be replayed in order to reproduce
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

Q: My API function requires some pre-conditions to be satisfied.

A: Write a simple wrapper to make sure that those conditions are met.

Q: My API function requires some var initialization / depends on internal state

A: Write wrapper or introduce new types that indicate internal state. Here's an quick example:

    STH_new *STH_new(void) => generic_not_null;
    STH *STH_init(STH_new *) => generic_not_null;

Q: Where I can get help?

A: Use -h switch or help command in the shell mode.

Q: Function input variables depend on each other?

A: You have to wrap function yourself, sorry.

Q: How can I help you?

A: See TODO, fill bug reports, send PRs. Your help is more than welcome.

Q: What are the dependencies?

A: Python, GNU Make, C compiler, Bison, Flex, py-yacc, libprogressbar.

Q: Is the name - Polish Fuzzy Lop a joke?

A: No, it's a coincidence.

Q: Have it found any bugs, ever?

A: Yes, including various minor bugs in OpenSSL, LibreSSL and OpenSSH.

Q: What platforms are supported?

A: We successfully ran this software on Linux, FreeBSD, NetBSD, OpenBSD and Mac OS X.

Q: When are you going to implement more features (like coverage)?

A: We don't know. For sure You'll do it first on your own!

## reading material

1. http://php-security.org/2010/05/11/mops-submission-05-the-minerva-php-fuzzer/
2. http://www.slideshare.net/logicaltrust/torturing-the-php-interpreter

## credits

 - @akat1_pl - http://akat1.pl/
 - n1x0n
 - s1m0n
 - PSi
