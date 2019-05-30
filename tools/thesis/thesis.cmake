cmake_minimum_required (VERSION 3.6)
project (thesis)
add_executable(thesis thesis.cpp)

configure_file (
	"${PROJECT_SOURCE_DIR}/*.c
	"${PROJECT_HEADER_DIR}/*.c
include_directories()
