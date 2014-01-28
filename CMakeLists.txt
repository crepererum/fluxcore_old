# setup cmake
cmake_minimum_required (VERSION 2.8)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/extern/cotire/CMake")
include (cotire)

# project metadata
project (fluxcore)

# configure compiler
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# detect clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics -fdiagnostics-show-category=name")
endif ()

# set include paths
include_directories ("src" "extern/bandit")

# build libfluxcore
file (GLOB_RECURSE SourceFilesLib "src/fluxcore/*.cpp")
add_library (fluxcore ${SourceFilesLib})
cotire (fluxcore)

# build test
file (GLOB_RECURSE SourceFilesTest "tests/*.cpp")
add_executable (fluxtest EXCLUDE_FROM_ALL ${SourceFilesTest})
target_link_libraries (fluxtest fluxcore)
add_custom_target (test COMMAND fluxtest "--reporter=spec" DEPENDS fluxtest)
cotire (test)

# generate docs
string (STRIP "${CMAKE_CXX_FLAGS}" DocsCxxFlags)
string (REPLACE " " ";" DocsCxxFlags "${DocsCxxFlags}")
get_directory_property (tmp INCLUDE_DIRECTORIES)
foreach (item ${tmp})
    list (APPEND DocsCxxFlags "-I${item}")
endforeach ()
file (GLOB_RECURSE HeaderFilesLib "src/fluxcore/*.hpp")
add_custom_target (docs COMMAND cldoc "generate" ${DocsCxxFlags} "--" "--output=docs" ${HeaderFilesLib})

