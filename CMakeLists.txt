cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(etna)

include(cmake/CPM.cmake)
include(cmake/CompileShaders.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ETNA_BUILD_EXAMPLES "Build the examples" ${PROJECT_IS_TOP_LEVEL})
option(ETNA_INSTALL "Install the library" ${PROJECT_IS_TOP_LEVEL})

find_program(GLSLC glslc REQUIRED)

file(GLOB ETNA_SRC "src/*.cpp")
add_library(etna STATIC ${ETNA_SRC})

target_include_directories(etna PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(etna PUBLIC
  ETNA_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_options(etna PRIVATE
  $<$<CONFIG:Release>:-O3 -ffunction-sections -fdata-sections>
  $<$<CONFIG:Debug>:-Wall>
)

CPMAddPackage(
  NAME ignis
  GITHUB_REPOSITORY nablaFox/ignis
  GIT_TAG main
)

CPMAddPackage(
  NAME glfw
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.3.8
  OPTIONS
    "GLFW_BUILD_EXAMPLES OFF"
    "GLFW_BUILD_TESTS OFF"
    "GLFW_BUILD_DOCS OFF"
    "GLFW_INSTALL FALSE"
)

target_sources(etna PRIVATE
  $<TARGET_OBJECTS:ignis>
)

target_include_directories(etna PUBLIC
  ${ignis_SOURCE_DIR}/include
  ${glfw_SOURCE_DIR}/include
)

target_link_libraries(etna 
  PRIVATE glfw
  PUBLIC ignis
)

compile_shaders("src" "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
add_dependencies(etna compile_shaders_src)

if (ETNA_INSTALL)
  install(TARGETS etna
    EXPORT etnaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )

  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(DIRECTORY ${ignis_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(FILES LICENSE
    DESTINATION .
  )

  install(TARGETS glfw
    EXPORT etnaTargets
    ARCHIVE DESTINATION lib
  )
endif()

if(ETNA_BUILD_EXAMPLES)
  compile_shaders("examples" "${CMAKE_CURRENT_SOURCE_DIR}/examples/shaders" "${CMAKE_CURRENT_BINARY_DIR}/shaders")

  file(GLOB EXAMPLES_SRC "examples/*.cpp")

  foreach(EXAMPLE_SRC ${EXAMPLES_SRC})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})
    add_dependencies(${EXAMPLE_NAME} compile_shaders_examples)
    target_link_libraries(${EXAMPLE_NAME} PRIVATE etna)
    target_include_directories(${EXAMPLE_NAME} PRIVATE "include")
    target_link_options(${EXAMPLE_NAME} PRIVATE -Wl,--gc-sections)
  endforeach()
endif()
