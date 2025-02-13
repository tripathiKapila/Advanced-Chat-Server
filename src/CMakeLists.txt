cmake_minimum_required(VERSION 3.10)
project(AdvancedChatServer)

# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Set CMP0074 Policy ---
# This makes find_package honor BOOST_ROOT, BOOST_INCLUDEDIR, etc.
cmake_policy(SET CMP0074 NEW)

# --- vcpkg Integration and Boost Settings ---
# Set these to your vcpkg installed location for the x64-mingw-static triplet.
set(BOOST_ROOT "C:/vcpkg/installed/x64-mingw-static")
set(BOOST_INCLUDEDIR "C:/vcpkg/installed/x64-mingw-static/include")
set(BOOST_LIBRARYDIR "C:/vcpkg/installed/x64-mingw-static/lib")

# Force CMake to search only in the provided paths
set(Boost_NO_SYSTEM_PATHS ON)

# Options to force static linking (if desired)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# For MinGW with vcpkg, the Boost libraries might be built with an extra tag.
# If your library names are, for example: libboost_system-mt-x64-1_86.a, then you might not need this hint.
# Comment out or adjust the following line as needed:
# set(Boost_COMPILER "-mgw-mt")

# (Optional) Enable verbose Boost debug output for troubleshooting
#set(Boost_DEBUG ON)

# --- Find Boost ---
find_package(Boost REQUIRED COMPONENTS system thread log log_setup)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could NOT find Boost. Check BOOST_ROOT, BOOST_LIBRARYDIR, and ensure the required libraries exist!")
else()
    message(STATUS "Found Boost version ${Boost_VERSION} in: ${BOOST_LIBRARYDIR}")
endif()

# --- Find Other Dependencies ---
# nlohmann_json: Set the directory where its config file is located.
set(nlohmann_json_DIR "C:/vcpkg/installed/x64-mingw-static/share/nlohmann_json")
find_package(nlohmann_json REQUIRED)

# SQLite3: Ensure the paths are correct.
set(SQLite3_INCLUDE_DIR "C:/vcpkg/installed/x64-mingw-static/include")
set(SQLite3_LIBRARY "C:/vcpkg/installed/x64-mingw-static/lib/libsqlite3.a")
find_package(SQLite3 REQUIRED)

# --- Include Directories ---
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("C:/vcpkg/installed/x64-mingw-static/include")

# --- Define the Executable and Its Sources ---
add_executable(main
    main.cpp
    Server.cpp
    Session.cpp
    UserManager.cpp
    SessionManager.cpp
    ChatRoom.cpp
    ChatRoomManager.cpp
    Database.cpp   # SQLite integration file
)

# --- Link Libraries ---
target_link_libraries(main
    ${Boost_LIBRARIES}
    nlohmann_json::nlohmann_json
    ${SQLite3_LIBRARY}
)
