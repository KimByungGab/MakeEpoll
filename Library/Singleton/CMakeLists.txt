add_library(Singleton STATIC
	Singleton.cpp
)

target_include_directories(Singleton PUBLIC
	${CMAKE_SOURCE_DIR}
)
