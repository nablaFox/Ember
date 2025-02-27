cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(ember)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB EMBER_SRC "src/*.cpp")
add_library(ember STATIC ${EMBER_SRC})

# Dependencies
CPMAddPackage(
  NAME Ignis
  GITHUB_REPOSITORY nablaFox/Ignis
  GIT_TAG main
)

find_package(glfw3 REQUIRED)

target_link_libraries(ember PUBLIC
	glfw
	Ignis::Ignis
)

target_include_directories(ember PUBLIC 
	"include/ember" 
	${Ignis_SOURCE_DIR}/include
)

# Build examples
option(EMBER_BUILD_EXAMPLES "Build the examples" ON)

if(NOT EMBER_BUILD_EXAMPLES)
	return()
endif()

file(GLOB EXAMPLES_SRC "examples/*.cpp")

foreach(EXAMPLE_SRC ${EXAMPLES_SRC})
	get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
	add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
	target_link_libraries(${EXAMPLE_NAME} PRIVATE ember)
	target_include_directories(${EXAMPLE_NAME} PRIVATE "include")
endforeach()
