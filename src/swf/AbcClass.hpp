#pragma once

#include "common.hpp"
#include "AbcConstants.hpp"

namespace abc {
	class Multiname;
	class Namespace;
	class Class;
	class Method;

	class ClassTrait {
	public:
		Multiname *name;
		uint8_t kind;
		union {
			struct {
				uint32_t slot_id;
				Multiname *type_name;
				uint32_t vindex;
				uint8_t value_kind;
				void *value;
			} as_slot;
			struct {
				uint32_t slot_id;
				Class *classi;
			} as_class;
			struct {
				uint32_t slot_id;
				Method *method;
			} as_function;
			struct {
				uint32_t disp_id;
				Method *method;
			} as_method;
		};
		bool is_slot() const {
			return (kind & 0x0F) == Trait_Slot || (kind & 0x0F) == Trait_Const;
		}
		bool is_class() const {
			return (kind & 0x0F) == Trait_Class;
		}
		bool is_function() const {
			return (kind & 0x0F) == Trait_Function;
		}
		bool is_method() const {
			switch (kind & 0x0F) {
			case Trait_Method:
			case Trait_Getter:
			case Trait_Setter:
				return true;
			default:
				return false;
			}
		}
	};
	class Script {
	public:
		Method *entry_point;
		std::vector<ClassTrait> traits;
	};
	class Class {
	public:
		Multiname *name;
		Multiname *super;
		uint8_t flags;
		Namespace *protected_ns;
		std::vector<Multiname *> interfaces;
		Method *ctor;
		std::vector<ClassTrait> traits;
		Method *static_ctor;
		std::vector<ClassTrait> static_traits;
	};
};
