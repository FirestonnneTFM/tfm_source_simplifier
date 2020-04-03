#include "Buffer.hpp"

uint32_t BufferReader::u30()
{
	int i;
	uint32_t value = 0;
	int pos = 0;
	int count = 0;
	bool next;
	do {
		i = *ptr;
		++ptr;
		next = (i >> 7) == 1;
		i &= 0x7F;
		value += (i << pos);
		++count;
		pos += 7;
	} while (next && count < 5);
	return value;
}

std::string BufferReader::str()
{
	uint32_t len = u30();
	std::string value(len, '#');
	for (unsigned i = 0; i < len; ++i, ++ptr) {
		value[i] = *ptr;
	}
	return value;
}

void BufferWriter::u30(uint32_t value)
{
	do {
		*ptr = static_cast<uint8_t>(value & 0x7F);
		value >>= 7;
		if (value)
			*ptr |= 0x80;
		++ptr;
	} while (value);
}

void BufferWriter::str(const std::string &value)
{
	u30(value.size());
	for (unsigned i = 0; i < value.size(); ++i, ++ptr) {
		*ptr = value[i];
	}
}
