add_library(DataArchive STATIC
	DataArchive.cpp
)

target_include_directories(DataArchive PUBLIC
	${CMAKE_SOURCE_DIR}
)
