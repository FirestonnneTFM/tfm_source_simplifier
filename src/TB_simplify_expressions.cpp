#include "ToolBox.hpp"
#include "swf/AbcOpcode.hpp"
#include "swf/AbcConstants.hpp"
#include "Buffer.hpp"
#include <utility>
#include <map>
#include <set>
#include <cstring>
#include <math.h>
#include <variant>

class Variant {
private:
	std::variant<int32_t, std::string> cont;
	bool m_is_bool;
public:
	Variant(std::string s)
		: cont(s), m_is_bool(false) {}
	Variant(int32_t n)
		: cont(n), m_is_bool(false) {}
	bool is_str() const {
		return std::holds_alternative<std::string>(cont);
	}
	bool is_i32() const {
		return std::holds_alternative<int32_t>(cont);
	}
	bool is_bool() const {
		return m_is_bool;
	}
	void to_bool() {
		if (m_is_bool)
			return;
		m_is_bool = true;
		if (is_str()) {
			cont = (as_str() == "") ? 0 : 1;
		}
	}
	std::string &as_str() {
		return std::get<std::string>(cont);
	}
	int32_t &as_i32() {
		return std::get<int32_t>(cont);
	}
	const std::string &as_str() const {
		return std::get<std::string>(cont);
	}
	const int32_t &as_i32() const {
		return std::get<int32_t>(cont);
	}
	bool operator== (const Variant &rhs) const {
		if (is_i32() && rhs.is_i32())
			return as_i32() == rhs.as_i32();
		if (is_str() && rhs.is_i32())
			return as_str() == rhs.as_str();
		return false;
	}
};

class Stack {
public:
	std::vector<Variant> vec;
	void push(Variant value) {
		vec.push_back(value);
	}
	Variant &top() {
		if (vec.empty())
			throw 1;
		return vec.back();
	}
	Variant pop() {
		const Variant value = top();
		vec.pop_back();
		return value;
	}
};

union OpArg {
	uint32_t u30;
	int32_t s30;
	int32_t s24;
	uint8_t byte;
	int8_t sbyte;
};

class OpPeeker {
	const uint8_t *const base_ptr;
	const int code_len;
	BufferReader reader;
public:
	OpPeeker(BufferReader l_reader, int l_code_len)
		: base_ptr(l_reader.raw_ptr()), code_len(l_code_len), reader(l_reader) {}
	uint8_t next(OpArg *args, const int arg_count);
	const uint8_t *raw_ptr() const {
		return reader.raw_ptr();
	}
};

class DFile;

class DStaticClass {
private:
	bool m_successful_static_init;
public:
	std::map<abc::Multiname *, Variant> traits;
	bool successful_static_init() const {
		return m_successful_static_init;
	}
	bool find_trait(abc::Multiname *mn, Variant &out_value) const;
	DStaticClass(abc::Class *cl, DFile &dfile);
};

class DFile {
private:
	template<typename T>
	void generic_init(std::map<T, uint32_t> &map, const std::vector<T> &vec);
	template<typename T>
	uint32_t generic_lookup(std::map<T, uint32_t> &map, std::map<T, uint32_t> &map_ext, const std::vector<T> &vec, T key);
	template<typename T>
	void generic_writeback(std::map<T, uint32_t> &map_ext, std::vector<T> &vec);
	std::map<int32_t, uint32_t> i32_map;
	std::map<uint32_t, uint32_t> u32_map;
	std::map<double, uint32_t> f64_map;
	std::map<std::string, uint32_t> str_map;
	std::map<int32_t, uint32_t> i32_map_ext;
	std::map<uint32_t, uint32_t> u32_map_ext;
	std::map<double, uint32_t> f64_map_ext;
	std::map<std::string, uint32_t> str_map_ext;
	uint32_t lookup_i32(int32_t value);
	uint32_t lookup_u32(uint32_t value);
	uint32_t lookup_f64(double value);
	uint32_t lookup_str(std::string value);
public:
	abc::AbcFile &abc_file;
	DFile(abc::AbcFile &l_abc_file);
	void write_back();
	void reload();
	//void process_method(BufferReader &reader, BufferWriter &writer, const int code_len);
	void swap_static_vars(BufferReader &reader, BufferWriter &writer, const int code_len, std::map<abc::Multiname *, DStaticClass> &static_classes, abc::Multiname *coerce_mn, const std::set<abc::Multiname *> &bad_methods);
	void remove_static_write_entries(Buffer &input, std::map<abc::Multiname *, DStaticClass> &static_classes, std::map<abc::Multiname *, abc::Multiname *> &trait_map);
	void process_packet_out(BufferReader &reader, const int code_len, abc::Multiname *mn);
};

