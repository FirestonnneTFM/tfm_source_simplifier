#include "ToolBox.hpp"
#include "common.hpp"
#ifdef sdkfjsdfksd
#include <cstring>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <tuple>
#include "MaxBipartiteMatch.hpp"

enum class MatchResult {
	NoMatch,
	CompleteMatch,
	PartialMatch
};

static const char *match_result_string(const MatchResult mr)
{
	switch (mr) {
	case MatchResult::NoMatch:
		return "NoMatch";
	case MatchResult::CompleteMatch:
		return "CompleteMatch";
	case MatchResult::PartialMatch:
		return "PartialMatch";
	default:
		return "UnknownMatchResult";
	}
}

struct MethodMatch {
	default() = delete;
	std::string method_name;
	std::vector<std::string> param_types;
	std::string return_type;
};

struct ClassMatch {
	default() = delete;
	std::string class_name;
	std::string super_class;
	MethodMatch ctor;
	std::vector<MethodMatch> static_methods;
	std::vector<MethodMatch> methods;
	std::vector<std::string> static_var_types;
	std::vector<std::string> var_types;
};

static MatchResult string_matches(const std::string *const target, const std::string &against)
{
	if (target == nullptr)
		return MatchResult::NoMatch;
	if (is_string_invalid(*target))
		return MatchResult::PartialMatch;
	return (*target == against) ? MatchResult::CompleteMatch :  MatchResult::NoMatch;
}

static MatchResult method_matches(abc::Method *m, const MethodMatch &match, abc::Multiname *class_name)
{
	if (m == nullptr)
		return MatchResult::NoMatch;
	const std::vector<std::string> &match_params = std::get<1>(match);
	if (m->params.size() != match_params.size())
		return MatchResult::NoMatch;
	const std::string &match_return_type = std::get<2>(match);
	MatchResult ret;
	if (match_return_type == "self")
		ret = (m->return_type == class_name) ? MatchResult::CompleteMatch : MatchResult::NoMatch;
	else
		ret = string_matches(grab_name(*m->return_type), match_return_type);
	if (ret == MatchResult::NoMatch)
		return MatchResult::NoMatch;
	for (size_t i = 0; i < m->params.size(); ++i) {
		abc::Multiname *const p = m->params[i];
		if (p == nullptr)
			return MatchResult::NoMatch;
		const MatchResult res = string_matches(grab_name(*p), match_params[i]);
		switch (res) {
		case MatchResult::NoMatch:
			return MatchResult::NoMatch;
		case MatchResult::PartialMatch:
			ret = MatchResult::PartialMatch;
			break;
		}
	}
	return ret;
}

static void count_traits(const std::vector<abc::ClassTrait> &traits, int &method_count, std::vector<std::string> &var_types)
{
	for (const auto &trait : traits) {
		if (trait.is_method())
			++method_count;
		else if (trait.is_slot()) {
			abc::Multiname *const mn = trait.as_slot.type_name;
			bool needs_default = true;
			if (mn) {
				std::string *const name = grab_name(*mn);
				if (name) {
					var_types.emplace_back(*name);
					needs_default = false;
				}
			}
			if (needs_default) {
				var_types.emplace_back("");
			}
		}
	}
}

// note: first parameter is not const, second is
static bool type_vectors_equal(std::vector<std::string> &a, const std::vector<std::string> &const_b)
{
	if (a.size() != const_b.size())
		return false;
	constexpr const char *b2ObjectName = "123";
	std::vector<std::string> b(const_b);
	for (auto &item : a) {
		if (is_string_invalid(item)) {
			item = b2ObjectName;
		}
	}
	for (auto &item : b) {
		if (item.size() >= 2 && item[0] == 'b' && item[1] == '2') {
			item = b2ObjectName;
		}
	}
	std::sort(a.begin(), a.end());
	std::sort(b.begin(), b.end());
	return a == b;
}

