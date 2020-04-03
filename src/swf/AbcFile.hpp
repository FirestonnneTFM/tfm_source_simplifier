#pragma once

#include "common.hpp"
#include "Buffer.hpp"
#include "AbcMultiname.hpp"
#include "AbcMethod.hpp"
#include "AbcClass.hpp"

namespace abc {
	class AbcFile {
		void read_constant_pool(BufferReader &reader);
		void read_method_signatures(BufferReader &reader);
		void read_method_bodies(BufferReader &reader);
		void read_classes(BufferReader &reader);
		void read_class_traits(BufferReader &reader, std::vector<ClassTrait> &vec);
		void write_constant_pool(BufferWriter &writer) const;
		void write_method_signatures(BufferWriter &writer) const;
		void write_method_bodies(BufferWriter &writer) const;
		void write_classes(BufferWriter &writer) const;
		void write_class_traits(BufferWriter &writer, const std::vector<ClassTrait> &vec) const;
	public:
		uint32_t file_flags;
		std::string file_label;
		uint16_t version_minor;
		uint16_t version_major;
		std::vector<int32_t> int_pool;
		std::vector<uint32_t> uint_pool;
		std::vector<double> double_pool;
		std::vector<std::string> string_pool;
		std::vector<Namespace> namespace_pool;
		std::vector<std::vector<Namespace*>> namespace_set_pool;
		std::vector<Multiname> multiname_pool;
		std::vector<Method> method_pool;
		std::vector<Class> class_pool;
		std::vector<Script> script_pool;
		void write_to(BufferWriter &writer) const;
		AbcFile(BufferReader &reader);
	};
}
