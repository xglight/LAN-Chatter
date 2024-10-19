cmake_minimum_required(VERSION 3.5.0)
project(LAN-CHATTER VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_executable(server server.cpp)
add_executable(client client.cpp)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(PROJECT_SOURCE
  server.cpp
  client.cpp
)

target_link_libraries(server wsock32 ws2_32)
target_link_libraries(client wsock32 ws2_32)
