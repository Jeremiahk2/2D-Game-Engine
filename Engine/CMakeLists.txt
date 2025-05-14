cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${CMAKE_PROJECT_NAME}")

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )


#Get VCPKG
find_package(SFML REQUIRED COMPONENTS System Window Graphics)
find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        libzmq
        cppzmq
)