static MatchResult class_matches(abc::Class &c, const ClassMatch &cm)
{
	if (c.super == nullptr)
		return MatchResult::NoMatch;
	int num_static_methods = 0;
	int num_methods = 0;
	std::vector<std::string> static_var_types;
	std::vector<std::string> var_types;
	count_traits(c.static_traits, num_static_methods, static_var_types);
	if (num_static_methods != std::get<3>(cm))
		return MatchResult::NoMatch;
	count_traits(c.traits, num_methods, var_types);
	if (num_methods != std::get<4>(cm))
		return MatchResult::NoMatch;
	if (! type_vectors_equal(static_var_types, std::get<5>(cm)))
		return MatchResult::NoMatch;
	if (! type_vectors_equal(var_types, std::get<6>(cm)))
		return MatchResult::NoMatch;
	MatchResult ret = string_matches(grab_name(*c.super), std::get<1>(cm));
	if (ret == MatchResult::NoMatch)
		return MatchResult::NoMatch;
	ret = method_matches(c.ctor, std::get<2>(cm), c.name);
	if (ret == MatchResult::NoMatch)
		return MatchResult::NoMatch;
	return ret;
}

#define PRINT_UNMATCHED

void ToolBox::rename_box2d()
{
	// b2Mat22 = Object_aQ
	const ClassMatch cms[] {
#include "TB_rename_box2d_table.hpp"
	};
	constexpr int num_cms = ARR_SIZE(cms);
	const int num_classes = abc_file.class_pool.size();
	MaxBipartiteMatch mbm(num_cms, num_classes);
	for (int l = 0; l < num_cms; ++l) {
		const ClassMatch &cm = cms[l];
#ifdef PRINT_UNMATCHED
		int num_matched_to = 0;
#endif
		for (int r = 0; r < num_classes; ++r) {
			abc::Class &c = abc_file.class_pool[r];
			const MatchResult res = class_matches(c, cm);
			if (res != MatchResult::NoMatch) {
				mbm.connect(l, r);
#ifdef PRINT_UNMATCHED
				++num_matched_to;
#endif
			}
		}
#ifdef PRINT_UNMATCHED
		if (num_matched_to == 0) {
			TRACE << "unmatched " << std::get<0>(cm) << std::endl;
		}
#endif
	}
	std::vector<std::pair<int, int>> mbm_results;
	const int mbm_result = mbm.solve(&mbm_results);
	if (mbm_result != mbm_results.size()) {
		trace(mbm_result);
		trace(mbm_results.size());
		TRACE << "WARNING: Max bipartite match results disagree" << std::endl;
	}
	for (const auto &lr : mbm_results) {
		const std::string &expected_name = std::get<0>(cms[lr.first]);
		abc::Class &c = abc_file.class_pool[lr.second];
		*grab_name(*c.name) = std::string("A_") + expected_name;
	}
	TRACE << "renamed " << mbm_result << " / " << num_cms << " Box2D classes" << std::endl;
}
/*
19 / 63: Box2DFlashAS3_1.4.2
22 / 66: Box2DFlashAS3_1.4.3
22 / 66: Box2DFlashAS3_1.4.3.1
46 / 74: Box2DFlashAS3_2.0.0
58 / 76: Box2DFlashAS3_2.0.1
58 / 76: Box2DFlashAS3_2.0.2
40 / 87: box2d_git
Box2DFlashAS3_2.1a
Box2DFlashAS3_2.1a_f9
*/

/*
First commit 128 - 0x80 was seen in b2DebugDraw.as -> baff063a6bc55b1dc462b7c26fbd6ed17c42d7e1 "port of r182 June 13 2009"
Removed in 550e47cf50b26d671bf0fd4450f5b5e6e28d38c0 "MAJOR CHANGES Sep 30 2009"
*/
#else
void ToolBox::rename_box2d()
{
	TRACE << "currently broken" << std::endl;
}
#endif