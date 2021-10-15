TARGET=openssl
LOCAL_SRC= \
    generate.c \
	stringify.c

CFLAGS=-I include_dir/
LDFLAGS=-z muldefs -L . -lcrypto

include ../../mk/minerva.mk
