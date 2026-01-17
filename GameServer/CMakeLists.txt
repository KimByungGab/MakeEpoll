# Position Independent Code 설정 (공유 라이브러리 생성을 위해 필요)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(Object)
add_subdirectory(Manager)
add_subdirectory(Packet)

# GameServer 실행 파일 생성
add_executable(GameServer
	GameServer.cpp
	ServiceMain.cpp
	GameService.cpp
	${OBJECT_DIR_SOURCES}
	${MANAGER_DIR_SOURCES}
	${PACKET_DIR_SOURCES}
)

# 라이브러리 링크 및 인클루드 디렉토리 설정
target_link_libraries(GameServer PRIVATE 
	Epoll
	Ini
	Singleton
	DataArchive
)

# include 디렉토리 설정을 맨 위로 하여 Library 접근 시 가독성이 좋게 명시함
target_include_directories(GameServer PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)