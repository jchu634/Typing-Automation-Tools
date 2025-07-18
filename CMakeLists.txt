# CMakeList.txt : CMake project for WPM-Simulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Typing Automation Tools")

# Build key_sim_lib as a shared library (DLL)
add_library(key_sim_lib SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/src/key_sim_lib.cpp"
    #"${CMAKE_CURRENT_SOURCE_DIR}/include/key_sim_lib.h"
)
target_include_directories(key_sim_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Define KEY_SIM_EXPORTS for the DLL build
target_compile_definitions(key_sim_lib PRIVATE KEY_SIM_EXPORTS)

# Type-at-wpm executable
add_executable("type-at-wpm"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/type-at-wpm.cpp"
)
target_link_libraries("type-at-wpm" PRIVATE key_sim_lib)
target_include_directories("type-at-wpm" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Type-clipboard executable
add_executable("type-clipboard" WIN32
    "${CMAKE_CURRENT_SOURCE_DIR}/src/type-clipboard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.rc"
    # "${CMAKE_CURRENT_SOURCE_DIR}/resources/resource.h"
)
target_link_libraries("type-clipboard" PRIVATE key_sim_lib)
target_include_directories("type-clipboard" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/resources")
target_include_directories("type-clipboard" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Set C++20 for all targets
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET key_sim_lib PROPERTY CXX_STANDARD 20)
    set_property(TARGET "type-at-wpm" PROPERTY CXX_STANDARD 20)
    set_property(TARGET "type-clipboard" PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
