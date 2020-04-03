#pragma once

#include "common.hpp"

namespace abc {
	class Namespace {
	public:
		uint8_t kind;
		std::string *name;
	};
	class Multiname {
	public:
		uint8_t kind;
		union {
			struct {
				Namespace *ns;
				std::string *name;
			} as_qname;
			struct {
				std::string *name;
			} as_rtqname;
			struct {
				std::vector<Namespace*> *ns;
				std::string *name;
			} as_qnameset;
			struct {
				std::vector<Namespace*> *ns;
			} as_qnameset_l;
			struct {
				Multiname *qname;
				std::vector<uint32_t> *something;
			} as_qtypename;
		};
	};
}
