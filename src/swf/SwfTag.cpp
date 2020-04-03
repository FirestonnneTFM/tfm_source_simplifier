#include "SwfTag.hpp"
#include "Buffer.hpp"
#include <cstring>

swf::SwfTag::SwfTag(BufferReader &reader)
{
	uint16_t header;
	reader >> header;
	kind = header >> 6;
	uint32_t length = header & 63;
	if (length == 0x3F) {
		reader >> length;
	}
	if (length) {
		buffer.resize(length);
		std::memcpy(&buffer[0], reader.raw_ptr(), length);
		reader += length;
	}
}

uint32_t swf::SwfTag::get_write_to_size() const
{
	uint32_t header_size = 2;
	if (buffer.size() >= 0x3F) {
		header_size += 4;
	}
	return buffer.size() + header_size;
}

void swf::SwfTag::write_to(BufferWriter &writer) const
{
	uint16_t header = kind << 6;
	if (buffer.size() >= 0x3F) {
		header |= 0x3F;
		writer << header;
		writer << static_cast<uint32_t>(buffer.size());
	} else {
		header |= buffer.size();
		writer << header;
	}
	for (unsigned i = 0; i < buffer.size(); ++i) {
		writer << buffer[i];
	}
}
