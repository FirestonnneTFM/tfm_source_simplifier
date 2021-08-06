add_subdirectory(swf)
target_include_directories (tfm_src_simple PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(tfm_src_simple PUBLIC
	main.cpp
	Buffer.cpp
	Buffer.hpp
	HungarianMatch.cpp
	HungarianMatch.hpp
	MaxBipartiteMatch.cpp
	MaxBipartiteMatch.hpp
	common.cpp
	common.hpp
	TB_rename_box2d.cpp
	TB_rename_box2d_table.hpp
	TB_rename_identifiers.cpp
	TB_simplify_expressions.cpp
	ToolBox.hpp
)
