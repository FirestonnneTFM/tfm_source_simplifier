#include "common.hpp"
#include "swf/SwfFile.hpp"
#include "swf/SwfTag.hpp"
#include "swf/AbcFile.hpp"
#include "Buffer.hpp"
#include <utility>
#include "ToolBox.hpp"

struct TestScope {
	~TestScope() {
		fatal("end of test scope");
	}
};

static swf::SwfTag &find_do_abc(swf::SwfFile &tfm)
{
	std::vector<swf::SwfTag*> search;
	// 82 is DoABC tag
	tfm.find_tags(82, search);
	if (search.empty())
		fatal("file does not have a DoABC tag");
	if (search.size() > 1)
		fatal("file has more than one DoABC tag?");
	return *search[0];
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		trace(argc);
		fatal("needs two arguments, input file and output file");
	}
	const char *const input_fname = argv[1];
	const char *const output_fname = argv[2];
	TRACE << "reading file" << std::endl;
	swf::SwfFile tfm(input_fname);
	TRACE << "finished reading file" << std::endl;
	swf::SwfTag &do_abc_tag = find_do_abc(tfm);
	BufferReader abc_reader(do_abc_tag.buffer);
	abc::AbcFile actionscript(abc_reader);
	{
		ToolBox tb(actionscript);
		tb.rename_by_inheritance();
		tb.rename_invalid_identifiers();
		tb.simplify_expressions();
#ifdef INVESTIGATE
		TRACE << "INVESTIGATING INHERITANCE" << std::endl;
		tb.rename_by_inheritance();
		TRACE << "... DONE INVESTIGATING INHERITANCE" << std::endl;
#endif
	}
	TRACE << "old DoABC size: " << do_abc_tag.buffer.size() << std::endl;
	{
		Buffer output(do_abc_tag.buffer.size()*2);
		BufferWriter writer(output);
		actionscript.write_to(writer);
		output.resize(writer.raw_ptr() - &output[0]);
		TRACE << "new DoABC size: " << output.size() << std::endl;
		do_abc_tag.buffer = std::move(output);
	}
	TRACE << "writing file" << std::endl;
	tfm.write_to(output_fname);
	TRACE << "finished writing file" << std::endl;
}
