#include "ToolBox.hpp"
#include "common.hpp"
#include <cstring>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <tuple>

static inline char digit(int n)
{
	return static_cast<char>(n) + '0';
}

static inline char letter(int n)
{
	if (n < 26)
		return static_cast<char>(n) + 'a';
	else
		return static_cast<char>(n - 26) + 'A';
}

static bool is_character_valid(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == '$';
}

static bool is_invalid(const std::string &s)
{
	for (auto &c : s) {
		if (! is_character_valid(c))
			return true;
	}
	return false;
}

class NameGenerator {
	std::string plate;
	int offset;
	int count;
public:
	NameGenerator(const std::string &prefix)
		:
		plate(prefix),
		offset(prefix.size()),
		count(0)
		{
			for (int i = 0; i < 4; ++i) {
				plate += '#';
		}
	}
	std::string next() {
		if (count >= 9999) {
			trace(plate);
			fatal("too many names to generate, need more leading zeros");
		}
		plate[offset] = digit(count / 1000 % 10);
		plate[offset + 1] = digit(count / 100 % 10);
		plate[offset + 2] = digit(count / 10 % 10);
		plate[offset + 3] = digit(count % 10);
		++count;
		return plate;
	}
	int get_count() const {
		return count;
	}
};

static inline void rename_if_invalid(abc::Multiname &mn, NameGenerator &gen)
{
	std::string *const name = grab_name(mn);
	if (name && is_invalid(*name)) {
		*name = gen.next();
	}
}

static inline void make_single_tag(int n, char *out_chars)
{
	// 2704 = 52^2
	if (n >= 2704) {
		trace(n);
		fatal("n is too large");
	}
	out_chars[0] = letter(n / 52);
	out_chars[1] = letter(n % 52);
	out_chars[2] = '\0';
}

static inline std::string make_tag(const std::vector<std::string> &prefixes, const std::vector<int> &vec)
{
	std::string ret = "";
	for (auto &v : vec) {
		if (v < 0) {
			const int index = -v - 1;
			if (index < 0 || index >= static_cast<int>(prefixes.size())) {
				trace(v);
				trace(index);
				trace(prefixes.size());
				fatal("index out of range");
			}
			ret += prefixes[index];
		} else {
			char s[3];
			make_single_tag(v, s);
			ret += '_';
			ret += s;
		}
	}
	return ret;
}

void ToolBox::rename_invalid_identifiers()
{
	{
		using MN_ptr = abc::Multiname *;
		std::map<MN_ptr, std::vector<MN_ptr>> g;
		std::set<MN_ptr> top_level_names;
		for (auto &c : abc_file.class_pool) {
			// ignore static classes
			if (c.traits.empty() && !c.static_traits.empty())
				continue;
			g[c.super].push_back(c.name);
			top_level_names.insert(c.super);
		}
		for (auto &c : abc_file.class_pool) {
			auto iter = top_level_names.find(c.name);
			if (iter != top_level_names.end()) {
				top_level_names.erase(iter);
			}
		}
		std::vector<std::pair<MN_ptr, std::vector<int>>> stack;
		std::vector<std::string> prefixes(top_level_names.size());
		{
			int i = 0;
			for (auto &mn : top_level_names) {
				std::string *name = grab_name(*mn);
				if (name == nullptr || name->empty())
					continue;
				prefixes[i] = *name;
				std::vector<int> vec;
				vec.push_back(-i - 1);
				stack.push_back(std::make_pair(mn, vec));
				++i;
			}
		}
		TRACE << "found " << prefixes.size() << " base classes" << std::endl;
		while (! stack.empty()) {
			MN_ptr mn = stack.back().first;
			std::vector<int> list = stack.back().second;
			std::string &old_name = *grab_name(*mn);
			if (is_invalid(old_name)) {
				old_name = make_tag(prefixes, list);
			}
			stack.pop_back();
			auto iter = g.find(mn);
			if (iter == g.end())
				continue;
			int i = 0;
			list.push_back(0);
			for (auto &item : iter->second) {
				list.back() = i;
				stack.push_back(std::make_pair(item, list));
				++i;
			}
		}
	}
	NameGenerator class_gen("class_");
	NameGenerator s_class_gen("s_class_");
	NameGenerator interface_gen("interface_");
	NameGenerator var_gen("var_");
	NameGenerator s_var_gen("s_var_");
	// rename all the classes first, just in case there are classes that are
	// traits (then they would get var_ names)
	for (auto &c : abc_file.class_pool) {
		NameGenerator *gen;
		if (c.flags & abc::CONSTANT_ClassInterface) {
			gen = &interface_gen;
		} else if (c.traits.empty() && !c.static_traits.empty()) {
			gen = &s_class_gen;
		} else {
			gen = &class_gen;
		}
		rename_if_invalid(*c.name, *gen);
	}
	for (auto &c : abc_file.class_pool) {
		for (auto &trait : c.traits) {
			rename_if_invalid(*trait.name, var_gen);
		}
		for (auto &trait : c.static_traits) {
			rename_if_invalid(*trait.name, s_var_gen);
		}
	}
	TRACE << "out of " << abc_file.class_pool.size() << " total classes..." << std::endl;
	TRACE << "...renamed " << s_class_gen.get_count() << " static classes" << std::endl;
	TRACE << "...renamed " << interface_gen.get_count() << " interfaces" << std::endl;
	TRACE << "...renamed " << class_gen.get_count() << " other improperly named classes" << std::endl;
	TRACE << "renamed " << var_gen.get_count() << " vars" << std::endl;
	TRACE << "renamed " << s_var_gen.get_count() << " static vars" << std::endl;
	NameGenerator name_gen("name_");
	for (auto &mn : abc_file.multiname_pool) {
		rename_if_invalid(mn, name_gen);
	}
	TRACE << "renamed " << name_gen.get_count() << " other names" << std::endl;
}

