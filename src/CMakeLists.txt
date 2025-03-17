cmake_minimum_required(VERSION 3.10)
project(AdvancedChatServer)

# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Set output directories ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

# --- Windows-specific definitions ---
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7 or later
endif()

# --- Define the executable and its sources ---
add_executable(AdvancedChatServer
    main.cpp
    Server.cpp
    Session.cpp
    UserManager.cpp
    SessionManager.cpp
    ChatRoom.cpp
    ChatRoomManager.cpp
    Database.cpp
    ThreadPool.cpp
    Logging.cpp
)

# --- Find Other Dependencies ---
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# --- Target-Specific Includes and Linking ---
target_include_directories(AdvancedChatServer PRIVATE 
    "C:/vcpkg/installed/x64-windows/include"
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against libraries
target_link_libraries(AdvancedChatServer PRIVATE
    nlohmann_json::nlohmann_json
    unofficial::sqlite3::sqlite3
)

# Copy the executable to the build directory
add_custom_command(TARGET AdvancedChatServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:AdvancedChatServer> ${CMAKE_BINARY_DIR}
)
