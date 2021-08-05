#include "AbcFile.hpp"
#include "AbcConstants.hpp"
#include "common.hpp"
#include <string.h>

// v_o = vector offset, calculates vector index from pointer
template <typename T>
static inline int v_o(const std::vector<T> &vec, const T *const ptr)
{
	return static_cast<int>(ptr - &vec[0]);
}

void abc::AbcFile::write_constant_pool(BufferWriter &writer) const
{
	writer.u30(int_pool.size());
	for (unsigned i = 1; i < int_pool.size(); ++i) {
		writer.s30(int_pool[i]);
	}
	writer.u30(uint_pool.size());
	for (unsigned i = 1; i < uint_pool.size(); ++i) {
		writer.u30(uint_pool[i]);
	}
	writer.u30(double_pool.size());
	for (unsigned i = 1; i < double_pool.size(); ++i) {
		writer << double_pool[i];
	}
	writer.u30(string_pool.size());
	for (unsigned i = 1; i < string_pool.size(); ++i) {
		writer.str(string_pool[i]);
	}
	writer.u30(namespace_pool.size());
	for (unsigned i = 1; i < namespace_pool.size(); ++i) {
		const Namespace &ns = namespace_pool[i];
		writer << ns.kind;
		writer.u30(v_o(string_pool, ns.name));
	}
	writer.u30(namespace_set_pool.size());
	for (unsigned i = 1; i < namespace_set_pool.size(); ++i) {
		const std::vector<Namespace*> &ns_set = namespace_set_pool[i];
		writer.u30(ns_set.size());
		for (unsigned j = 0; j < ns_set.size(); ++j) {
			writer.u30(v_o(namespace_pool, ns_set[j]));
		}
	}
	writer.u30(multiname_pool.size());
	for (unsigned i = 1; i < multiname_pool.size(); ++i) {
		const Multiname &mn = multiname_pool[i];
		writer << mn.kind;
		switch (mn.kind) {
		case CONSTANT_QName:
		case CONSTANT_QNameA:
			writer.u30(v_o(namespace_pool, mn.as_qname.ns));
			writer.u30(v_o(string_pool, mn.as_qname.name));
			break;
		case CONSTANT_RTQName:
		case CONSTANT_RTQNameA:
			writer.u30(v_o(string_pool, mn.as_rtqname.name));
			break;
		case CONSTANT_RTQNameL:
		case CONSTANT_RTQNameLA:
			break;
		case CONSTANT_Multiname:
		case CONSTANT_MultinameA:
			writer.u30(v_o(string_pool, mn.as_qnameset.name));
			writer.u30(v_o(namespace_set_pool, mn.as_qnameset.ns));
			break;
		case CONSTANT_MultinameL:
		case CONSTANT_MultinameLA:
			writer.u30(v_o(namespace_set_pool, mn.as_qnameset_l.ns));
			break;
		case CONSTANT_Typename: {
			writer.u30(v_o(multiname_pool, mn.as_qtypename.qname));
			const std::vector<uint32_t> *something = mn.as_qtypename.something;
			writer.u30(something->size());
			for (unsigned j = 0; j < something->size(); ++j) {
				writer.u30((*something)[j]);
			}
			break;
		}
		default:
			trace(+mn.kind);
			fatal("illegal multiname kind");
		}
	}
}

void abc::AbcFile::write_method_signatures(BufferWriter &writer) const
{
	writer.u30(method_pool.size());
	for (unsigned i = 0; i < method_pool.size(); ++i) {
		const Method &method = method_pool[i];
		writer.u30(method.params.size());
		writer.u30(v_o(multiname_pool, method.return_type));
		for (unsigned j = 0; j < method.params.size(); ++j) {
			writer.u30(v_o(multiname_pool, method.params[j]));
		}
		writer.u30(v_o(string_pool, method.name));
		writer << method.flags;
		if (method.flags & HAS_OPTIONAL) {
			writer.u30(method.optional_params.size());
			for (unsigned j = 0; j < method.optional_params.size(); ++j) {
				const Method::Option &opt = method.optional_params[j];
				writer.u30(opt.value);
				writer << opt.kind;
			}
		}
	}
}

