cmake_minimum_required(VERSION 3.27...3.29)

project(tpcpp CXX)

set(CMAKE_CXX_STANDARD 17)


#############
# libraries #
#############


###############
# executables #
###############

add_executable(${PROJECT_NAME} "src/main.cpp" "src/Interpreter.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "inc")


install(TARGETS ${PROJECT_NAME} DESTINATION bin)


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
