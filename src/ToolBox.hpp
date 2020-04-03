#pragma once

#include "swf/AbcFile.hpp"
#include "swf/AbcConstants.hpp"
#include "common.hpp"

class ToolBox {
	abc::AbcFile &abc_file;
	abc::Multiname *packet_out_mn = nullptr;
	abc::Multiname *packet_in_mn = nullptr;
	abc::Multiname *ui_el_mn = nullptr;
	abc::Multiname *coerce_mn = nullptr;
public:
	ToolBox(abc::AbcFile &l_abc_file)
		: abc_file(l_abc_file)
		{}
	void rename_by_inheritance();
	void rename_invalid_identifiers();
	void simplify_expressions();
};

//#define INVESTIGATE

static inline std::string *grab_name(abc::Multiname &mn)
{
	std::string *name = nullptr;
	switch (mn.kind) {
	case abc::CONSTANT_QName:
	case abc::CONSTANT_QNameA:
		name = mn.as_qname.name;
		break;
	case abc::CONSTANT_RTQName:
	case abc::CONSTANT_RTQNameA:
		name = mn.as_rtqname.name;
		break;
	case abc::CONSTANT_Multiname:
	case abc::CONSTANT_MultinameA:
		name = mn.as_qnameset.name;
		break;
	}
	return name;
}
