# CMakeList.txt : CMake project for WPM-Simulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("WPM-Simulator")

# Build key_sim_lib as a shared library (DLL)
add_library(key_sim_lib SHARED "key_sim_lib.cpp" "key_sim_lib.h")
target_include_directories(key_sim_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Define KEY_SIM_EXPORTS for the DLL build
target_compile_definitions(key_sim_lib PRIVATE KEY_SIM_EXPORTS)

# Main executable
add_executable (WPM-Simulator "WPM-Simulator.cpp" "WPM-Simulator.h")
target_link_libraries(WPM-Simulator PRIVATE key_sim_lib)

# Clipboard-Paster executable
add_executable (Clipboard-Paster "Clipboard-Paster.cpp" "Clipboard-Paster.h")
target_link_libraries(Clipboard-Paster PRIVATE key_sim_lib)

# Set C++20 for all targets
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WPM-Simulator PROPERTY CXX_STANDARD 20)
  set_property(TARGET key_sim_lib PROPERTY CXX_STANDARD 20)
  set_property(TARGET Clipboard-Paster PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
