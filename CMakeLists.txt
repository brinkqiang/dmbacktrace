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

    include(CheckLinkerFlag)

    check_linker_flag(CXX "-rdynamic" CMAKE_LINKER_SUPPORTS_RDYNAMIC)

    # 3. 如果检查通过，则为目标添加该选项
    if(CMAKE_LINKER_SUPPORTS_RDYNAMIC)
        target_link_options(dmbacktracetest PRIVATE "-rdynamic")
    endif()
endif()
