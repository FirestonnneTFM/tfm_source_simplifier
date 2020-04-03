#pragma once

#include "common.hpp"

namespace abc {
	constexpr uint8_t CONSTANT_NS = 0x08;
	constexpr uint8_t CONSTANT_PackageNS = 0x16;
	constexpr uint8_t CONSTANT_PackageInternalNS = 0x17;
	constexpr uint8_t CONSTANT_ProtectedNS = 0x18;
	constexpr uint8_t CONSTANT_ExplicitNS = 0x19;
	constexpr uint8_t CONSTANT_StaticProtectedNS = 0x1A;
	constexpr uint8_t CONSTANT_PrivateNS = 0x05;

	constexpr uint8_t CONSTANT_QName = 0x07;
	constexpr uint8_t CONSTANT_QNameA = 0x0D;
	constexpr uint8_t CONSTANT_RTQName = 0x0F;
	constexpr uint8_t CONSTANT_RTQNameA = 0x10;
	constexpr uint8_t CONSTANT_RTQNameL = 0x11;
	constexpr uint8_t CONSTANT_RTQNameLA = 0x12;
	constexpr uint8_t CONSTANT_Multiname = 0x09;
	constexpr uint8_t CONSTANT_MultinameA = 0x0E;
	constexpr uint8_t CONSTANT_MultinameL = 0x1B;
	constexpr uint8_t CONSTANT_MultinameLA = 0x1C;
	constexpr uint8_t CONSTANT_Typename = 0x1D;

	constexpr uint8_t NEED_ARGUMENTS = 0x01;
	constexpr uint8_t NEED_ACTIVATION = 0x02;
	constexpr uint8_t NEED_REST = 0x04;
	constexpr uint8_t HAS_OPTIONAL = 0x08;
	constexpr uint8_t SET_DXNS = 0x40;
	constexpr uint8_t HAS_PARAM_NAMES = 0x80;

	constexpr uint8_t CONSTANT_Int = 0x03;
	constexpr uint8_t CONSTANT_UInt = 0x04;
	constexpr uint8_t CONSTANT_Double = 0x06;
	constexpr uint8_t CONSTANT_Utf8 = 0x01;
	constexpr uint8_t CONSTANT_True = 0x0B;
	constexpr uint8_t CONSTANT_False = 0x0A;
	constexpr uint8_t CONSTANT_Null = 0x0C;

	constexpr uint8_t CONSTANT_ClassSealed = 0x01;
	constexpr uint8_t CONSTANT_ClassFinal = 0x02;
	constexpr uint8_t CONSTANT_ClassInterface = 0x04;
	constexpr uint8_t CONSTANT_ClassProtectedNS = 0x08;

	constexpr uint8_t Trait_Slot = 0;
	constexpr uint8_t Trait_Method = 1;
	constexpr uint8_t Trait_Getter = 2;
	constexpr uint8_t Trait_Setter = 3;
	constexpr uint8_t Trait_Class = 4;
	constexpr uint8_t Trait_Function = 5;
	constexpr uint8_t Trait_Const = 6;

	constexpr uint8_t ATTR_Final = 0x01;
	constexpr uint8_t ATTR_Override = 0x02;
	constexpr uint8_t ATTR_Metadata = 0x04;
}
