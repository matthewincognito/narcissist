cmake_minimum_required(VERSION 3.7)
project(narcissist)

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

add_subdirectory(narcissist)
add_subdirectory(narcissist-cli)
