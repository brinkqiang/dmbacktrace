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
        include(CheckLinkerFlag)
        check_linker_flag(CXX "-rdynamic" CMAKE_LINKER_SUPPORTS_RDYNAMIC)
        if(CMAKE_LINKER_SUPPORTS_RDYNAMIC)
            target_link_options(dmbacktracetest PRIVATE "-rdynamic")
            target_link_options(dmbacktrace_easytest PRIVATE "-rdynamic")
        endif()
    endif()
endif()
