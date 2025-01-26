cmake_minimum_required(VERSION 3.10)
project(Distributed_chat)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for headers
include_directories(include)

# Add executable
add_executable(Distributed_chat 
src/main.c
src/network.c
src/chat_handler.c
src/file_transfer.c
)
