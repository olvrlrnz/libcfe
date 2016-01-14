TESTS=check_filewriter check_ref check_keystore check_hash check_create
TESTS_LDFLAGS = @CHECK_LIBS@ -lkeyutils -lcrypto

check_PROGRAMS = check_filewriter check_ref check_keystore check_hash check_create

check_filewriter_SOURCES = check_filewriter.c
check_filewriter_CFLAGS  = @CHECK_CFLAGS@ -I$(top_builddir)/include
check_filewriter_LDADD   = $(top_builddir)/src/libcfe.la $(TESTS_LDFLAGS)

check_ref_SOURCES = check_ref.c
check_ref_CFLAGS  = @CHECK_CFLAGS@ -I$(top_builddir)/include
check_ref_LDADD   = $(top_builddir)/src/libcfe.la $(TESTS_LDFLAGS)

check_keystore_SOURCES = check_keystore.c
check_keystore_CFLAGS  = @CHECK_CFLAGS@ -I$(top_builddir)/include
check_keystore_LDADD   = $(top_builddir)/src/libcfe.la $(TESTS_LDFLAGS)

check_hash_SOURCES = check_hash.c
check_hash_CFLAGS  = @CHECK_CFLAGS@ -I$(top_builddir)/include
check_hash_LDADD   = $(top_builddir)/src/libcfe.la $(TESTS_LDFLAGS)

check_create_SOURCES = check_create.c
check_create_CFLAGS  = @CHECK_CFLAGS@ -I$(top_builddir)/include
check_create_LDADD   = $(top_builddir)/src/libcfe.la $(TESTS_LDFLAGS)
