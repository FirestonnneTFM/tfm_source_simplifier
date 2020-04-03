#pragma once

#include "SwfTag.hpp"

namespace swf {
	class SwfFile {
	private:
		std::vector<SwfTag> tags;
		uint8_t swf_version;
		Buffer frame_size;
		uint16_t frame_rate;
		uint16_t frame_count;
	public:
		SwfFile(const char *const fname);
		void write_to(const char *const fname) const;
		void find_tags(uint16_t kind, std::vector<SwfTag*> &out_tags);
	};
}
