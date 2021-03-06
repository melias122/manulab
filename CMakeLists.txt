cmake_minimum_required(VERSION 3.4.0 FATAL_ERROR)

project(ManuLab)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Automatically manage .qrc files
set(CMAKE_AUTORCC ON)

# TODO: Get rid of this..
set(CMAKE_CXX_FLAGS "-std=c++11")

# Export compile commands for tools that need it
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(clangformat)

if(APPLE AND EXISTS /usr/local/opt/qt5)
	# Homebrew installs Qt5 (up to at least 5.9.1) in
	# /usr/local/qt5, ensure it can be found by CMake since
	# it is not in the default /usr/local prefix.
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()

find_package(Qt5 COMPONENTS Core Widgets CONFIG REQUIRED)

include_directories(src)
add_subdirectory(src)