uint8_t OpPeeker::next(OpArg *args, const int arg_count)
{
	if (reader.raw_ptr() - base_ptr >= code_len)
		return 0;
	uint8_t op_code;
	reader >> op_code;
	int index = 0;
	for (const char *s = abc::OP::arg[op_code]; *s; ++s) {
		switch (*s) {
		case abc::OP::arg_byte:
			reader >> args[index].byte;
			break;
		case abc::OP::arg_sbyte:
			reader >> args[index].sbyte;
			break;
		case abc::OP::arg_u30:
			args[index].u30 = reader.u30();
			break;
		case abc::OP::arg_s30:
			args[index].s30 = reader.s30();
			break;
		case abc::OP::arg_jump_offset:
			args[index].s24 = reader.s24();
			break;
		case abc::OP::arg_case_offset: {
			const uint32_t count = reader.u30() + 1;
			for (unsigned i = 0; i < count; ++i) {
				args[index].s24 = reader.s24();
			}
			break;
		}
		default:
			trace(+op_code);
			trace(abc::OP::name[op_code]);
			trace(*s);
			trace(+*s);
			fatal("unknown arg");
		}
		if (index != arg_count - 1)
			++index;
	}
	return op_code;
}

template <typename T>
void DFile::generic_init(std::map<T, uint32_t> &map, const std::vector<T> &vec)
{
	map.clear();
	for (unsigned i = 1; i < vec.size(); ++i) {
		map.emplace(vec[i], i);
	}
}

DFile::DFile(abc::AbcFile &l_abc_file)
	: abc_file(l_abc_file)
{
	reload();
}

void DFile::reload()
{
	i32_map_ext.clear();
	u32_map_ext.clear();
	f64_map_ext.clear();
	str_map_ext.clear();
	generic_init(i32_map, abc_file.int_pool);
	generic_init(u32_map, abc_file.uint_pool);
	generic_init(f64_map, abc_file.double_pool);
	generic_init(str_map, abc_file.string_pool);
}

template<typename T>
void DFile::generic_writeback(std::map<T, uint32_t> &map_ext, std::vector<T> &vec)
{
	vec.resize(vec.size() + map_ext.size());
	for (auto item : map_ext) {
		const uint32_t i = item.second;
		const T value = item.first;
		if (i >= vec.size()) {
			trace(i);
			trace(vec.size());
			fatal("index is out of range of the constant pool");
		}
		vec[i] = value;
	}
}

void DFile::write_back()
{
	generic_writeback(i32_map_ext, abc_file.int_pool);
	generic_writeback(u32_map_ext, abc_file.uint_pool);
	generic_writeback(f64_map_ext, abc_file.double_pool);
	generic_writeback(str_map_ext, abc_file.string_pool);
}

template <typename T>
uint32_t DFile::generic_lookup(std::map<T, uint32_t> &map, std::map<T, uint32_t> &map_ext, const std::vector<T> &vec, T key)
{
	auto iter = map.find(key);
	if (iter != map.end())
		return iter->second;
	auto iter_ext = map_ext.find(key);
	if (iter_ext != map_ext.end())
		return iter_ext->second;
	const uint32_t value = vec.size() + map_ext.size();
	map_ext.emplace(key, value);
	return value;
}

uint32_t DFile::lookup_i32(int32_t value)
{
	return generic_lookup(i32_map, i32_map_ext, abc_file.int_pool, value);
}

uint32_t DFile::lookup_u32(uint32_t value)
{
	return generic_lookup(u32_map, u32_map_ext, abc_file.uint_pool, value);
}

