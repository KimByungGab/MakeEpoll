cmake_minimum_required(VERSION 3.10)
project(MyServers CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 하위 디렉토리 포함
add_subdirectory(Library)
add_subdirectory(GameServer)