void abc::AbcFile::write_method_bodies(BufferWriter &writer) const
{
	uint32_t count = 0;
	for (unsigned i = 0; i < method_pool.size(); ++i) {
		const Method &method = method_pool[i];
		if (method.has_body)
			++count;
	}
	writer.u30(count);
	for (unsigned i = 0; i < method_pool.size(); ++i) {
		const Method &method = method_pool[i];
		if (! method.has_body)
			continue;
		writer.u30(i);
		writer.u30(method.max_stack);
		writer.u30(method.local_variable_count);
		writer.u30(method.min_scope_depth);
		writer.u30(method.max_scope_depth);
		writer.u30(method.code.size());
		for (unsigned j = 0; j < method.code.size(); ++j) {
			writer << method.code[j];
		}
		writer.u30(method.exceptions.size());
		for (unsigned j = 0; j < method.exceptions.size(); ++j) {
			const Method::ExceptionInfo &e = method.exceptions[j];
			writer.u30(e.from);
			writer.u30(e.to);
			writer.u30(e.target);
			writer.u30(v_o(multiname_pool, e.type));
			writer.u30(v_o(multiname_pool, e.var_name));
		}
		write_class_traits(writer, method.traits);
	}
}

void abc::AbcFile::write_classes(BufferWriter &writer) const
{
	writer.u30(class_pool.size());
	// instance information
	for (unsigned i = 0; i < class_pool.size(); ++i) {
		const Class &c = class_pool[i];
		writer.u30(v_o(multiname_pool, c.name));
		writer.u30(v_o(multiname_pool, c.super));
		writer <<c.flags;
		if (c.flags & CONSTANT_ProtectedNS) {
			writer.u30(v_o(namespace_pool, c.protected_ns));
		}
		writer.u30(c.interfaces.size());
		for (unsigned j = 0; j < c.interfaces.size(); ++j) {
			writer.u30(v_o(multiname_pool, c.interfaces[j]));
		}
		writer.u30(v_o(method_pool, c.ctor));
		write_class_traits(writer, c.traits);
	}
	// static information
	for (unsigned i = 0; i < class_pool.size(); ++i) {
		const Class &c = class_pool[i];
		writer.u30(v_o(method_pool, c.static_ctor));
		write_class_traits(writer, c.static_traits);
	}
}

void abc::AbcFile::write_class_traits(BufferWriter &writer, const std::vector<ClassTrait> &vec) const
{
	writer.u30(vec.size());
	for (unsigned i = 0; i < vec.size(); ++i) {
		const ClassTrait &trait = vec[i];
		writer.u30(v_o(multiname_pool, trait.name));
		writer << trait.kind;
		const uint8_t kind_high = trait.kind >> 4;
		const uint8_t kind_low = trait.kind & 0x0F;
		switch (kind_low) {
		case Trait_Slot:
		case Trait_Const:
			writer.u30(trait.as_slot.slot_id);
			writer.u30(v_o(multiname_pool, trait.as_slot.type_name));
			writer.u30(trait.as_slot.vindex);
			if (trait.as_slot.vindex) {
				writer << trait.as_slot.value_kind;
			}
			break;
		case Trait_Class:
			writer.u30(trait.as_class.slot_id);
			writer.u30(v_o(class_pool, trait.as_class.classi));
			break;
		case Trait_Function:
			writer.u30(trait.as_function.slot_id);
			writer.u30(v_o(method_pool, trait.as_function.method));
			break;
		case Trait_Method:
		case Trait_Getter:
		case Trait_Setter:
			writer.u30(trait.as_method.disp_id);
			writer.u30(v_o(method_pool, trait.as_method.method));
			break;
		default:
			trace(+kind_low);
			fatal("low bits of trait kind unknown");
		}
	}
}

void abc::AbcFile::write_to(BufferWriter &writer) const
{
	writer << file_flags;
	for (unsigned i = 0; i < file_label.size(); ++i) {
		writer << file_label[i];
	}
	writer.put(0);
	writer << version_minor;
	writer << version_major;
	write_constant_pool(writer);
	write_method_signatures(writer);
	// metadata count
	writer.u30(0);
	write_classes(writer);
	// write scripts
	{
		writer.u30(script_pool.size());
		for (auto &script : script_pool) {
			writer.u30(v_o(method_pool, script.entry_point));
			write_class_traits(writer, script.traits);
		}
	}
	write_method_bodies(writer);
}