uint32_t DFile::lookup_f64(double value)
{
	return generic_lookup(f64_map, f64_map_ext, abc_file.double_pool, value);
}

uint32_t DFile::lookup_str(std::string value)
{
	return generic_lookup(str_map, str_map_ext, abc_file.string_pool, value);
}

#ifdef sdfkdsjfkd
void DFile::process_method(BufferReader &reader, BufferWriter &writer, const int code_len)
{
	OpPeeker peeker(reader, code_len);
	for ( ; ; ) {
		OpArg op_arg;
		const uint8_t op_code = peeker.next(&op_arg, 1);
		switch (op_code) {
		case 0:
			return;
		case abc::OP_pushbyte: {
			writer << abc::OP_pushbyte;
			const int8_t my_byte = op_arg.sbyte;
			writer << my_byte;
			reader += peeker.raw_ptr() - reader.raw_ptr();
			break;
		}
		case abc::OP_pushshort: {
			writer << abc::OP_pushshort;
			const int32_t my_short = op_arg.s30;
			writer.s30(my_short);
			reader += peeker.raw_ptr() - reader.raw_ptr();
			break;
		}
		case abc::OP_pushint: {
			writer << abc::OP_pushint;
			const int32_t my_int = abc_file.int_pool[op_arg.u30];
			writer.u30(lookup_i32(my_int));
			reader += peeker.raw_ptr() - reader.raw_ptr();
			break;
		}
		case abc::OP_pushuint: {
			writer << abc::OP_pushuint;
			const int32_t my_uint = abc_file.uint_pool[op_arg.u30];
			writer.u30(lookup_u32(my_uint));
			reader += peeker.raw_ptr() - reader.raw_ptr();
			break;
		}
		case abc::OP_pushdouble: {
			writer << abc::OP_pushdouble;
			const double my_double = abc_file.double_pool[op_arg.u30];
			writer.u30(lookup_f64(my_double));
			reader += peeker.raw_ptr() - reader.raw_ptr();
			break;
		}
		default: {
			const int diff = peeker.raw_ptr() - reader.raw_ptr();
			for (int i = 0; i < diff; ++i) {
				uint8_t b;
				reader >> b;
				writer << b;
			}
		}
		}
	}
}
#endif

void DFile::swap_static_vars(BufferReader &reader, BufferWriter &writer, const int code_len, std::map<abc::Multiname *, DStaticClass> &static_classes, abc::Multiname *coerce_mn, const std::set<abc::Multiname *> &bad_methods)
{
	OpPeeker peeker(reader, code_len);
	OpPeeker lookahead(reader, code_len);
	OpArg next_op_arg[2];
	OpArg op_arg[2];
	lookahead.next(next_op_arg, 2);
	for ( ; ; ) {
		const uint8_t op_code = peeker.next(op_arg, 2);
		const uint8_t next_op_code = lookahead.next(next_op_arg, 2);
		if (op_code == 0)
			return;
		bool flush_instruction = true;
		int nops_to_write = 0;
		if (op_code == abc::OP_getlex && (next_op_code == abc::OP_getproperty || next_op_code == abc::OP_callproperty)) {
			abc::Multiname *const class_mn = &abc_file.multiname_pool[op_arg[0].u30];
			abc::Multiname *const trait_mn = &abc_file.multiname_pool[next_op_arg[0].u30];
			auto iter = static_classes.find(class_mn);
			if (iter != static_classes.end()) {
				Variant my_value = 0;
				if (iter->second.find_trait(trait_mn, my_value)) {
					const uint8_t *const base_ptr = writer.raw_ptr();
					if (my_value.is_bool()) {
						if (my_value.as_i32()) {
							writer << abc::OP_pushtrue;
						}
						else {
							writer << abc::OP_pushfalse;
						}
					} else if (my_value.is_i32()) {
						writer << abc::OP_pushint;
						writer.u30(lookup_i32(my_value.as_i32()));
					} else {
						writer << abc::OP_pushstring;
						writer.u30(lookup_str(my_value.as_str()));
					}
					const int write_size = writer.raw_ptr() - base_ptr;
					const int space_available = lookahead.raw_ptr() - reader.raw_ptr();
					nops_to_write = space_available - write_size;
					reader += space_available;
					peeker.next(op_arg, 2);
					lookahead.next(next_op_arg, 2);
					flush_instruction = false;
				}
			}
		} else if (op_code == abc::OP_getlex && &abc_file.multiname_pool[op_arg[0].u30] == coerce_mn) {
			const int space_available = peeker.raw_ptr() - reader.raw_ptr();
			reader += space_available;
			nops_to_write = space_available;
			flush_instruction = false;
		} else if (op_code == abc::OP_callproperty) {
			abc::Multiname *property_name = &abc_file.multiname_pool[op_arg[0].u30];
			if (bad_methods.find(property_name) != bad_methods.end()) {
				const int space_available = peeker.raw_ptr() - reader.raw_ptr();
				reader += space_available;
				nops_to_write = space_available;
				flush_instruction = false;
			}
		}
		// flush instruction keeps the same instruction, byte for byte
		// otherwise, nops are added to pad it (this keeps the offsets the
		// same for jumps and try-catch etc
		if (flush_instruction) {
			const int diff = peeker.raw_ptr() - reader.raw_ptr();
			for (int i = 0; i < diff; ++i) {
				uint8_t b;
				reader >> b;
				writer << b;
			}
		} else {
			if (nops_to_write < 0) {
				trace(nops_to_write);
				fatal("nops to write negative");
			}
			while (nops_to_write > 0) {
				writer << abc::OP_nop;
				--nops_to_write;
			}
		}
	}
}

