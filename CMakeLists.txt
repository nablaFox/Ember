cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(etna)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ETNA_BUILD_EXAMPLES "Build the examples" ON)

file(GLOB ETNA_SRC "src/*.cpp")
add_library(etna STATIC ${ETNA_SRC})

# Dependencies
CPMAddPackage(
  NAME Ignis
  GITHUB_REPOSITORY nablaFox/Ignis
  GIT_TAG main
  OPTIONS
  	"IGNIS_BUILD_EXAMPLES OFF"
)

CPMAddPackage(
  NAME glfw
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.3.8
  OPTIONS
    "GLFW_BUILD_EXAMPLES OFF"
    "GLFW_BUILD_TESTS OFF"
    "GLFW_BUILD_DOCS OFF"
    "BUILD_SHARED_LIBS ON"
	"GLFW_INSTALL FALSE"
)

install(TARGETS glfw LIBRARY DESTINATION lib)

target_link_libraries(etna PUBLIC
	glfw
	Ignis::Ignis
)

target_include_directories(etna PUBLIC 
	"include/etna" 
	${Ignis_SOURCE_DIR}/include
)

if(ETNA_BUILD_EXAMPLES)
	file(GLOB EXAMPLES_SRC "examples/*.cpp")

	foreach(EXAMPLE_SRC ${EXAMPLES_SRC})
		get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
		add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
		target_link_libraries(${EXAMPLE_NAME} PRIVATE etna)
		target_include_directories(${EXAMPLE_NAME} PRIVATE "include")
	endforeach()
endif()
