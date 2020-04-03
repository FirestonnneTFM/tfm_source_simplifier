#include "AbcFile.hpp"
#include "AbcConstants.hpp"
#include "common.hpp"
#include <string.h>
#include <math.h>

void abc::AbcFile::read_constant_pool(BufferReader &reader)
{
	uint32_t count;
	count = reader.u30();
	count = count ? count : 1;
	int_pool.resize(count);
	int_pool[0] = 0;
	for (unsigned i = 1; i < count; ++i) {
		int_pool[i] = reader.s30();
	}
	count = reader.u30();
	count = count ? count : 1;
	uint_pool.resize(count);
	uint_pool[0] = 0;
	for (unsigned i = 1; i < count; ++i) {
		uint_pool[i] = reader.u30();
	}
	count = reader.u30();
	count = count ? count : 1;
	double_pool.resize(count);
	double_pool[0] = NAN;
	for (unsigned i = 1; i < count; ++i) {
		reader >> double_pool[i];
	}
	count = reader.u30();
	count = count ? count : 1;
	string_pool.resize(count);
	string_pool[0] = "";
	for (unsigned i = 1; i < count; ++i) {
		string_pool[i] = reader.str();
	}
	count = reader.u30();
	count = count ? count : 1;
	namespace_pool.resize(count);
	namespace_pool[0].kind = 0;
	namespace_pool[0].name = &string_pool[0];
	for (unsigned i = 1; i < count; ++i) {
		reader >> namespace_pool[i].kind;
		uint32_t string_index = reader.u30();
		namespace_pool[i].name = &string_pool[string_index];
	}
	count = reader.u30();
	count = count ? count : 1;
	namespace_set_pool.resize(count);
	for (unsigned i = 1; i < count; ++i) {
		const uint32_t n = reader.u30();
		namespace_set_pool[i].resize(n);
		for (unsigned j = 0; j < n; ++j) {
			const uint32_t index = reader.u30();
			if (index == 0)
				fatal("namespace set contained 0 (namespace *ANY)");
			namespace_set_pool[i][j] = &namespace_pool[index];
		}
	}
	count = reader.u30();
	count = count ? count : 1;
	multiname_pool.resize(count);
	multiname_pool[0].kind = CONSTANT_RTQName;
	multiname_pool[0].as_rtqname.name = &string_pool[0];
	for (unsigned i = 1; i < count; ++i) {
		Multiname &mn = multiname_pool[i];
		reader >> mn.kind;
		switch (mn.kind) {
		case CONSTANT_QName:
		case CONSTANT_QNameA:
			mn.as_qname.ns = &namespace_pool[reader.u30()];
			mn.as_qname.name = &string_pool[reader.u30()];
			break;
		case CONSTANT_RTQName:
		case CONSTANT_RTQNameA:
			mn.as_rtqname.name = &string_pool[reader.u30()];
			break;
		case CONSTANT_RTQNameL:
		case CONSTANT_RTQNameLA:
			break;
		case CONSTANT_Multiname:
		case CONSTANT_MultinameA:
			mn.as_qnameset.name = &string_pool[reader.u30()];
			mn.as_qnameset.ns = &namespace_set_pool[reader.u30()];
			break;
		case CONSTANT_MultinameL:
		case CONSTANT_MultinameLA:
			mn.as_qnameset_l.ns = &namespace_set_pool[reader.u30()];
			break;
		case CONSTANT_Typename: {
			mn.as_qtypename.qname = &multiname_pool[reader.u30()];
			const uint32_t param_count = reader.u30();
			mn.as_qtypename.something = new std::vector<uint32_t>(param_count);
			for (unsigned j = 0; j < param_count; ++j) {
				(*mn.as_qtypename.something)[j] = reader.u30();
			}
			break;
		}
		default:
			trace(+mn.kind);
			fatal("illegal multiname kind");
		}
	}
}

void abc::AbcFile::read_method_signatures(BufferReader &reader)
{
	// this is the number of method signatures
	// there can be more method signatures than bodies due to things like native methods
	const uint32_t count = reader.u30();
	method_pool.resize(count);
	for (unsigned i = 0; i < count; ++i) {
		Method &method = method_pool[i];
		const uint32_t param_count = reader.u30();
		method.return_type = &multiname_pool[reader.u30()];
		method.params.resize(param_count);
		for (unsigned j = 0; j < param_count; ++j) {
			method.params[j] = &multiname_pool[reader.u30()];
		}
		method.name = &string_pool[reader.u30()];
		reader >> method.flags;
		if (method.flags & HAS_OPTIONAL) {
			const uint32_t option_count = reader.u30();
			method.optional_params.resize(option_count);
			for (unsigned j = 0; j < option_count; ++j) {
				Method::Option &opt = method.optional_params[j];
				opt.value = reader.u30();
				reader >> opt.kind;
			}
		}
		if (method.flags & HAS_PARAM_NAMES) {
			// ignore all param names
			for (unsigned j = 0; j < param_count; ++j) {
				reader.u30();
			}
			// disable this flag
			method.flags &= ~HAS_PARAM_NAMES;
		}
		// fill in with true once we read the body
		method.has_body = false;
	}
}