void DFile::remove_static_write_entries(Buffer &input, std::map<abc::Multiname *, DStaticClass> &static_classes, std::map<abc::Multiname *, abc::Multiname *> &trait_map)
{
	if (input.empty())
		return;
	BufferReader reader(input);
	OpPeeker peeker(reader, input.size());
	OpArg op_arg;
	for ( ; ; ) {
		const uint8_t op_code = peeker.next(&op_arg, 1);
		if (op_code == 0)
			return;
		if (op_code == abc::OP_setproperty) {
			abc::Multiname *const mn = &abc_file.multiname_pool[op_arg.u30];
			auto iter = trait_map.find(mn);
			if (iter != trait_map.end()) {
				auto iter2 = static_classes.find(iter->second);
				if (iter2 != static_classes.end()) {
					auto iter3 = iter2->second.traits.find(mn);
					if (iter3 != iter2->second.traits.end()) {
						iter2->second.traits.erase(iter3);
					}
				}
			}
		}
	}
}

bool DStaticClass::find_trait(abc::Multiname *mn, Variant &out_value) const
{
	auto iter = traits.find(mn);
	if (iter == traits.end())
		return false;
	out_value = iter->second;
	return true;
}

static bool do_math_ops(DFile &dfile, Stack &stack, const uint8_t op_code, OpArg *op_arg)
{
	switch (op_code) {
	case abc::OP_pushbyte:
		stack.push(op_arg[0].sbyte);
		break;
	case abc::OP_pushshort:
		stack.push(op_arg[0].s30);
		break;
	case abc::OP_pushint:
		stack.push(dfile.abc_file.int_pool[op_arg[0].u30]);
		break;
	case abc::OP_pushuint:
		stack.push(dfile.abc_file.uint_pool[op_arg[0].u30]);
		break;
	case abc::OP_pushstring:
		stack.push(dfile.abc_file.string_pool[op_arg[0].u30]);
		break;
	case abc::OP_pushfalse:
	case abc::OP_pushnull:
		stack.push(0);
		break;
	case abc::OP_pushtrue:
		stack.push(1);
		break;
	case abc::OP_not: {
		Variant v = stack.pop();
		if (v.is_i32()) {
			if (v.as_i32())
				stack.push(0);
			else
				stack.push(1);
		} else {
			stack.push(0);
		}
		break;
	}
	case abc::OP_equals:
		if (stack.pop() == stack.pop())
			stack.push(1);
		else
			stack.push(0);
		break;
	case abc::OP_dup:
		stack.push(stack.top());
		break;
	case abc::OP_pop:
		stack.pop();
		break;
	case abc::OP_add: {
		Variant rhs = stack.pop();
		Variant lhs = stack.pop();
		if (lhs.is_i32() && rhs.is_i32()) {
			stack.push(lhs.as_i32() + rhs.as_i32());
		} else if (lhs.is_i32()) {
			stack.push(std::to_string(lhs.as_i32()) + rhs.as_str());
		} else if (rhs.is_i32()) {
			stack.push(lhs.as_str() + std::to_string(rhs.as_i32()));
		} else {
			// both strings
			stack.push(lhs.as_str() + rhs.as_str());
		}
		break;
	}
	case abc::OP_multiply:
		stack.push(stack.pop().as_i32()*stack.pop().as_i32());
		break;
	case abc::OP_divide: {
		const int32_t rhs = stack.pop().as_i32();
		const int32_t lhs = stack.pop().as_i32();
		if (lhs == 0 || rhs == 0)
			stack.push(0);
		else
			stack.push(lhs / rhs);
		break;
	}
	case abc::OP_nop:
	case abc::OP_label:
		// do nothing;
		return true;
	default:
		return false;
	}
	return true;
}

