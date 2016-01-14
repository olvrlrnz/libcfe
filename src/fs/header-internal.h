#ifndef FS_HEADER_INTERNAL_H
#define FS_HEADER_INTERNAL_H


#define CFE_HEADER_IDENT		"LIBCFE-ENCRYPTED"
#define CFE_HEADER_IDENT_SIZE		16U


static inline uint32_t normalize_headersize(uint32_t size)
{
	unsigned long pagesize;

	pagesize = (unsigned long) sysconf(_SC_PAGESIZE);
	return size ? (size + (pagesize - 1)) & ~(pagesize- 1) : pagesize;
}

struct cfe_file_meta {
	char ident[CFE_HEADER_IDENT_SIZE];
	uint16_t version;
	uint32_t size;
};


#endif
