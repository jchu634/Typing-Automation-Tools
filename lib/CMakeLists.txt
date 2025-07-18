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
add_library(key_sim_lib SHARED "key_sim_lib.cpp" "key_sim_lib.h")
target_include_directories(key_sim_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Define KEY_SIM_EXPORTS for the DLL build
target_compile_definitions(key_sim_lib PRIVATE KEY_SIM_EXPORTS)

# Type-at-wpm executable
add_executable ("type-at-wpm" "type-at-wpm.cpp" "type-at-wpm.h")
target_link_libraries("type-at-wpm" PRIVATE key_sim_lib)

# Type-clipboard executable
add_executable ("type-clipboard" WIN32 "type-clipboard.cpp" "resources.rc" "resource.h")
target_link_libraries("type-clipboard" PRIVATE key_sim_lib)

# Set C++20 for all targets
if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET key_sim_lib PROPERTY CXX_STANDARD 20)  
	set_property(TARGET "type-at-wpm" PROPERTY CXX_STANDARD 20)
	set_property(TARGET "type-clipboard" PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
