#include "common.hpp"
#include "SwfFile.hpp"
#include <fstream>
#include <zlib.h>

template <typename T>
static inline void read_b(std::ifstream &file, T &value)
{
	file.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
static inline void write_b(std::ofstream &file, const T &value)
{
	file.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

static inline void read_buffer(std::ifstream &file, Buffer &buf)
{
	file.read(reinterpret_cast<char*>(&buf[0]), buf.size());
}

static inline void write_buffer(std::ofstream &file, const Buffer &buf)
{
	file.write(reinterpret_cast<const char*>(&buf[0]), buf.size());
}

swf::SwfFile::SwfFile(const char *const fname)
{
	std::ifstream file;
	file.open(fname, std::ios::binary | std::ios::ate);
	if (! file.is_open())
		fatal("could not open swf file");
	// 8 is the size of information not compressed
	const int compressed_size = static_cast<int>(file.tellg()) - 8;
	file.close();
	file.open(fname, std::ios::binary);
	char signature[3];
	file.read(signature, 3);
	bool should_decompress = false;
	bool valid_signature = signature[1] == 'W' && signature[2] == 'S';
	switch (signature[0]) {
	case 'F':
		should_decompress = false;
		break;
	case 'C':
		should_decompress = true;
		break;
	case 'Z':
		fatal("lzma swf decompression not available");
		break;
	default:
		valid_signature = false;
		break;
	}
	if (! valid_signature) {
		fatal("improper signature in swf header");
	}
	read_b(file, swf_version);
	uint32_t decompressed_size;
	read_b(file, decompressed_size);
	Buffer backing(decompressed_size);
	if (should_decompress) {
		Buffer in_data(decompressed_size);
		read_buffer(file, in_data);
		z_stream stream;
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		stream.avail_in = static_cast<uInt>(compressed_size);
		stream.next_in = static_cast<Bytef*>(&in_data[0]);
		stream.avail_out = decompressed_size;
		stream.next_out = static_cast<Bytef*>(&backing[0]);
		inflateInit(&stream);
		inflate(&stream, Z_NO_FLUSH);
		inflateEnd(&stream);
	} else {
		trace(compressed_size);
		trace(decompressed_size);
		fatal("check if this is right");
		backing.resize(compressed_size);
		read_buffer(file, backing);
	}
	BufferReader reader(backing);
	// read frame size
	frame_size.clear();
	uint16_t bit_size = reader.peek() >> 3;
	bit_size = bit_size*4 + 5;
	for ( ; bit_size < 200; bit_size -= 8) {
		frame_size.push_back(reader.peek());
		reader += 1;
	}
	reader >> frame_rate;
	reader >> frame_count;
	for ( ; ; ) {
		swf::SwfTag tag(reader);
		tags.push_back(tag);
		if (tag.kind == 0)
			break;
	}
}

void swf::SwfFile::write_to(const char *const fname) const
{
	std::ofstream file(fname, std::ios::binary);
	file.write("FWS", 3);
	write_b(file, swf_version);
	// frame_size + frame_rate + frame_count
	uint32_t backing_buffer_size = frame_size.size() + 2 + 2;
	for (unsigned i = 0; i < tags.size(); ++i) {
		backing_buffer_size += tags[i].get_write_to_size();
	}
	Buffer backing(backing_buffer_size);
	BufferWriter writer(backing);
	for (unsigned i = 0; i < frame_size.size(); ++i) {
		writer << frame_size[i];
	}
	writer << frame_rate;
	writer << frame_count;
	for (unsigned i = 0; i < tags.size(); ++i) {
		tags[i].write_to(writer);
	}
	write_b(file, static_cast<uint32_t>(backing.size()));
	write_buffer(file, backing);
}

void swf::SwfFile::find_tags(uint16_t kind, std::vector<swf::SwfTag*> &out_tags)
{
	for (unsigned i = 0; i < tags.size(); ++i) {
		if (tags[i].kind == kind) {
			out_tags.push_back(&tags[i]);
		}
	}
}