DStaticClass::DStaticClass(abc::Class *cl, DFile &dfile)
{
	m_successful_static_init = true;
	try {
		const Buffer &code = cl->static_ctor->code;
		if (code.empty()) {
			m_successful_static_init = false;
			return;
		}
		BufferReader reader(code);
		OpPeeker peeker(reader, code.size());
		Stack stack;
		bool not_done = true;
		while (not_done) {
			OpArg op_arg[2];
			const uint8_t op_code = peeker.next(op_arg, 2);
			if (do_math_ops(dfile, stack, op_code, op_arg)) {
				continue;
			}
			switch (op_code) {
			case abc::OP_returnvoid:
				not_done = false;
				break;
			case abc::OP_setproperty:
			case abc::OP_initproperty: {
				abc::Multiname *const mn = &dfile.abc_file.multiname_pool[op_arg[0].u30];
				traits.emplace(mn, stack.pop());
				break;
			}
			case abc::OP_callproperty: {
				abc::Multiname *const mn = &dfile.abc_file.multiname_pool[op_arg[0].u30];
				if (*grab_name(*mn) == "Boolean") {
					stack.top().to_bool();
				}
				break;
			}
			case abc::OP_getproperty:
				stack.push(0);
				break;
			case abc::OP_getlocal0:
			case abc::OP_pushscope:
			case abc::OP_findproperty:
			case abc::OP_findpropstrict:
			case abc::OP_getlex:
				// do nothing
				break;
			default:
				m_successful_static_init = false;
				not_done = false;
			}
		}
	} catch (int) {
		m_successful_static_init = false;
	}
	if (! m_successful_static_init)
		return;
	for (auto t : cl->static_traits) {
		abc::Method *method = nullptr;
		if (t.is_function()) {
			method = t.as_function.method;
		} else if (t.is_method()) {
			method = t.as_method.method;
		} else {
			continue;
		}
		const Buffer &code = method->code;
		if (code.empty())
			continue;
		BufferReader reader(code);
		OpPeeker peeker(reader, code.size());
		Stack stack;
		bool not_done = true;
		try {
			while (not_done) {
				OpArg op_arg;
				const uint8_t op_code = peeker.next(&op_arg, 1);
				if (do_math_ops(dfile, stack, op_code, &op_arg)) {
					continue;
				}
				switch (op_code) {
				case abc::OP_getlocal0:
				case abc::OP_pushscope:
					// do nothing
					break;
				case abc::OP_returnvalue:
					traits.emplace(t.name, stack.pop());
					not_done = false;
					break;
				default:
					not_done = false;
				}
			}
		} catch (int) {
		}
	}
}


#ifdef sdfkkdsfj
static void disassemble_all(DFile &dfile)
{
	for (unsigned i = 0; i < dfile.abc_file.method_pool.size(); ++i) {
		Buffer &input = dfile.abc_file.method_pool[i].code;
		if (input.empty())
			continue;
		BufferReader reader(input);
		Buffer output(input.size() * 2);
		BufferWriter writer(output);
		dfile.process_method(reader, writer, input.size());
		output.resize(writer.raw_ptr() - &output[0]);
		input = std::move(output);
	}
}
#endif

