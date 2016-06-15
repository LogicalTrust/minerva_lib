#
# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE"
# If we meet some day, and you think this stuff is worth it, you can buy us 
# a beer in return.
# ----------------------------------------------------------------------------
#

BISON=bison
FLEX=flex
# from target perspective target/sth/
MICOMPILER=../../compiler/mi2c.py
WRKDIR=bin/
TARGETWRKDIR=$(WRKDIR)/target/
COREWRKDIR=$(WRKDIR)/core
LOCALWRKDIR=$(WRKDIR)/local/

COREDIR=../../core/

CFLAGS+=-I../../include/ -Wall -O0 -I$(TARGETWRKDIR)

ifdef ASAN
CFLAGS+=-fsanitize=address
#XXX: fix building on gcc
LDFLAGS+=-fsanitize=address
endif

ifdef DEBUG
CFLAGS+=-DMINERVA_DEBUG -ggdb
endif

ifeq ($(MAKECMDGOALS),clean)
MICONFIG=clean
endif

ifndef MICONFIG
TMPMICONFIG=$(shell find . -maxdepth 1 -name "*.mi" | wc -l | tr -d ' ')
ifeq ($(TMPMICONFIG), 1)
MICONFIG=$(shell find . -maxdepth 1 -name "*.mi")
$(info MICONFIG not provided, assuming $(MICONFIG))
endif
endif

ifndef MICONFIG
$(error Multiple mi files in directory, specify MICONFIG (eg. make MICONFIG=foo.mi))
endif

OUTFILE=minerva-$(TARGET)-$(basename $(notdir $(MICONFIG)))

.PHONY: all
all: bootstrap bin target local core link
