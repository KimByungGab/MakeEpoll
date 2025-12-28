add_library(Epoll STATIC
	NetworkCore.cpp
	Client.cpp
	RingBuffer.cpp
)

target_include_directories(Epoll PUBLIC
	${CMAKE_SOURCE_DIR}
)