static bool is_coerce_class(abc::Class &c)
{
	if (! c.traits.empty() || c.static_traits.empty())
		return false;
	for (auto &trait : c.static_traits) {
		abc::Method *method = nullptr;
		if (trait.is_method())
			method = trait.as_method.method;
		else
			return false;
		if (method->params.size() != 1)
			return false;
		if (method->params[0] != method->return_type)
			return false;
	}
	return true;
}

static void rename_traits_rules(bool is_methods, std::vector<abc::ClassTrait> &traits, const std::vector<std::pair<std::string, std::string>> &name_type_list)
{
	std::vector<int> counts(name_type_list.size());
	for (auto &trait : traits) {
		if (is_methods) {
			if (! trait.is_method())
				continue;
		} else {
			if (! trait.is_slot())
				continue;
		}
		abc::Multiname *mn = is_methods ? trait.as_method.method->return_type : trait.as_slot.type_name;
		std::string *s = grab_name(*mn);
		if (s == nullptr)
			continue;
		for (size_t i = 0; i < name_type_list.size(); ++i) {
			const auto &kv = name_type_list[i];
			if (*s == kv.second) {
				std::string &trait_name_str = *grab_name(*trait.name);
				trait_name_str = kv.first;
				if (counts[i]) {
					trait_name_str += '_';
					trait_name_str += std::to_string(counts[i]);
				}
				++counts[i];
			}
		}
	}
}

static void rename_PacketOut_methods(abc::Class &c)
{
	// tuple: my name, param count, param 1 name, return name is this class
	using MyTuple = std::tuple<std::string, int, std::string, bool>;
	const MyTuple lookup[] {
		{ "writeBytes", 1, "ByteArray", true },
		{ "writeBoolean", 1, "Boolean", true },
		{ "writeString", 1, "String", true },
		{ "blockCipher", 2, "ByteArray", true },
		{ "xorCipher", 1, "int", false }
	};
	constexpr int lookup_size = ARR_SIZE(lookup);
	for (auto &trait : c.traits) {
		if (! trait.is_method())
			continue;
		abc::Method *m = trait.as_method.method;
		for (int i = 0; i < lookup_size; ++i) {
			const MyTuple &l = lookup[i];
			if (m->params.size() != std::get<1>(l))
				continue;
			if (std::get<3>(l) != (m->return_type == c.name))
				continue;
			if (*grab_name(*m->params[0]) != std::get<2>(l))
				continue;
			*grab_name(*trait.name) = std::get<0>(l);
		}
	}
	rename_traits_rules(false, c.traits, {
		{ "buffer", "ByteArray" },
		{ "needsXor", "Boolean" }
	});
}

static void rename_PacketIn_methods(abc::Class &c)
{
	rename_traits_rules(true, c.static_traits, {
		{ "read_leb128", "int" },
		{ "read_uleb128", "uint" }
	});
	rename_traits_rules(true, c.traits, {
		{ "getCompressedString", "String" },
		{ "getCompressedByteArray", "ByteArray" }
	});
}

void ToolBox::rename_by_inheritance()
{
	using Pair = std::pair<int, abc::Multiname *>;
	std::vector<Pair> graph;
	{
		std::map<abc::Multiname *, int> freq;
		for (auto &c : abc_file.class_pool) {
			++freq[c.super];
		}
		graph.resize(freq.size());
		unsigned i = 0;
		for (auto &item : freq) {
			graph[i] = std::make_pair(item.second, item.first);
			++i;
		}
	}
	if (graph.size() <= 10) {
		trace(graph.size());
		fatal("size of search graph is too small");
	}
	std::sort(graph.begin(), graph.end(), std::greater<Pair>());
#ifdef INVESTIGATE
	for (unsigned i = 0; i < 20; ++i) {
		TRACE << *grab_name(*graph[i].second) << " : " << graph[i].first << std::endl;
	}
#endif
	packet_out_mn = graph[1].second;
	packet_in_mn = graph[2].second;
	if (*grab_name(*graph[4].second) == "_PaquetSortant") {
		ui_el_mn = graph[5].second;
	} else {
		ui_el_mn = graph[4].second;
	}
	*grab_name(*packet_out_mn) = "PacketOut";
	*grab_name(*packet_in_mn) = "PacketIn";
	*grab_name(*ui_el_mn) = "UiElement";
	NameGenerator packet_out_gen("packet_out_");
	NameGenerator packet_in_gen("packet_in_");
	NameGenerator ui_el_gen("ui_element_");
	for (auto &c : abc_file.class_pool) {
		if (c.super == packet_out_mn) {
			*grab_name(*c.name) = packet_out_gen.next();
		} else if (c.super == packet_in_mn) {
			*grab_name(*c.name) = packet_in_gen.next();
		} else if (c.super == ui_el_mn) {
			*grab_name(*c.name) = ui_el_gen.next();
		} else if (is_coerce_class(c)) {
			*grab_name(*c.name) = "CoerceClass";
			coerce_mn = c.name;
		} else if (c.name == packet_out_mn) {
			rename_PacketOut_methods(c);
		} else if (c.name == packet_in_mn) {
			rename_PacketIn_methods(c);
		}
	}
	TRACE << "renamed " << packet_out_gen.get_count() << " classes that inherit from PacketOut" << std::endl;
	TRACE << "renamed " << packet_in_gen.get_count() << " classes that inherit from PacketIn" << std::endl;
	TRACE << "renamed " << ui_el_gen.get_count() << " classes that inherit from UiElement" << std::endl;
}