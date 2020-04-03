#pragma once

#include "common.hpp"
#include <utility>
#include "AbcClass.hpp"

namespace abc {
	class Multiname;

	class Method {
	public:
		class Option {
		public:
			uint8_t kind;
			uint32_t value;
		};
		class ExceptionInfo {
		public:
			uint32_t from;
			uint32_t to;
			uint32_t target;
			Multiname *type;
			Multiname *var_name;
		};
		Multiname *return_type;
		std::vector<Multiname*> params;
		std::string *name;
		uint8_t flags;
		std::vector<Option> optional_params;
		bool has_body;
		uint32_t max_stack;
		uint32_t local_variable_count;
		uint32_t min_scope_depth;
		uint32_t max_scope_depth;
		Buffer code;
		std::vector<ExceptionInfo> exceptions;
		std::vector<ClassTrait> traits;
	};
}
