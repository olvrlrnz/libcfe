#include "utils.h"

static char byte_map[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
static int byte_map_len = sizeof(byte_map);

static unsigned char nibble_from_char(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 255;
}
static char nibble_to_char(unsigned char nibble)
{
	if (nibble < byte_map_len)
		return byte_map[nibble];
	else
		return '*';
}

void hex_str_to_bytes(const char *in, unsigned char *out)
{
	unsigned char *p;
	int len, i;

	len = strlen(in) / 2;
	for (i = 0, p = (unsigned char *) in; i < len; i++) {
		out[i] = (nibble_from_char(*p) << 4) | nibble_from_char(*(p + 1));
		p += 2;
	}
	out[len] = 0;
}

void bytes_to_hex_str(const unsigned char *in, size_t size, char *out)
{
	int i;

	for(i = 0; i < size; ++i) {
		out[2 * i] = nibble_to_char(in[i] >> 4);
		out[2 * i + 1] = nibble_to_char(in[i] & 0x0F);
	}

	out[i] = '\0';
}
