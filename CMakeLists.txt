cmake_minimum_required(VERSION 3.21)

PROJECT(dmbacktrace)

INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

ADD_DEFINITIONS(-DUSE_DMBACKTRACE)

ModuleImport("dmformat" "thirdparty/dmformat")

LibImport("dmbacktrace" "src")

ExeImport("test" "dmbacktrace")
