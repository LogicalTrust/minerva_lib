#
# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE"
# If we meet some day, and you think this stuff is worth it, you can buy us 
# a beer in return.
# ----------------------------------------------------------------------------
#

MINERVA_CORE_SRC=\
    main.c \
    minerva_dict.c \
    minerva_call.c \
    minerva_func.c \
    minerva_generic.c \
    minerva_var.c \
    xmalloc.c \
    minerva_repl.c \
    minerva_loop.c \
    minerva_trace.c \
    minerva_signal.c \
    dict.c \
    random.c \
    rand.c

MINERVA_PARSER_OBJ=\
    $(COREWRKDIR)/minerva_lexer.o \
    $(COREWRKDIR)/minerva_parser.tab.o


MINERVA_CORE_OBJ=$(patsubst %.c, $(COREWRKDIR)/%.o, $(MINERVA_CORE_SRC))
MINERVA_CORE_SRCDIR=$(patsubst %, $(COREDIR)/%, $(MINERVA_CORE_SRC))

core: core_start coredir parser $(MINERVA_CORE_OBJ)

core_start:
	@echo ""
	@echo "####################################"
	@echo "# Building core                    #"
	@echo "####################################"
	@echo ""

.PHONY: coredir
coredir:
	@mkdir -p $(COREWRKDIR)

$(MINERVA_CORE_OBJ): $(COREWRKDIR)/%.o : $(COREDIR)/%.c
	$(CC) $(CFLAGS) -I../../include/ -I../../lib/progressbar/include/ -I$(TARGETWRKDIR) -c -o $@ $<

parser: $(COREWRKDIR)/minerva_parser.tab.c $(COREWRKDIR)/minerva_parser.tab.h $(COREWRKDIR)/minerva_lexer.c $(MINERVA_PARSER_OBJ)

$(COREWRKDIR)/minerva_parser.tab.c $(WRKDIR)/minerva_parser.tab.h: $(COREDIR)/minerva_parser.y
	$(BISON) -d $(COREDIR)/minerva_parser.y -o $(COREWRKDIR)/minerva_parser.tab.c

$(COREWRKDIR)/minerva_lexer.c: $(COREDIR)/minerva_lexer.l
	$(FLEX) -o $(COREWRKDIR)/minerva_lexer.c --header-file=$(COREWRKDIR)/minerva_lexer.h $(COREDIR)/minerva_lexer.l
