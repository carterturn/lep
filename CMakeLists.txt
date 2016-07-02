cmake_minimum_required(VERSION 2.8)

project(lep)

set(lep_MAJOR_VERSION 10)
set(lep_MINOR_VERSION 1)

include_directories("${/PROJECT_SOURCE_DIR}/src")

add_subdirectory(src)
