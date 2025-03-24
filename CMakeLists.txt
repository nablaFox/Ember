cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(etna)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB ETNA_SRC "src/*.cpp")
add_library(etna STATIC ${ETNA_SRC})

# Dependencies
CPMAddPackage(
  NAME Ignis
  GITHUB_REPOSITORY nablaFox/Ignis
  GIT_TAG main
)

find_package(glfw3 REQUIRED)

target_link_libraries(etna PUBLIC
	glfw
	Ignis::Ignis
)

target_include_directories(etna PUBLIC 
	"include/etna" 
	${Ignis_SOURCE_DIR}/include
)

# Build examples
option(ETNA_BUILD_EXAMPLES "Build the examples" ON)

if(NOT ETNA_BUILD_EXAMPLES)
	return()
endif()

file(GLOB ETNA_SRC "examples/*.cpp")

foreach(ETNA_SRC ${EXAMPLES_SRC})
	get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
	add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
	target_link_libraries(${EXAMPLE_NAME} PRIVATE etna)
	target_include_directories(${EXAMPLE_NAME} PRIVATE "include")
endforeach()
