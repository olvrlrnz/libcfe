check_PROGRAMS = check_filewriter check_ref check_keystore check_hash check_create
TESTS = $(check_PROGRAMS)


TESTS_CFLAGS  = @CMOCKA_CFLAGS@ -I$(top_builddir)/include
TESTS_LDADD   = @CMOCKA_LIBS@ $(top_builddir)/src/libcfe.la -lkeyutils -lcrypto


check_filewriter_SOURCES = check_filewriter.c
check_filewriter_CFLAGS  = $(TESTS_CFLAGS)
check_filewriter_LDADD   = $(TESTS_LDADD)

check_ref_SOURCES = check_ref.c
check_ref_CFLAGS  = $(TESTS_CFLAGS)
check_ref_LDADD   = $(TESTS_LDADD)

check_keystore_SOURCES = check_keystore.c
check_keystore_CFLAGS  = $(TESTS_CFLAGS)
check_keystore_LDADD   = $(TESTS_LDADD)

check_hash_SOURCES = check_hash.c
check_hash_CFLAGS  = $(TESTS_CFLAGS)
check_hash_LDADD   = $(TESTS_LDADD)

check_create_SOURCES = check_create.c
check_create_CFLAGS  = $(TESTS_CFLAGS)
check_create_LDADD   = $(TESTS_LDADD)
