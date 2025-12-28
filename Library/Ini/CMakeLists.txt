add_library(Ini STATIC
	Ini.cpp
)

target_include_directories(Ini PUBLIC
	${CMAKE_SOURCE_DIR}
)
