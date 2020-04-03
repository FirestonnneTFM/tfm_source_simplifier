#pragma once

#include "common.hpp"
#include "Buffer.hpp"

namespace swf {
	class SwfTag {
		public:
		Buffer buffer;
		uint16_t kind;
		SwfTag(BufferReader &reader);
		uint32_t get_write_to_size() const;
		void write_to(BufferWriter &writer) const;
	};
}
