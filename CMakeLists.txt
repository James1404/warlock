cmake_minimum_required(VERSION 3.27.7)

project(meridian VERSION 0.01)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

add_library(meridian)
add_executable(meridian_exe)
set_property(TARGET meridian_exe PROPERTY OUTPUT_NAME meridian)

if(MSVC)
  target_compile_options(meridian_exe PRIVATE /W4)
else()
  target_compile_options(meridian_exe PRIVATE -Wall -Wpedantic -Werror)
endif()

set(meridian_CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB meridian_src
	${meridian_CURRENT_DIR}/src/*.cpp
	${meridian_CURRENT_DIR}/src/*.c
)

target_sources(meridian PRIVATE ${meridian_src})
target_sources(meridian_exe PRIVATE ${meridian_src})

target_include_directories(meridian PUBLIC
    ${meridian_CURRENT_DIR}/include
)

target_include_directories(meridian_exe PUBLIC
    ${meridian_CURRENT_DIR}/include
)

add_custom_command(
    TARGET meridian_exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${meridian_CURRENT_DIR}/test
            ${CMAKE_CURRENT_BINARY_DIR}/test)
