cmake_minimum_required(VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Указываем имя проекта
project("ScreenTime")

# Ищем библиотеку jsoncpp, установленную через vcpkg
find_package(jsoncpp CONFIG REQUIRED)

# Проверка, что библиотека найдена
if (NOT jsoncpp_FOUND)
  message(FATAL_ERROR "jsoncpp not found")
endif()

# Создаем исполняемый файл
add_executable(ScreenTime ScreenTime.cpp ScreenTime.h main.cpp)

# Линкуем библиотеку jsoncpp к проекту
target_link_libraries(ScreenTime PRIVATE JsonCpp::JsonCpp)  # Используйте правильное имя библиотеки!

# Устанавливаем стандарт C++20 (если версия CMake >= 3.12)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ScreenTime PROPERTY CXX_STANDARD 20)
endif()