void DFile::process_packet_out(BufferReader &reader, const int code_len, abc::Multiname *mn)
{
	OpPeeker peeker(reader, code_len);
	Stack stack;
	for ( ; ; ) {
		OpArg op_arg;
		const uint8_t op_code = peeker.next(&op_arg, 1);
		if (do_math_ops(*this, stack, op_code, &op_arg)) {
			continue;
		}
		switch (op_code) {
		case abc::OP_returnvoid:
			return;
		case abc::OP_pushscope:
		case abc::OP_coerce:
		case abc::OP_coerce_s:
		case abc::OP_coerce_a:
		case abc::OP_getlocal:
		case abc::OP_getlocal0:
		case abc::OP_getlocal1:
		case abc::OP_getlocal2:
		case abc::OP_getlocal3:
			// do nothing
			break;
		case abc::OP_setlocal:
		case abc::OP_setlocal0:
		case abc::OP_setlocal1:
		case abc::OP_setlocal2:
		case abc::OP_setlocal3:
			stack.pop();
			break;
		case abc::OP_constructsuper: {
			if (op_arg.u30 != 2) {
				trace(op_arg.u30);
				fatal("constructsuper without arg count = 2");
			}
			const int32_t CC = stack.pop().as_i32();
			const int32_t C = stack.pop().as_i32();
			*grab_name(*mn) = std::string("PacketOut_") + std::to_string(C) + std::string("_") + std::to_string(CC);
			return;
		}
		default:
			trace(*grab_name(*mn));
			trace(abc::OP::name[op_code]);
			fatal("unhandled opcode when processing packet");
		}
	}
}

void ToolBox::simplify_expressions()
{
	TRACE << "disassembling" << std::endl;
	DFile dfile(abc_file);
	std::map<abc::Multiname *, DStaticClass> static_classes;
	std::set<abc::Multiname *> bad_methods;
	for (unsigned i = 0; i < abc_file.class_pool.size(); ++i) {
		abc::Class *const c = &abc_file.class_pool[i];
		if (c->name == coerce_mn) {
			for (auto trait : c->static_traits) {
				bad_methods.insert(trait.name);
			}
		} else if (c->traits.empty() && !c->static_traits.empty()) {
			DStaticClass dsc(c, dfile);
			if (dsc.successful_static_init())
				static_classes.emplace(c->name, dsc);
		}
	}
	trace(bad_methods.size());
	{
		std::map<abc::Multiname *, abc::Multiname *> trait_map;
		for (auto cl : static_classes) {
			for (auto tr : cl.second.traits) {
				trait_map.emplace(tr.first, cl.first);
			}
		}
		for (unsigned i = 0; i < dfile.abc_file.class_pool.size(); ++i) {
			abc::Class &c = dfile.abc_file.class_pool[i];
			for (unsigned j = 0; j < c.traits.size(); ++j) {
				abc::ClassTrait &trait = c.traits[j];
				abc::Method *method = nullptr;
				if (trait.is_function())
					method = trait.as_function.method;
				else if (trait.is_method())
					method = trait.as_method.method;
				else
					continue;
				Buffer &input = method->code;
				dfile.remove_static_write_entries(input, static_classes, trait_map);
			}
		}
	}
	for (unsigned i = 0; i < dfile.abc_file.method_pool.size(); ++i) {
		Buffer &input = dfile.abc_file.method_pool[i].code;
		if (input.empty())
			continue;
		BufferReader reader(input);
		Buffer output(input.size() * 2);
		BufferWriter writer(output);
		dfile.swap_static_vars(reader, writer, input.size(), static_classes, coerce_mn, bad_methods);
		output.resize(writer.raw_ptr() - &output[0]);
		input = std::move(output);
	}
	dfile.write_back();
	dfile.reload();
	for (auto c : abc_file.class_pool) {
		if (c.super == packet_out_mn) {
			Buffer &input = c.ctor->code;
			BufferReader reader(input);
			dfile.process_packet_out(reader, input.size(), c.name);
		}
	}
	dfile.write_back();
	TRACE << "finished disassembling" << std::endl;
}
