cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )
set (DEBUG TRUE)


project(Regex)
enable_language(CXX)
set (CMAKE_CXX_STANDARD 17)


if(DEBUG)
    set(CMAKE_CXX_FLAGS "-ggdb")
endif()


get_filename_component(APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
include_directories("${APP_PATH}/include")

set(SRC_FILES
        "include/Automata/dfa.h"
        "src/Automata/dfa.cpp"

        "include/Automata/nfa.h"
        "src/Automata/nfa.cpp"

        "include/Regex/parse.h"
        "include/Regex/tokens.h"
)


add_library(ebic_regex STATIC ${SRC_FILES})
target_compile_features(ebic_regex PUBLIC
  cxx_attributes
  cxx_defaulted_functions
  cxx_deleted_functions
  cxx_final)
target_include_directories(ebic_regex PUBLIC "${APP_PATH}/include")
set_target_properties(ebic_regex PROPERTIES LINKER_LANGUAGE CXX)

if(DEBUG)
    add_executable(ebic_regex_tests "${APP_PATH}/main.cpp")
    target_link_libraries(ebic_regex_tests ebic_regex)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, "${APP_PATH}/bin")
endif()
