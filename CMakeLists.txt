cmake_minimum_required(VERSION 3.21)

PROJECT(dmbacktrace)

INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

ADD_DEFINITIONS(-DUSE_DMBACKTRACE)

ModuleImport("dmformat" "thirdparty/dmformat")

LibImport("libdmbacktrace" "src/libdmbacktrace")

if(PROJECT_IS_TOP_LEVEL)
    ExeImport("test" "libdmbacktrace")

    if(NOT WIN32)
        target_link_options(dmbacktracetest PRIVATE "-rdynamic")
    endif()
endif()
