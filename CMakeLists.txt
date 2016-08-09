cmake_minimum_required(VERSION 2.8)

project(lep)

set(lep_MAJOR_VERSION 10)
set(lep_MINOR_VERSION 1)

option(SERVER "Compile LEP Server" ON)
option(SERVER_DEVICES "Compile LEP Server with Devices" ON)
option(SERVER_AUDIO "Compile LEP Server with Audio Output" OFF)

option(SPEECH "Compile LEP Speech Client" OFF)
option(GUI "Compile LEP GUI Client" OFF)
option(TEXT "Compile LEP Text Client" ON)

include_directories("${/PROJECT_SOURCE_DIR}/src")

add_subdirectory(src)