void abc::AbcFile::read_method_bodies(BufferReader &reader)
{
	const uint32_t count = reader.u30();
	if (count > method_pool.size()) {
		trace(method_pool.size());
		trace(count);
		fatal("more method bodies than method signatures");
	}
	for (unsigned i = 0; i < count; ++i) {
		Method &method = method_pool[reader.u30()];
		method.has_body = true;
		method.max_stack = reader.u30();
		method.local_variable_count = reader.u30();
		method.min_scope_depth = reader.u30();
		method.max_scope_depth = reader.u30();
		const uint32_t code_len = reader.u30();
		method.code.resize(code_len);
		for (unsigned j = 0; j < code_len; ++j) {
			reader >> method.code[j];
		}
		const uint32_t exception_count = reader.u30();
		method.exceptions.resize(exception_count);
		for (unsigned j = 0; j < exception_count; ++j) {
			Method::ExceptionInfo &e = method.exceptions[j];
			e.from = reader.u30();
			e.to = reader.u30();
			e.target = reader.u30();
			e.type = &multiname_pool[reader.u30()];
			e.var_name = &multiname_pool[reader.u30()];
		}
		read_class_traits(reader, method.traits);
	}
}

void abc::AbcFile::read_classes(BufferReader &reader)
{
	const uint32_t count = reader.u30();
	class_pool.resize(count);
	// instance information
	for (unsigned i = 0; i < count; ++i) {
		Class &c = class_pool[i];
		c.name = &multiname_pool[reader.u30()];
		c.super = &multiname_pool[reader.u30()];
		reader >> c.flags;
		if (c.flags & CONSTANT_ProtectedNS) {
			c.protected_ns = &namespace_pool[reader.u30()];
		} else {
			c.protected_ns = nullptr;
		}
		const uint32_t interface_count = reader.u30();
		c.interfaces.resize(interface_count);
		for (unsigned j = 0; j < interface_count; ++j) {
			c.interfaces[j] = &multiname_pool[reader.u30()];
		}
		c.ctor = &method_pool[reader.u30()];
		read_class_traits(reader, c.traits);
	}
	// static information
	for (unsigned i = 0; i < count; ++i) {
		Class &c = class_pool[i];
		c.static_ctor = &method_pool[reader.u30()];
		read_class_traits(reader, c.static_traits);
	}
}

void abc::AbcFile::read_class_traits(BufferReader &reader, std::vector<ClassTrait> &vec)
{
	const uint32_t count = reader.u30();
	vec.resize(count);
	for (unsigned i = 0; i < count; ++i) {
		ClassTrait &trait = vec[i];
		trait.name = &multiname_pool[reader.u30()];
		reader >> trait.kind;
		const uint8_t kind_high = trait.kind >> 4;
		const uint8_t kind_low = trait.kind & 0x0F;
		if (kind_high & ATTR_Metadata) {
			fatal("trait attribute metadata unsupported");
		}
		switch (kind_low) {
		case Trait_Slot:
		case Trait_Const: {
			trait.as_slot.slot_id = reader.u30();
			trait.as_slot.type_name = &multiname_pool[reader.u30()];
			const uint32_t vindex = reader.u30();
			trait.as_slot.value = nullptr;
			trait.as_slot.value_kind = 0;
			trait.as_slot.vindex = vindex;
			if (vindex) {
				reader >> trait.as_slot.value_kind;
				switch (trait.as_slot.value_kind) {
				case CONSTANT_Int:
					trait.as_slot.value = &int_pool[vindex];
					break;
				case CONSTANT_UInt:
					trait.as_slot.value = &uint_pool[vindex];
					break;
				case CONSTANT_Utf8:
					trait.as_slot.value = &string_pool[vindex];
					break;
				case CONSTANT_NS:
				case CONSTANT_PackageNS:
				case CONSTANT_PackageInternalNS:
				case CONSTANT_ProtectedNS:
				case CONSTANT_ExplicitNS:
				case CONSTANT_StaticProtectedNS:
				case CONSTANT_PrivateNS:
					trait.as_slot.value = &namespace_pool[vindex];
					break;
				}
			}
			break;
		}
		case Trait_Class:
			trait.as_class.slot_id = reader.u30();
			trait.as_class.classi = &class_pool[reader.u30()];
			break;
		case Trait_Function:
			trait.as_function.slot_id = reader.u30();
			trait.as_function.method = &method_pool[reader.u30()];
			break;
		case Trait_Method:
		case Trait_Getter:
		case Trait_Setter:
			trait.as_method.disp_id = reader.u30();
			trait.as_method.method = &method_pool[reader.u30()];
			break;
		default:
			trace(+kind_low);
			fatal("low bits of trait kind unknown");
		}
	}
}

abc::AbcFile::AbcFile(BufferReader &reader)
{
	reader >> file_flags;
	file_label = reinterpret_cast<const char*>(reader.raw_ptr());
	reader += file_label.size() + 1;
	reader >> version_minor;
	reader >> version_major;
	if (version_major != 46 || version_minor != 16) {
		trace(version_major);
		trace(version_minor);
		fatal("suspicious ABC version");
	}
	read_constant_pool(reader);
	read_method_signatures(reader);
	// ignore meta data
	{
		const uint32_t meta_data_count = reader.u30();
		for (unsigned i = 0; i < meta_data_count; ++i) {
			// name
			reader.u30();
			const uint32_t item_count = reader.u30();
			// doubled in order to read both keys and values
			for (unsigned j = 0; j < item_count*2; ++j) {
				reader.u30();
			}
		}
	}
	read_classes(reader);
	// read scripts
	{
		const uint32_t script_count = reader.u30();
		script_pool.resize(script_count);
		for (unsigned i = 0; i < script_count; ++i) {
			Script &script = script_pool[i];
			script.entry_point = &method_pool[reader.u30()];
			read_class_traits(reader, script.traits);
		}
	}
	read_method_bodies(reader);
}
