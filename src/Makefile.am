lib_LTLIBRARIES = libcfe.la

SRC  = log.c
SRC += open.c
SRC += keystore.c
SRC += keystore-linux.c
SRC += crypto/hash.c
SRC += crypto/hash-openssl.c
SRC += fs/header.c
SRC += fs/header-v1.c
SRC += fs/file_writer.c

libcfe_la_CFLAGS   = -I$(top_builddir)/include
libcfe_la_LDFLAGS  = -version-info 0:0:0
libcfe_la_LDFLAGS += -export-symbols-regex '^cfe_'
libcfe_la_SOURCES  = $(SRC)
libcfe_la_LIBADD   =
