cmake_minimum_required(VERSION 3.10)
project(AdvancedChatServer)

# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Set CMP0074 Policy ---
cmake_policy(SET CMP0074 NEW)

# --- vcpkg Integration and Dependency Paths ---
if(NOT DEFINED VCPKG_ROOT)
    set(VCPKG_ROOT "C:/vcpkg")
endif()
set(BOOST_ROOT "${VCPKG_ROOT}/installed/x64-mingw-static")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_NO_BOOST_CMAKE ON)

# --- Find Boost --- 
# Include additional components required by Boost.Log
find_package(Boost REQUIRED COMPONENTS system thread log log_setup date_time)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could NOT find Boost. Check BOOST_ROOT and BOOST_LIBRARYDIR!")
else()
    message(STATUS "Found Boost version ${Boost_VERSION} in: ${BOOST_LIBRARYDIR}")
endif()

# --- Find Other Dependencies ---
set(nlohmann_json_DIR "${VCPKG_ROOT}/installed/x64-mingw-static/share/nlohmann_json")
find_package(nlohmann_json REQUIRED)

# SQLite3: Setup include and library paths
set(SQLite3_INCLUDE_DIR "${VCPKG_ROOT}/installed/x64-mingw-static/include")
set(SQLite3_LIBRARY "${VCPKG_ROOT}/installed/x64-mingw-static/lib/libsqlite3.a")
find_package(SQLite3 REQUIRED)

# --- Define the executable and its sources ---
add_executable(main
    main.cpp
    Server.cpp
    Session.cpp
    UserManager.cpp
    SessionManager.cpp
    ChatRoom.cpp
    ChatRoomManager.cpp
    Database.cpp
    ThreadPool.cpp  # ThreadPool implementation in a separate source file
)

# --- Target-Specific Includes and Linking ---
target_include_directories(main PRIVATE 
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SQLite3_INCLUDE_DIR}
)
target_link_libraries(main PRIVATE
    ${Boost_LIBRARIES}                # Now includes system, thread, log, log_setup, date_time
    nlohmann_json::nlohmann_json
    ${SQLite3_LIBRARY}
)

# Define BOOST_DATE_TIME_NO_LIB to disable auto-linking for Boost.DateTime
target_compile_definitions(main PRIVATE BOOST_DATE_TIME_NO_LIB)
