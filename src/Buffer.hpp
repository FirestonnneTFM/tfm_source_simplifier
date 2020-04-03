#pragma once

#include "common.hpp"
#include <cstring>

using Buffer = std::vector<uint8_t>;

class BufferPosition;

class BufferReader {
	friend class BufferPosition;
private:
	const uint8_t *ptr;
public:
	BufferReader(BufferReader &reader)
		: ptr(reader.ptr) {}
	BufferReader(const Buffer &buf)
		: ptr(&buf[0]) {}
	BufferReader(const uint8_t *l_ptr)
		: ptr(l_ptr) {}
	void operator += (const int amt) {
		ptr += amt;
	}
	uint8_t peek() const {
		return *ptr;
	}
	template <typename T>
	BufferReader &operator >> (T &v) {
		// this is an alias violation
		// v = *reinterpret_cast<const T*>(ptr);
		std::memcpy(&v, ptr, sizeof(T));
		ptr += sizeof(T);
		return *this;
	}
	const uint8_t *raw_ptr() const {
		return ptr;
	}
	int32_t s24() {
		int32_t value = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16);
		ptr += 3;
		return value;
	}
	uint32_t u30();
	int32_t s30() {
		return static_cast<int32_t>(u30());
	}
	std::string str();
};

class BufferWriter {
	friend class BufferPosition;
private:
	uint8_t *ptr;
public:
	BufferWriter(Buffer &buf)
		: ptr(&buf[0]) {}
	BufferWriter(uint8_t *l_ptr)
		: ptr(l_ptr) {}
	void put(uint8_t value) {
		*ptr = value;
		++ptr;
	}
	template <typename T>
	BufferWriter &operator << (const T &v) {
		// this is an alias violation
		// *reinterpret_cast<T*>(ptr) = v;
		std::memcpy(ptr, &v, sizeof(T));
		ptr += sizeof(T);
		return *this;
	}
	const uint8_t *raw_ptr() const {
		return ptr;
	}
	void s24(const int32_t value) {
		const uint8_t *const v = reinterpret_cast<const uint8_t*>(&value);
		ptr[0] = v[0];
		ptr[1] = v[1];
		ptr[2] = v[2];
		ptr += 3;
	}
	void u30(uint32_t value);
	void s30(int32_t value) {
		u30(static_cast<uint32_t>(value));
	}
	void str(const std::string &value);
};

class BufferPosition {
private:
	const uint8_t *ptr;
public:
	BufferPosition()
		: ptr(nullptr) {}
	BufferPosition(const Buffer &buf)
		: ptr(&buf[0]) {}
	BufferPosition(const uint8_t *l_ptr)
		: ptr(l_ptr) {}
	BufferPosition(const BufferReader &reader)
		: ptr(reader.ptr) {}
	BufferReader read() {
		return BufferReader(ptr);
	}
	const uint8_t *raw_ptr() const {
		return ptr;
	}
	bool is_null() const {
		return ptr == nullptr;
	}
};
