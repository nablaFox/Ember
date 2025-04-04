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
	find_program(GLSLC glslc REQUIRED)

	file(GLOB ALL_SHADER_SOURCES
	  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
	  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
	  "${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders/*.vert"
	  "${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders/*.frag"
	)

	set(SHADER_SPV_FILES)

	foreach(SHADER_SOURCE ${ALL_SHADER_SOURCES})
	  file(RELATIVE_PATH SHADER_REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_SOURCE})
	  set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_REL_PATH}.spv")
	  get_filename_component(SHADER_OUTPUT_DIR ${SHADER_OUTPUT} DIRECTORY)
	  file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})
	  add_custom_command(
		OUTPUT ${SHADER_OUTPUT}
		COMMAND ${GLSLC} ${SHADER_SOURCE} -o ${SHADER_OUTPUT}
		DEPENDS ${SHADER_SOURCE}
		COMMENT "Compiling ${SHADER_REL_PATH} to SPIR-V"
	  )
	  list(APPEND SHADER_SPV_FILES ${SHADER_OUTPUT})
	endforeach()

	file(GLOB EXAMPLES_SRC "examples/*.cpp")

	add_custom_target(compile_shaders ALL DEPENDS ${SHADER_SPV_FILES})

	foreach(EXAMPLE_SRC ${EXAMPLES_SRC})
		get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
		add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
		target_link_libraries(${EXAMPLE_NAME} PRIVATE etna)
		target_include_directories(${EXAMPLE_NAME} PRIVATE "include")
	endforeach()
endif()
