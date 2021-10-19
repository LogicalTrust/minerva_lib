TARGET=libressl
LOCAL_SRC= \
    generate.c \
	stringify.c

CFLAGS=-I include_dir/ -Werror
LDFLAGS=-z muldefs -L . -lcompat -lcrypto -pthread

include ../../mk/minerva.mk
