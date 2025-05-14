cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Add source to this project's executable.
add_executable (Engine "src/Engine.cpp" "src/Engine.h")

# Recursively collect all .cpp and .h files in src/
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.h
)

target_include_directories(Engine PRIVATE ${CMAKE_SOURCE_DIR}/src)

#Get VCPKG
find_package(SFML REQUIRED COMPONENTS System Window Graphics)
find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)

target_link_libraries(Engine
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        libzmq
        cppzmq
)
