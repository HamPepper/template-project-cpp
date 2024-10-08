cmake_minimum_required(VERSION 3.27...3.29)

project(template-project-cpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS off)
# ^to force gcc into `-std=c++17`, instead of the default `-std=gnu++17`
set(CMAKE_CXX_STANDARD_REQUIRED on)
# ^so that compile_commands.json contains `-std=c++17`


#############
# libraries #
#############

add_library(tpcpp-object STATIC "src/Object.cpp")
target_include_directories(tpcpp-object PUBLIC "inc")


###############
# executables #
###############

add_executable(tpcpp "src/main.cpp" "src/Interpreter.cpp")
target_include_directories(tpcpp PUBLIC "inc")


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
endforeach(s ${src_test_tpcpp_object})


################
# extra config #
################

# treat warnings as errors for all targets
get_property(current_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
foreach(tgt ${current_targets})
    target_compile_options(${tgt} PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
endforeach()


#######
# doc #
#######
# NOTE: this has to come after the 'extra config' section

find_package(Doxygen)

if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    add_custom_target(doc ALL
                      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
endif()
