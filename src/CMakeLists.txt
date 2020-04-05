add_subdirectory(swf)
target_include_directories (tfm_src_simple PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(tfm_src_simple PUBLIC
	main.cpp
	Buffer.cpp
	Buffer.hpp
	common.cpp
	common.hpp
	TB_rename_by_trait.cpp
	TB_rename_identifiers.cpp
	TB_simplify_expressions.cpp
	ToolBox.hpp
)
