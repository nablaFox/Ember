cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(ember)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB EMBER_SRC "src/*.cpp")

add_executable(Ember ${EMBER_SRC})

# Dependencies
CPMAddPackage(
  NAME Ignis
  GITHUB_REPOSITORY nablaFox/Ignis
  GIT_TAG main
)

find_package(glfw3 REQUIRED)

target_link_libraries(Ember PRIVATE 
	glfw
	Ignis::Ignis
)

target_include_directories(Ember PUBLIC 
	"include" 
	${Ignis_SOURCE_DIR}/include
)
