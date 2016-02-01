lib_LTLIBRARIES = libcfe.la

SRC  = log.c
SRC += fcntl.c
SRC += open.c
SRC += keystore.c
if LINUX
SRC += keystore-linux.c
endif
if DARWIN
SRC += keystore-darwin.c
endif
SRC += crypto/hash.c
SRC += crypto/hash-openssl.c
SRC += crypto/cipher.c
SRC += crypto/cipher-openssl.c
SRC += crypto/pbkdf.c
SRC += crypto/pbkdf-openssl.c
SRC += fs/header.c
SRC += fs/header-v1.c
SRC += fs/filewriter.c
SRC += fs/filewriter-v1.c

libcfe_la_CFLAGS   = -I$(top_builddir)/include @OPENSSL_CFLAGS@
libcfe_la_LDFLAGS  = -version-info 0:0:0
libcfe_la_LDFLAGS += -export-symbols-regex '^cfe_'
libcfe_la_LDFLAGS += @OPENSSL_LIBS@
if DARWIN
libcfe_la_LDFLAGS += -framework CoreFoundation
endif
libcfe_la_SOURCES  = $(SRC)
libcfe_la_LIBADD   =
