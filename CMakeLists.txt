cmake_minimum_required(VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("ScreenTime")

find_package(jsoncpp CONFIG REQUIRED)

if (NOT jsoncpp_FOUND)
  message(FATAL_ERROR "jsoncpp not found")
endif()

add_executable(ScreenTime ScreenTime.cpp ScreenTime.h main.cpp)

target_link_libraries(ScreenTime PRIVATE JsonCpp::JsonCpp) 

set_target_properties(ScreenTime PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ScreenTime PROPERTY CXX_STANDARD 20)
endif()
