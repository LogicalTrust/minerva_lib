TARGET=libressl
LOCAL_SRC= \
    generate.c \
	stringify.c

CFLAGS=-I include_dir/
LDFLAGS=-z muldefs -L . -lcompat -lcrypto

include ../../mk/minerva.mk
