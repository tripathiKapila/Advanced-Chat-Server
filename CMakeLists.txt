cmake_minimum_required(VERSION 3.10)
project(AdvancedChatServer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force output directory for all configurations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Define _WIN32_WINNT for Windows
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Find Boost
find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

# Add source files for the server
set(SOURCES
    src/main.cpp
    src/Logging.cpp
    src/ChatRoom.cpp
    src/Session.cpp
    src/SessionManager.cpp
    src/UserManager.cpp
    src/Command.cpp
    src/Commands.cpp
    src/ThreadPool.cpp
)

# Add executable for the server
add_executable(ChatServer ${SOURCES})

# Link libraries
target_link_libraries(ChatServer ${Boost_LIBRARIES})

# Include directories
target_include_directories(ChatServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src) 