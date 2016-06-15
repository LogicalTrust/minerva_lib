#
# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE"
# If we meet some day, and you think this stuff is worth it, you can buy us 
# a beer in return.
# ----------------------------------------------------------------------------
#

.PHONY: bootstrap
bootstrap:
	@$(MAKE) -C ../../ -f Makefile.bootstrap

bin:
	@mkdir -p $(WRKDIR)

TARGET_OBJS=\
	$(TARGETWRKDIR)/target.o

.PHONY: target
target: target_start targetdir $(TARGET_OBJS)

.PHONY: target_start
target_start:
	@echo ""
	@echo "####################################"
	@echo "# Generate & Build targets         #"
	@echo "####################################"
	@echo ""

$(TARGET_OBJS): $(TARGETWRKDIR)/target.c $(TARGETWRKDIR)/target.h
	$(CC) $(CFLAGS) -I. -c -o $@ $<

$(TARGETWRKDIR)/target.c $(TARGETWRKDIR)/target.h: $(MICONFIG)
	$(MICOMPILER) -m $(MICONFIG) -o $(TARGETWRKDIR)/target

.PHONY: targetdir
targetdir:
	@mkdir -p $(TARGETWRKDIR)

LOCAL_SRC?=
LOCAL_OBJ=$(patsubst %.c, $(LOCALWRKDIR)/%.o, $(LOCAL_SRC))

local: local_start localdir $(LOCAL_OBJ)

.PHONY: local_start
local_start:
	@echo ""
	@echo "####################################"
	@echo "# Build local                      #"
	@echo "####################################"
	@echo ""

$(LOCAL_OBJ): $(LOCALWRKDIR)/%.o : %.c
	$(CC) $(CFLAGS) -I. -c -o $@ $<

.PHONY: localdir
localdir:
	@mkdir -p $(LOCALWRKDIR)

LDFLAGS+=../../lib/progressbar/libprogressbar.a -lreadline -lcurses

link: link_start $(WRKDIR)/$(OUTFILE)

.PHONY: link_start
link_start:
	@echo ""
	@echo "####################################"
	@echo "# Linking                          #"
	@echo "####################################"
	@echo ""

$(WRKDIR)/$(OUTFILE): $(LOCAL_OBJ) $(MINERVA_CORE_OBJ) $(MINERVA_PARSER_OBJ) $(TARGET_OBJS)
	$(CC) -o $(WRKDIR)/$(OUTFILE) $(MINERVA_CORE_OBJ) $(MINERVA_PARSER_OBJ) $(TARGET_OBJS) $(LOCAL_OBJ) $(LDFLAGS)

.PHONY: clean
clean:
	@rm -fr $(WRKDIR)
