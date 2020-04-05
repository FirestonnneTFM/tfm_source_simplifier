#include "ToolBox.hpp"
#include <set>
#include <tuple>
#include <initializer_list>

class ClassProps {
private:
	void init(const std::vector<abc::ClassTrait> &traits, int &method_count, int &var_count, int &arg_sum, int &unique_arg_types, int &unique_return_types, int &unique_var_types);
public:
	static constexpr int N = 12;
	int backing[N];
	abc::Multiname *const name;
	abc::Multiname *const super;
	ClassProps(abc::Class &c);
	ClassProps(std::initializer_list<int> list);
	int compare(const ClassProps &rhs) const;
	bool operator< (const ClassProps &rhs) const {
		return compare(rhs) < 0;
	}
	bool operator== (const ClassProps &rhs) const {
		return compare(rhs) == 0;
	}
};

static inline std::ostream &operator<< (std::ostream &o, const ClassProps &rhs)
{
	o << '{' << rhs.backing[0];
	for (int i = 1; i < ClassProps::N; ++i) {
	o << ", " << rhs.backing[i];
	}
	o << '}';
	return o;
}

ClassProps::ClassProps(abc::Class &c)
	: name(c.name), super(c.super)
{

	constexpr int half = N / 2;
	init(c.traits, backing[0], backing[1], backing[2], backing[3], backing[4], backing[5]);
	init(c.static_traits, backing[half], backing[half + 1], backing[half + 2], backing[half + 3], backing[half + 4], backing[half + 5]);
}

ClassProps::ClassProps(std::initializer_list<int> list)
	: name(nullptr), super(nullptr)
{
	int i = 0;
	for (auto item : list) {
		if (i == N)
			return;
		backing[i] = item;
		++i;
	}
}

void ClassProps::init(const std::vector<abc::ClassTrait> &traits, int &method_count, int &var_count, int &arg_sum, int &unique_arg_types, int &unique_return_types, int &unique_var_types)
{
	method_count = 0;
	var_count = 0;
	arg_sum = 0;
	std::set<const abc::Multiname *> arg_types;
	std::set<const abc::Multiname *> return_types;
	std::set<const abc::Multiname *> var_types;
	for (auto trait : traits) {
		if (trait.is_method()) {
			++method_count;
			const abc::Method *const method = trait.as_method.method;
			arg_sum += method->params.size();
			for (auto item : method->params) {
				arg_types.insert(item);
			}
			return_types.insert(method->return_type);
		}
		if (trait.is_slot())
			++var_count;
	}
	unique_arg_types = arg_types.size();
	unique_return_types = return_types.size();
	unique_var_types = var_types.size();
}

int ClassProps::compare(const ClassProps &rhs) const
{
	for (int i = 0; i < N; ++i) {
		const int diff = backing[i] - rhs.backing[i];
		if (diff)
			return diff;
	}
	return 0;
}

static void print_props_if(const ClassProps &p, const char *const str, const char *const looking_for, const bool is_base)
{
	const std::string &s = *grab_name(*p.name);
	if (s == str) {
		std::cout << "else if (p == ClassProps(" << p << ")) {" << std::endl;
		std::cout << "new_name = \"" << looking_for << "\";" << std::endl;
		if (is_base) {
			std::cout << "S." << looking_for << " = p.name;" << std::endl;
		}
		std::cout << '}' << std::endl;
	}
}

class Box2DState {
public:
	std::set<ClassProps> set;
	bool log(ClassProps props) {
		auto iter = set.find(props);
		if (iter == set.end()) {
			set.insert(props);
			return true;
		} else {
			TRACE << *grab_name(*props.name) << " looks same as " << *grab_name(*iter->name) << std::endl;
			return false;
		}
	}
	using MN = abc::Multiname *;
	MN Object = nullptr;
	MN b2JointDef = nullptr;
};

static void try_rename_box2d_class(Box2DState &S, const ClassProps &p)
{
	// Common/Math
	//
	//print_props_if(p, "Object_bp", "b2Mat22", false);
	//print_props_if(p, "s_class_0115", "b2Math", false);
	print_props_if(p, "????", "b2Sweep", false);
	//print_props_if(p, "Object_ag", "b2Vec2", false);
	//print_props_if(p, "Object_ar", "b2XForm", false);
#ifdef CHUNK
	// Dynamics/Joints
	//
	print_props_if(p, "Object_ao", "b2JointDef", true);
	print_props_if(p, "Object_ao_ab", "joint_def_0", false);
	print_props_if(p, "Object_ao_ac", "joint_def_1", false);
	print_props_if(p, "Object_ao_ad", "joint_def_2", false);
	print_props_if(p, "Object_ao_ae", "joint_def_3", false);
	print_props_if(p, "Object_ao_af", "b2MouseJointDef", false);
#endif
	const char *new_name = nullptr;
	std::string &old_name = *grab_name(*p.name);
	if (p.super == S.Object) {
		if (p == ClassProps({19, 2, 12, 3, 4, 0, 1, 0, 2, 1, 1, 0})) {
			new_name = "b2Vec2";
		}
		else if (p == ClassProps({3, 2, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0})) {
			new_name = "b2XForm";
		}
		else if (p == ClassProps({0, 0, 0, 0, 0, 0, 31, 3, 56, 6, 6, 0})) {
			new_name = "b2Math";
		}
		else if (p == ClassProps({11, 2, 9, 3, 4, 0, 0, 0, 0, 0, 0, 0})) {
			new_name = "b2Mat22";
		}
	} else if (p.super == S.b2JointDef) {
		if (p == ClassProps({1, 9, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0})) {
			new_name = "joint_def_0";
		}
		else if (p == ClassProps({1, 10, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0})) {
			new_name = "joint_def_1";
		}
		else if (p == ClassProps({1, 9, 7, 3, 1, 0, 0, 0, 0, 0, 0, 0})) {
			new_name = "joint_def_2";
		}
		else if (p == ClassProps({0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) {
			new_name = "joint_def_3";
		}
		else if (p == ClassProps({0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})) {
			new_name = "b2MouseJointDef";
		}
	}
	if (new_name && S.log(p)) {
		old_name = std::string("A_") + std::string(new_name);
	}
}

void ToolBox::rename_by_traits()
{
	TRACE << "looking for box2D classes" << std::endl;
	Box2DState state;
	state.Object = object_mn;
	std::vector<ClassProps> all_class_props;
	for (auto c : abc_file.class_pool) {
		all_class_props.emplace_back(c);
	}
	// two passes required (one for each level of inheritance)
	for (int i = 0; i < 2; ++i) {
		state.set.clear();
		TRACE << "PASS " << i << std::endl;
		for (auto item : all_class_props) {
			try_rename_box2d_class(state, item);
		}
	}
	TRACE << "finished looking for box2D classes" << std::endl;
}
