cmake_minimum_required(VERSION 3.27...3.29)

project(template-project-cpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS off)
# ^to force gcc into `-std=c++20`, instead of the default `-std=gnu++20`
set(CMAKE_CXX_STANDARD_REQUIRED on)
# ^so that compile_commands.json contains `-std=c++20`


#############
# libraries #
#############

add_library(tpcpp-object STATIC "src/Object.cpp")
target_include_directories(tpcpp-object PUBLIC "inc")
target_compile_options(tpcpp-object PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/wd4702 /wd4804 /wd4805>
)

file(GLOB src_tpcpp_parsing CONFIGURE_DEPENDS "src/parsing/*.cpp")
file(GLOB src_tpcpp_visitors CONFIGURE_DEPENDS "src/visitors/*.cpp")
add_library(tpcpp-interpreter STATIC
    ${src_tpcpp_parsing}
    ${src_tpcpp_visitors}
    "src/Interpreter.cpp"
)
target_include_directories(tpcpp-interpreter PUBLIC "inc")
target_link_libraries(tpcpp-interpreter PUBLIC tpcpp-object)


###############
# executables #
###############

add_executable(tpcpp "src/main.cpp")
target_link_libraries(tpcpp tpcpp-interpreter)


install(TARGETS tpcpp DESTINATION bin)


########
# test #
########

enable_testing()


# for tpcpp-object
file(GLOB src_test_tpcpp_object CONFIGURE_DEPENDS "test/tpcpp-object/*.cpp")
foreach(s ${src_test_tpcpp_object})
    get_filename_component(name ${s} NAME_WE)

    add_executable(${name} ${s})
    target_link_libraries(${name} tpcpp-object)

    target_include_directories(${name} PUBLIC "test")

    add_test(NAME ${name} COMMAND ${name})
endforeach()


# for tpcpp-interpreter
file(GLOB src_test_tpcpp_interpreter CONFIGURE_DEPENDS "test/tpcpp-interpreter/*.cpp")
foreach(s ${src_test_tpcpp_interpreter})
    get_filename_component(name ${s} NAME_WE)

    add_executable(${name} ${s})
    target_link_libraries(${name} tpcpp-interpreter)

    target_include_directories(${name} PUBLIC "test")

    add_test(NAME ${name} COMMAND ${name})
endforeach()


################
# extra config #
################

# treat warnings as errors for all targets
get_property(current_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
foreach(tgt ${current_targets})
    target_compile_options(${tgt} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
endforeach()


#######
# doc #
#######
# NOTE: this has to come after the 'extra config' section

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    set(DOXYGEN_WARN_AS_ERROR NO)
    set(DOXYGEN_FILE_PATTERN ".cpp;.hpp")

    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_STRIP_FROM_INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/inc")

    set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)

    # doxygen-awesome
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_DISABLE_INDEX NO)
    set(DOXYGEN_FULL_SIDEBAR NO)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET
        "doc/doxygen-awesome-css/doxygen-awesome.css"
        "doc/doxygen-awesome-css/doxygen-awesome-sidebar-only.css"
    )
    set(DOXYGEN_HTML_COLORSTYLE "LIGHT")

    # graphviz
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_COLLABORATION_GRAPH YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT "svg")

    doxygen_add_docs(doc
        "inc" "src" "README.md"
        "doc/debug.md"
        "doc/dev.md"
        COMMENT "Generate API documentation"
    )
endif()
